
#include <algorithm>

#include "dsp/dsp.hpp"
#include "BogaudioModules.hpp"

struct ChannelAnalyzer : bogaudio::dsp::SpectrumAnalyzer {
	const int _averageN;
	const int _binsN;
	float* _bins;
	float* _frames;
	int _currentFrame;

	ChannelAnalyzer(
		bogaudio::dsp::SpectrumAnalyzer::Size size,
		bogaudio::dsp::SpectrumAnalyzer::Overlap overlap,
		bogaudio::dsp::SpectrumAnalyzer::WindowType windowType,
		float sampleRate,
		int averageN
	)
	: bogaudio::dsp::SpectrumAnalyzer(size, overlap, windowType, sampleRate)
	, _averageN(averageN)
	, _binsN(size / 2)
	, _currentFrame(0)
	{
		_bins = new float[size] { 0.0 };
		_frames = new float[_averageN * _binsN] { 0.0 };
	}
	virtual ~ChannelAnalyzer() {
		delete[] _bins;
		delete[] _frames;
	}

	virtual bool step(float sample) override;
};

bool ChannelAnalyzer::step(float sample) {
	if (bogaudio::dsp::SpectrumAnalyzer::step(sample)) {
		float* frame = _frames + _currentFrame*_binsN;
		getMagnitudes(frame, _binsN);

		for (int bin = 0; bin < _binsN; ++bin) {
			_bins[bin] = 0.0;
			for (int i = 0; i < _averageN; ++i) {
				_bins[bin] += _frames[i*_binsN + bin];
			}
			_bins[bin] /= (float)_averageN;
			// FIXME: still unclear if should or should not take the root here: _bins[bin] = sqrtf(_bins[bin]);
		}
		_currentFrame = (_currentFrame + 1) % _averageN;
		return true;
	}
	return false;
}


struct Analyzer : Module {
	enum ParamsIds {
		RANGE_PARAM,
		// SMOOTH_PARAM,
		// TYPE_PARAM,
		POWER_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		SIGNALA_INPUT,
		SIGNALB_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		SIGNALA_OUTPUT,
		SIGNALB_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	enum DisplayType {
		DISPLAYTYPE_LINE,
		DISPLAYTYPE_BAR
	};

	const bogaudio::dsp::SpectrumAnalyzer::Size _size = bogaudio::dsp::SpectrumAnalyzer::SIZE_1024;
	DisplayType _displayType;
	int _averageN;
	ChannelAnalyzer* _channelA = NULL;
	ChannelAnalyzer* _channelB = NULL;
	float _range = 0.0;

	Analyzer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		reset();
	}
	virtual ~Analyzer() {
		reset();
	}

	virtual void reset() override;
	virtual void step() override;
	void stepChannel(ChannelAnalyzer*& channelPointer, bool running, Input& input, Output& output);
};

void Analyzer::reset() {
	_displayType = DISPLAYTYPE_LINE;
	_averageN = 10;
	if (_channelA) {
		delete _channelA;
		_channelA = NULL;
	}
	if (_channelB) {
		delete _channelB;
		_channelB = NULL;
	}
}

void Analyzer::step() {
	_range = clampf(params[RANGE_PARAM].value, 0.1, 1.0);
	bool running = params[POWER_PARAM].value == 1.0;
	stepChannel(_channelA, running, inputs[SIGNALA_INPUT], outputs[SIGNALA_OUTPUT]);
	stepChannel(_channelB, running, inputs[SIGNALB_INPUT], outputs[SIGNALB_OUTPUT]);
}

void Analyzer::stepChannel(ChannelAnalyzer*& channelPointer, bool running, Input& input, Output& output) {
	if (running && input.active) {
		if (!channelPointer) {
			channelPointer = new ChannelAnalyzer(
				_size,
				bogaudio::dsp::SpectrumAnalyzer::OVERLAP_2,
				bogaudio::dsp::SpectrumAnalyzer::WINDOW_HAMMING,
				engineGetSampleRate(),
				_averageN
			);
		}

		channelPointer->step(input.value);
		output.value = input.value;
	}
	else {
		if (channelPointer) {
			delete channelPointer;
			channelPointer = NULL;
		}

		output.value = 0.0;
	}
}


struct AnalyzerDisplay : TransparentWidget {
	const int _insetLeft = 27;
	const int _insetRight = 4;
	const int _insetTop = 2;
	const int _insetBottom = 18;

	const float _refDB0 = 20.0; // arbitrary; makes a 10.0-amplitude sine wave reach to about 0db on the output.
	const float _displayDB = 120.0;

	const float xAxisLogFactor = 1 / 3.321; // magic number.

