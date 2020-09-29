
#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "bogaudio.hpp"
#include "dsp/analyzer.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct ChannelAnalyzer {
	SpectrumAnalyzer _analyzer;
	int _binsN;
	float* _bins0;
	float* _bins1;
	float* _currentBins;
	std::atomic<float*>& _currentOutBuf;
	AveragingBuffer<float>* _averagedBins;
	const int _stepBufN;
	float* _stepBuf;
	int _stepBufI = 0;
	const int _workerBufN;
	float* _workerBuf;
	int _workerBufWriteI = 0;
	int _workerBufReadI = 0;
	bool _workerStop = false;
	std::mutex _workerMutex;
	std::condition_variable _workerCV;
	std::thread _worker;

	ChannelAnalyzer(
		SpectrumAnalyzer::Size size,
		SpectrumAnalyzer::Overlap overlap,
		SpectrumAnalyzer::WindowType windowType,
		float sampleRate,
		int averageN,
		int binSize,
		float* outBuf1,
		float* outBuf2,
		std::atomic<float*>& currentOutBuf
	)
	: _analyzer(size, overlap, windowType, sampleRate, false)
	, _binsN(size / binSize)
	, _bins0(outBuf1)
	, _bins1(outBuf2)
	, _currentBins(_bins0)
	, _currentOutBuf(currentOutBuf)
	, _averagedBins(averageN == 1 ? NULL : new AveragingBuffer<float>(_binsN, averageN))
	, _stepBufN(size / overlap)
	, _stepBuf(new float[_stepBufN] {})
	, _workerBufN(size + 1)
	, _workerBuf(new float[_workerBufN] {})
	, _worker(&ChannelAnalyzer::work, this)
	{
		assert(averageN >= 1);
		assert(binSize >= 1);
	}
	virtual ~ChannelAnalyzer();

	void step(float sample);
	void work();
};

struct AnalyzerCore {
	enum Quality {
		QUALITY_ULTRA,
		QUALITY_HIGH,
		QUALITY_GOOD,
		QUALITY_FIXED_16K,
		QUALITY_FIXED_32K,
		QUALITY_ULTRA_ULTRA
	};

	enum Window {
		WINDOW_NONE,
		WINDOW_HAMMING,
		WINDOW_KAISER
	};

	int _nChannels;
	ChannelAnalyzer** _channels;
	SpectrumAnalyzer::Size _size;
	const int _binAverageN = 2;
	const int _outBufferN = SpectrumAnalyzer::maxSize / _binAverageN;
	int _binsN;
	float* _outBufs;
	std::atomic<float*>* _currentOutBufs;
	int _averageN = 1;
	Quality _quality = QUALITY_GOOD;
	Window _window = WINDOW_KAISER;
	SpectrumAnalyzer::Overlap _overlap = SpectrumAnalyzer::OVERLAP_2;
	std::mutex _channelsMutex;

	AnalyzerCore(int nChannels, SpectrumAnalyzer::Overlap overlap = SpectrumAnalyzer::OVERLAP_2)
	: _nChannels(nChannels)
	, _channels(new ChannelAnalyzer*[_nChannels] {})
	, _outBufs(new float[2 * nChannels * _outBufferN] {})
	, _currentOutBufs(new std::atomic<float*>[nChannels])
	, _overlap(overlap)
	{
		for (int i = 0; i < nChannels; ++i) {
			_currentOutBufs[i] = _outBufs + 2 * i * _outBufferN;
		}
	}
	virtual ~AnalyzerCore() {
		resetChannels();
		delete[] _channels;
		delete[] _outBufs;
		delete[] _currentOutBufs;
	}

	void setParams(int averageN, Quality quality, Window window);
	void resetChannels();
	SpectrumAnalyzer::Size size();
	SpectrumAnalyzer::WindowType window();
	inline float* getBins(int i) {
		assert(i >= 0 && i < _nChannels);
		return _currentOutBufs[i];
	}
	float getPeak(int channel, float minHz, float maxHz);
	void stepChannel(int channelIndex, Input& input);
	void stepChannelSample(int channelIndex, float sample);
};

struct AnalyzerTypes {
	enum FrequencyPlot {
		LOG_FP,
		LINEAR_FP
	};

	enum AmplitudePlot {
		DECIBELS_80_AP,
		DECIBELS_140_AP,
		PERCENTAGE_AP
	};
};

struct AnalyzerBase : BGModule, AnalyzerTypes {
	float _range = 0.0f;
	float _rangeMinHz = 0.0;
	float _rangeMaxHz = 0.0;
	FrequencyPlot _frequencyPlot = LOG_FP;
	AmplitudePlot _amplitudePlot = DECIBELS_80_AP;
	AnalyzerCore _core;

	AnalyzerBase(
		int nChannels,
		int np,
		int ni,
		int no,
		int nl = 0,
		SpectrumAnalyzer::Overlap overlap = SpectrumAnalyzer::OVERLAP_2
	)
	: _core(nChannels, overlap)
	{
		config(np, ni, no, nl);
	}

	void frequencyPlotToJson(json_t* root);
	void frequencyPlotFromJson(json_t* root);
	void frequencyRangeToJson(json_t* root);
	void frequencyRangeFromJson(json_t* root);
	void amplitudePlotToJson(json_t* root);
	void amplitudePlotFromJson(json_t* root);
};

