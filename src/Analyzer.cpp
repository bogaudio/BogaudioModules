
#include <algorithm>

#include "dsp/dsp.hpp"
#include "BogaudioModules.hpp"

struct Analyzer : Module {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		SIGNAL_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		SIGNAL_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	bogaudio::dsp::SpectrumAnalyzer* _analyzer = NULL;
	const int _displayBins = 256;
	const int _avgN = 10;
	float* _bins = NULL;
	float* _frames = NULL;
	int _currentFrame = 0;

	Analyzer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		reset();
	}
	virtual ~Analyzer() {
		if (_analyzer) {
			delete _analyzer;
			delete[] _bins;
			delete[] _frames;
		}
	}

	virtual void reset() override;
	virtual void step() override;
};

void Analyzer::reset() {
}

void Analyzer::step() {
	if (!inputs[SIGNAL_INPUT].active) {
		if (_analyzer) {
			delete _analyzer;
			_analyzer = NULL;
			delete[] _bins;
			_bins = NULL;
			delete[] _frames;
			_frames = NULL;
		}
		outputs[SIGNAL_OUTPUT].value = 0.0;
		return;
	}

	if (!_analyzer) {
		_analyzer = new bogaudio::dsp::SpectrumAnalyzer(
			bogaudio::dsp::SpectrumAnalyzer::SIZE_1024,
			bogaudio::dsp::SpectrumAnalyzer::OVERLAP_2,
			bogaudio::dsp::SpectrumAnalyzer::WINDOW_HAMMING,
			// bogaudio::dsp::SpectrumAnalyzer::WINDOW_NONE,
			engineGetSampleRate()
		);
		_bins = new float[_displayBins] { 0.0 };
		_frames = new float[_avgN * _displayBins] { 0.0 };
		_currentFrame = 0;
	}

	if (_analyzer->step(inputs[SIGNAL_INPUT].value)) {
		float* frame = _frames + _currentFrame*_displayBins;
		_analyzer->getMagnitudes(frame, _displayBins);

		for (int bin = 0; bin < _displayBins; ++bin) {
			_bins[bin] = 0.0;
			for (int i = 0; i < _avgN; ++i) {
				_bins[bin] += _frames[i * _displayBins + bin];
			}
			_bins[bin] /= (float)_avgN;
			// FIXME: still unclear if should or should not take the root here: _bins[bin] = sqrtf(_bins[bin]);
		}
		_currentFrame = (_currentFrame + 1) % _avgN;
	}

	outputs[SIGNAL_OUTPUT].value = inputs[SIGNAL_INPUT].value;
}


struct AnalyzerDisplay : TransparentWidget {
	Analyzer* _module;
	const Vec _size;

	const int _levelWidth = 1;
	const int _levelSpace = 0;
	const Vec _levelsInset = Vec(30, 5);
	const int _levelHeight;
	const int _levelCount;

	AnalyzerDisplay(
		Analyzer* module,
		Vec size
	)
	: _module(module)
	, _size(size)
	, _levelHeight(_size.y - 2*_levelsInset.y)
	, _levelCount((size.x - _levelsInset.x + 1) / (_levelWidth + _levelSpace))
	{
	}

	void draw(NVGcontext *vg) override {
		const float refDB0 = 20.0; // arbitrary; makes a 10.0-amplitude sine wave reach to about 0db on the output.
		const float displayDB = 120.0;

		nvgBeginPath(vg);
		nvgRect(vg, 0, 0, _size.x, _size.y);
		nvgFillColor(vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
		nvgFill(vg);
		nvgStrokeColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
		nvgStroke(vg);

		nvgBeginPath(vg);
		int lineY = _levelsInset.y + _levelHeight - _levelHeight*(displayDB - 20.0)/displayDB;
		nvgMoveTo(vg, _levelsInset.x, lineY);
		nvgLineTo(vg, _size.x, lineY);
		nvgStrokeColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
		nvgStroke(vg);
		nvgBeginPath(vg);
		lineY = _levelsInset.y + _levelHeight - _levelHeight*(displayDB - 70.0)/displayDB;
		nvgMoveTo(vg, _levelsInset.x, lineY);
		nvgLineTo(vg, _size.x, lineY);
		nvgStrokeColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
		nvgStroke(vg);

		const bool lineGraph = true; // else bar graph. // FIXME: control from module.
		if (_module->_bins) {
			int n = std::min(_module->_displayBins, _levelCount) - 2;
			nvgFillColor(vg, nvgRGBA(0x00, 0xff, 0x00, 0xff));
			nvgStrokeColor(vg, nvgRGBA(0x00, 0xff, 0x00, 0xff));

			if (lineGraph) {
				nvgBeginPath(vg);
			}
			for (int i = 0; i < n; ++i) {
				float value = _module->_bins[i];
				value /= refDB0;
				value = log10(value);
				value = std::max(-5.0f, value);
				value = std::min(1.0f, value);
				value *= 20.0;
				value = (value + displayDB - 20.0) / displayDB;

				int height = round(_levelHeight * value);
				if (lineGraph) {
					if (i == 0) {
						nvgMoveTo(vg, _levelsInset.x, _levelsInset.y + (_levelHeight - height));
					}
					else {
						nvgLineTo(vg, _levelsInset.x + i, _levelsInset.y + (_levelHeight - height));
					}
				}
				else {
					nvgBeginPath(vg);
					nvgRect(
						vg,
						_levelsInset.x + i*(_levelWidth + _levelSpace),
						_levelsInset.y + (_levelHeight - height),
						_levelWidth,
						height
					);
					nvgFill(vg);
				}
			}
			if (lineGraph) {
				nvgStroke(vg);
			}
		}
	}
};


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
		auto inset = Vec(10, 40);
		auto size = Vec(box.size.x - 2*inset.x, 100 + inset.y);
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
	auto signalInputPosition = Vec(15.0, 324.0);

	auto signalOutputPosition = Vec(261.0, 324.0);
	// end generated by svg_widgets.rb

	addInput(createInput<PJ301MPort>(signalInputPosition, module, Analyzer::SIGNAL_INPUT));

	addOutput(createOutput<PJ301MPort>(signalOutputPosition, module, Analyzer::SIGNAL_OUTPUT));
}