	const NVGcolor _axisColor = nvgRGBA(0xff, 0xff, 0xff, 0x70);
	const NVGcolor _channelAColor = nvgRGBA(0x00, 0xff, 0x00, 0xd0);
	const NVGcolor _channelBColor = nvgRGBA(0xff, 0x00, 0x00, 0xd0);

	Analyzer* _module;
	const Vec _size;
	const Vec _graphSize;

	AnalyzerDisplay(
		Analyzer* module,
		Vec size
	)
	: _module(module)
	, _size(size)
	, _graphSize(_size.x - _insetLeft - _insetRight, _size.y - _insetTop - _insetBottom)
	{
	}

	void draw(NVGcontext* vg) override;
	void drawBlank(NVGcontext* vg);
	void drawYAxis(NVGcontext* vg);
	void drawXAxis(NVGcontext* vg);
	void drawXAxisLine(NVGcontext* vg, float hz, float maxHz);
	void drawLine(NVGcontext* vg, float* bins, int binsN, NVGcolor color);
	void drawBars(NVGcontext* vg, float* bins, int binsN, NVGcolor color);

	int binValueToHeight(float value);
};

void AnalyzerDisplay::draw(NVGcontext* vg) {
	drawBlank(vg);
	drawYAxis(vg);
	drawXAxis(vg);

	switch(_module->_displayType) {
		case Analyzer::DISPLAYTYPE_LINE: {
			if (_module->_channelA) {
				drawLine(vg, _module->_channelA->_bins, _module->_channelA->_binsN, _channelAColor);
			}
			if (_module->_channelB) {
				drawLine(vg, _module->_channelB->_bins, _module->_channelB->_binsN, _channelBColor);
			}
			break;
		}
		case Analyzer::DISPLAYTYPE_BAR: {
			if (_module->_channelA) {
				drawBars(vg, _module->_channelA->_bins, _module->_channelA->_binsN, _channelAColor);
			}
			if (_module->_channelB) {
				drawBars(vg, _module->_channelB->_bins, _module->_channelB->_binsN, _channelBColor);
			}
			break;
		}
	}
}

void AnalyzerDisplay::drawBlank(NVGcontext* vg) {
	nvgSave(vg);
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, _size.x, _size.y);
	nvgFillColor(vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGBA(0xc0, 0xc0, 0xc0, 0xff));
	nvgStroke(vg);
	nvgRestore(vg);
}

void AnalyzerDisplay::drawYAxis(NVGcontext* vg) {
	nvgSave(vg);
	nvgStrokeColor(vg, _axisColor);
	const int lineX = _insetLeft - 2;

	nvgBeginPath(vg);
	int lineY = _insetTop + (_graphSize.y - _graphSize.y*(_displayDB - 20.0)/_displayDB);
	nvgMoveTo(vg, lineX, lineY);
	nvgLineTo(vg, _size.x - _insetRight, lineY);
	nvgStroke(vg);

	nvgBeginPath(vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(_displayDB - 70.0)/_displayDB);
	nvgMoveTo(vg, lineX, lineY);
	nvgLineTo(vg, _size.x - _insetRight, lineY);
	nvgStroke(vg);

	nvgBeginPath(vg);
	lineY = _insetTop + _graphSize.y + 1;
	nvgMoveTo(vg, lineX, lineY);
	nvgLineTo(vg, _size.x - _insetRight, lineY);
	nvgStroke(vg);

	nvgBeginPath(vg);
	nvgMoveTo(vg, lineX, _insetTop);
	nvgLineTo(vg, lineX, lineY);
	nvgStroke(vg);
	nvgRestore(vg);
}

void AnalyzerDisplay::drawXAxis(NVGcontext* vg) {
	nvgSave(vg);
	nvgStrokeColor(vg, _axisColor);
	nvgStrokeWidth(vg, 0.8);

	const float maxHz = _module->_range * (engineGetSampleRate() / 2.0);
	float hz = 100.0;
	while (hz < maxHz && hz < 1001.0) {
		drawXAxisLine(vg, hz, maxHz);
		hz += 100.0;
	}
	hz = 2000.0;
	while (hz < maxHz && hz < 10001.0) {
		drawXAxisLine(vg, hz, maxHz);
		hz += 1000.0;
	}
	hz = 20000.0;
	while (hz < maxHz && hz < 100001.0) {
		drawXAxisLine(vg, hz, maxHz);
		hz += 10000.0;
	}

	nvgRestore(vg);
}