struct AnalyzerBaseWidget : BGModuleWidget {
	void addFrequencyPlotContextMenu(Menu* menu);
	void addFrequencyRangeContextMenu(Menu* menu);
	void addAmplitudePlotContextMenu(Menu* menu, bool linearOption = true);
};

struct AnalyzerDisplay : TransparentWidget, AnalyzerTypes {
	struct BinsReader {
		BinsReader() {}
		virtual ~BinsReader() {}
		virtual float at(int i) = 0;
	};

	struct GenericBinsReader : BinsReader {
		float* _bins;
		GenericBinsReader(float* bins) : _bins(bins) {}
		float at(int i) override { return _bins[i]; }
	};

	const int _insetAround = 2;
	const int _insetLeft = _insetAround + 12;
	const int _insetRight = _insetAround + 2;
	const int _insetTop = _insetAround + 13;
	const int _insetBottom = _insetAround + 9;

	// const float _displayDB = 140.0;
	const float _positiveDisplayDB = 20.0;
	const float _totalLinearAmplitude = 2.0;

	const float baseXAxisLogFactor = 1 / 3.321; // magic number.

	const NVGcolor _axisColor = nvgRGBA(0xff, 0xff, 0xff, 0x70);
	const NVGcolor _textColor = nvgRGBA(0xff, 0xff, 0xff, 0xc0);
	static constexpr int channelColorsN = 8;
	const NVGcolor _channelColors[channelColorsN] = {
		nvgRGBA(0x00, 0xff, 0x00, 0xd0),
		nvgRGBA(0xff, 0x00, 0xff, 0xd0),
		nvgRGBA(0xff, 0x80, 0x00, 0xd0),
		nvgRGBA(0x00, 0x80, 0xff, 0xd0),

		nvgRGBA(0xff, 0x00, 0x00, 0xd0),
		nvgRGBA(0xff, 0xff, 0x00, 0xd0),
		nvgRGBA(0x00, 0xff, 0xff, 0xd0),
		nvgRGBA(0xff, 0x80, 0x80, 0xd0)
	};

	AnalyzerBase* _module;
	const Vec _size;
	const Vec _graphSize;
	bool _drawInset;
	std::shared_ptr<Font> _font;
	float _xAxisLogFactor = baseXAxisLogFactor;
	BinsReader** _channelBinsReaders = NULL;
	bool* _displayChannel = NULL;
	std::string* _channelLabels = NULL;
	Vec _freezeMouse;
	bool _freezeDraw = false;
	float* _freezeBufs = NULL;

	AnalyzerDisplay(
		AnalyzerBase* module,
		Vec size,
		bool drawInset
	)
	: _module(module)
	, _size(size)
	, _graphSize(_size.x - _insetLeft - _insetRight, _size.y - _insetTop - _insetBottom)
	, _drawInset(drawInset)
	, _font(APP->window->loadFont(asset::plugin(pluginInstance, "res/fonts/inconsolata.ttf")))
	{
		if (_module) {
			_channelBinsReaders = new BinsReader*[_module->_core._nChannels] {};
			_displayChannel = new bool[_module->_core._nChannels] {};
			_channelLabels = new std::string[_module->_core._nChannels];
			std::fill_n(_displayChannel, _module->_core._nChannels, true);
		}
	}
	~AnalyzerDisplay() {
		if (_module) {
			for (int i = 0; i < _module->_core._nChannels; ++i) {
				if (_channelBinsReaders) {
					delete _channelBinsReaders[i];
				}
			}
			delete[] _channelBinsReaders;
			delete[] _displayChannel;
			delete[] _channelLabels;
		}
	}

	void onButton(const event::Button& e) override;
	void onDragMove(const event::DragMove& e) override;
	void onDragEnd(const event::DragEnd& e) override;
	void setChannelBinsReader(int channel, BinsReader* br);
	void displayChannel(int channel, bool display);
	void channelLabel(int channel, std::string label);
	void draw(const DrawArgs& args) override;
	void drawBackground(const DrawArgs& args);
	virtual void drawHeader(const DrawArgs& args, float rangeMinHz, float rangeMaxHz);
	void drawYAxis(const DrawArgs& args, float strokeWidth, AmplitudePlot plot);
	void drawXAxis(const DrawArgs& args, float strokeWidth, FrequencyPlot plot, float rangeMinHz, float rangeMaxHz);
	void drawXAxisLine(const DrawArgs& args, float hz, float rangeMinHz, float rangeMaxHz);
	void drawGraph(const DrawArgs& args, BinsReader& bins, NVGcolor color, float strokeWidth, FrequencyPlot freqPlot, float rangeMinHz, float rangeMaxHz, AmplitudePlot ampPlot);
	void freezeValues(float rangeMinHz, float rangeMaxHz, int& binI, float& lowHz, float& highHz);
	void drawFreezeUnder(const DrawArgs& args, float lowHz, float highHz, float rangeMinHz, float rangeMaxHz, float strokeWidth);
	void drawFreezeOver(const DrawArgs& args, int binI, int binsN, float lowHz, float highHz, float strokeWidth);
	void drawText(const DrawArgs& args, const char* s, float x, float y, float rotation = 0.0, const NVGcolor* color = NULL, int fontSize = 10);
	int binValueToHeight(float value, AmplitudePlot plot);
	static float binValueToAmplitude(float value);
	static float binValueToDb(float value);
	static float dbToBinValue(float db);
};

} // namespace bogaudio