void AnalyzerDisplay::drawXAxisLine(NVGcontext* vg, float hz, float maxHz) {
	float x = hz / maxHz;
	x = powf(x, xAxisLogFactor);
	if (x < 1.0) {
		x *= _graphSize.x;
		nvgBeginPath(vg);
		nvgMoveTo(vg, _insetLeft + x, _insetTop);
		nvgLineTo(vg, _insetLeft + x, _insetTop + _graphSize.y);
		nvgStroke(vg);
	}
}

void AnalyzerDisplay::drawLine(NVGcontext* vg, float* bins, int binsN, NVGcolor color) {
  const int pointsN = roundf(_module->_range*(_module->_size/2));
	nvgSave(vg);
	nvgStrokeColor(vg, color);
	nvgBeginPath(vg);
	for (int i = 0; i < pointsN; ++i) {
		int height = binValueToHeight(bins[i]);
		if (i == 0) {
			nvgMoveTo(vg, _insetLeft, _insetTop + (_graphSize.y - height));
		}
		else {
			float x = _graphSize.x * powf(i / (float)pointsN, xAxisLogFactor);
			nvgLineTo(vg, _insetLeft + x, _insetTop + (_graphSize.y - height));
		}
	}
	nvgStroke(vg);
	nvgRestore(vg);
}

void AnalyzerDisplay::drawBars(NVGcontext* vg, float* bins, int binsN, NVGcolor color) {
	// nvgSave(vg);
	// int n = std::min(_module->_displayBins, 100) - 2; // FIXME
	// nvgFillColor(vg, color);
  //
	// for (int i = 0; i < n; ++i) {
	// 	float value = bins[i];
	// 	value /= refDB0;
	// 	value = log10(value);
	// 	value = std::max(-5.0f, value);
	// 	value = std::min(1.0f, value);
	// 	value *= 20.0;
	// 	value = (value + displayDB - 20.0) / displayDB;
  //
	// 	int height = round(_levelHeight * value);
	// 	nvgBeginPath(vg);
	// 	nvgRect(
	// 		vg,
	// 		_levelsInset.x + i*(_levelWidth + _levelSpace),
	// 		_levelsInset.y + (_levelHeight - height),
	// 		_levelWidth,
	// 		height
	// 	);
	// 	nvgFill(vg);
	//  nvgRestore(vg);
	// }
}

int AnalyzerDisplay::binValueToHeight(float value) {
	value /= _refDB0;
	value = log10(value);
	value = std::max(-5.0f, value);
	value = std::min(1.0f, value);
	value *= 20.0;
	value = (value + _displayDB - 20.0) / _displayDB;
	return round(_graphSize.y * value);
}


AnalyzerWidget::AnalyzerWidget() {
	auto module = new Analyzer();
	setModule(module);
	box.size = Vec(RACK_GRID_WIDTH * 20, RACK_GRID_HEIGHT);

	{
		auto panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Analyzer.svg")));
		addChild(panel);
	}

	{
		auto inset = Vec(10, 30);
		auto size = Vec(box.size.x - 2*inset.x, 230);
		auto display = new AnalyzerDisplay(module, size);
		display->box.pos = inset;
		display->box.size = size;
		addChild(display);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 365)));

	// generated by svg_widgets.rb
	auto rangeParamPosition = Vec(100.08, 303.58);
	// auto smoothParamPosition = Vec(164.08, 303.58);
	// auto typeParamPosition = Vec(231.9, 305.4);
	auto powerParamPosition = Vec(272.9, 305.4);

	auto signalaInputPosition = Vec(27.5, 273.0);
	auto signalbInputPosition = Vec(27.5, 323.0);

	auto signalaOutputPosition = Vec(56.5, 273.0);
	auto signalbOutputPosition = Vec(56.5, 323.0);
	// end generated by svg_widgets.rb

	addParam(createParam<Knob38>(rangeParamPosition, module, Analyzer::RANGE_PARAM, 0.1, 1.0, 0.5));
	// addParam(createParam<Knob38>(smoothParamPosition, module, Analyzer::SMOOTH_PARAM, 0.1, 1.0, 0.3));
	// addParam(createParam<CKSS>(typeParamPosition, module, Analyzer::TYPE_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<CKSS>(powerParamPosition, module, Analyzer::POWER_PARAM, 0.0, 1.0, 1.0));

	addInput(createInput<PJ301MPort>(signalaInputPosition, module, Analyzer::SIGNALA_INPUT));
	addInput(createInput<PJ301MPort>(signalbInputPosition, module, Analyzer::SIGNALB_INPUT));

	addOutput(createOutput<PJ301MPort>(signalaOutputPosition, module, Analyzer::SIGNALA_OUTPUT));
	addOutput(createOutput<PJ301MPort>(signalbOutputPosition, module, Analyzer::SIGNALB_OUTPUT));
}
