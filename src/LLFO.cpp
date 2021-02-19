
#include "LLFO.hpp"

#define OUTPUT_SAMPLING "output_sampling"
#define PULSE_WIDTH "pulse_width"
#define SMOOTHING "smoothing"

void LLFO::reset() {
	for (int c = 0; c < maxChannels; ++c) {
		_resetTrigger[c].reset();
	}
}

void LLFO::sampleRateChange() {
	for (int c = 0; c < maxChannels; ++c) {
		_phasor[c].setSampleRate(APP->engine->getSampleRate());
	}
}

json_t* LLFO::toJson(json_t* root) {
	json_object_set_new(root, OUTPUT_SAMPLING, json_real(_sample));
	json_object_set_new(root, PULSE_WIDTH, json_real(_pulseWidth));
	json_object_set_new(root, SMOOTHING, json_real(_smooth));
	return root;
}

void LLFO::fromJson(json_t* root) {
	json_t* os = json_object_get(root, OUTPUT_SAMPLING);
	if (os) {
		_sample = json_real_value(os);
	}

	json_t* pw = json_object_get(root, PULSE_WIDTH);
	if (pw) {
		_pulseWidth = json_real_value(pw);
	}

	json_t* s = json_object_get(root, SMOOTHING);
	if (s) {
		_smooth = json_real_value(s);
	}
}

bool LLFO::active() {
	return outputs[OUT_OUTPUT].isConnected();
}

int LLFO::channels() {
	return inputs[PITCH_INPUT].getChannels();
}

void LLFO::addChannel(int c) {
	if (c > 0) {
		_phasor[c].syncPhase(_phasor[0]);
	}
}

void LLFO::modulate() {
	_slowMode = params[SLOW_PARAM].getValue() > 0.5f;

	_invert = false;
	Wave wave = (Wave)params[WAVE_PARAM].getValue();
	switch (wave) {
		case SINE_WAVE: {
			_oscillator = &_sine;
			_samplingEnabled = true;
			break;
		}
		case TRIANGLE_WAVE: {
			_oscillator = &_triangle;
			_samplingEnabled = true;
			break;
		}
		case RAMP_UP_WAVE: {
			_oscillator = &_ramp;
			_samplingEnabled = true;
			break;
		}
		case RAMP_DOWN_WAVE: {
			_oscillator = &_ramp;
			_invert = true;
			_samplingEnabled = true;
			break;
		}
		case SQUARE_WAVE: {
			_oscillator = &_square;
			_square.setPulseWidth(0.5f);
			_samplingEnabled = false;
			break;
		}
		case PULSE_WAVE: {
			_oscillator = &_square;
			_samplingEnabled = false;

			float pw = _pulseWidth;
			pw *= 1.0f - 2.0f * SquareOscillator::minPulseWidth;
			pw *= 0.5f;
			pw += 0.5f;
			_square.setPulseWidth(pw);
			break;
		}
		case STEPPED_WAVE: {
			_oscillator = &_stepped;
			_samplingEnabled = false;
			break;
		}
	}

	_offset = params[OFFSET_PARAM].getValue() * 5.0f;
	_scale = params[SCALE_PARAM].getValue();
}

void LLFO::modulateChannel(int c) {
	setFrequency(params[FREQUENCY_PARAM], inputs[PITCH_INPUT], _phasor[c], c);

	if (_samplingEnabled) {
		float maxSampleSteps = (_phasor[c]._sampleRate / _phasor[c]._frequency) / 4.0f;
		_sampleSteps[c] = clamp((int)(_sample * maxSampleSteps), 1, (int)maxSampleSteps);
	}
	else {
		_sampleSteps[c] = 1;
	}

	_smoother[c].setParams(APP->engine->getSampleRate(), _phasor[c]._frequency, _smooth);
}

void LLFO::processAlways(const ProcessArgs& args) {
	Wave wave = (Wave)params[WAVE_PARAM].getValue();
	lights[SINE_LIGHT].value = wave == SINE_WAVE;
	lights[TRIANGLE_LIGHT].value = wave == TRIANGLE_WAVE;
	lights[RAMP_UP_LIGHT].value = wave == RAMP_UP_WAVE;
	lights[RAMP_DOWN_LIGHT].value = wave == RAMP_DOWN_WAVE;
	lights[SQUARE_LIGHT].value = wave == SQUARE_WAVE;
	lights[PULSE_LIGHT].value = wave == PULSE_WAVE;
	lights[STEPPED_LIGHT].value = wave == STEPPED_WAVE;
}

void LLFO::processChannel(const ProcessArgs& args, int c) {
	if (_resetTrigger[c].next(inputs[RESET_INPUT].getPolyVoltage(c))) {
		_phasor[c].resetPhase();
	}
	_phasor[c].advancePhase();

	bool useSample = false;
	if (_sampleSteps[c] > 1) {
		++_sampleStep[c];
		if (_sampleStep[c] >= _sampleSteps[c]) {
			_sampleStep[c] = 0;
		}
		else {
			useSample = true;
		}
	}
	if (!useSample) {
		_currentSample[c] = _oscillator->nextFromPhasor(_phasor[c]) * amplitude * _scale;
		if (_invert) {
			_currentSample[c] = -_currentSample[c];
		}
		_currentSample[c] += _offset;
	}

	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(_smoother[c].next(_currentSample[c]), c);
}

struct SampleQuantity : Quantity {
	LLFO* _module;

	SampleQuantity(LLFO* m) : _module(m) {}

	void setValue(float value) override {
		value = clamp(value, getMinValue(), getMaxValue());
		if (_module) {
			_module->_sample = value;
		}
	}

	float getValue() override {
		if (_module) {
			return _module->_sample;
		}
		return getDefaultValue();
	}

	float getMinValue() override { return 0.0f; }
	float getMaxValue() override { return 1.0f; }
	float getDefaultValue() override { return LLFO::defaultSample; }
	float getDisplayValue() override { return roundf(100.0f * getValue()); }
	void setDisplayValue(float displayValue) override { setValue(displayValue / 100.0f); }
	std::string getLabel() override { return "Output sampling"; }
	std::string getUnit() override { return "%"; }
};

struct PWQuantity : Quantity {
	LLFO* _module;

	PWQuantity(LLFO* m) : _module(m) {}

	void setValue(float value) override {
		value = clamp(value, getMinValue(), getMaxValue());
		if (_module) {
			_module->_pulseWidth = value;
		}
	}

	float getValue() override {
		if (_module) {
			return _module->_pulseWidth;
		}
		return getDefaultValue();
	}

	float getMinValue() override { return -1.0f; }
	float getMaxValue() override { return 1.0f; }
	float getDefaultValue() override { return LLFO::defaultPulseWidth; }
	float getDisplayValue() override { return roundf(50.0f + 100.0f * 0.5f * getValue() * (1.0f - 2.0f*SquareOscillator::minPulseWidth)); }
	void setDisplayValue(float displayValue) override { setValue((displayValue - 50.0f) / (100.0f * 0.5f * (1.0f - 2.0f*SquareOscillator::minPulseWidth))); }
	std::string getLabel() override { return "Pulse width"; }
	std::string getUnit() override { return "%"; }
};

struct SmoothQuantity : Quantity {
	LLFO* _module;

	SmoothQuantity(LLFO* m) : _module(m) {}

	void setValue(float value) override {
		value = clamp(value, getMinValue(), getMaxValue());
		if (_module) {
			_module->_smooth = value;
		}
	}

	float getValue() override {
		if (_module) {
			return _module->_smooth;
		}
		return getDefaultValue();
	}

	float getMinValue() override { return 0.0f; }
	float getMaxValue() override { return 1.0f; }
	float getDefaultValue() override { return LLFO::defaultSmooth; }
	float getDisplayValue() override { return roundf(100.0f * getValue()); }
	void setDisplayValue(float displayValue) override { setValue(displayValue / 100.0f); }
	std::string getLabel() override { return "Smoothing"; }
	std::string getUnit() override { return "%"; }
};

template<class Q>
struct LLFOSlider : ui::Slider {
	LLFOSlider(LLFO* module) {
		quantity = new Q(module);
		box.size.x = 200.0f;
	}
	virtual ~LLFOSlider() {
		delete quantity;
	}
};

template<class Q>
struct LLFOSliderMenuItem : MenuItem {
	LLFO* _module;

	LLFOSliderMenuItem(LLFO* m, const char* label) : _module(m) {
		this->text = label;
		this->rightText = "▸";
	}

	Menu* createChildMenu() override {
		Menu* menu = new Menu;
		menu->addChild(new LLFOSlider<Q>(_module));
		return menu;
	}
};

struct LLFOWidget : BGModuleWidget {
	static constexpr int hp = 3;

	LLFOWidget(LLFO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "LLFO");
		createScrews();

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(9.5, 27.0);
		auto slowParamPosition = Vec(31.0, 71.0);
		auto waveParamPosition = Vec(29.5, 126.0);
		auto offsetParamPosition = Vec(14.5, 158.5);
		auto scaleParamPosition = Vec(14.5, 199.5);

		auto pitchInputPosition = Vec(10.5, 231.0);
		auto resetInputPosition = Vec(10.5, 266.0);

		auto outOutputPosition = Vec(10.5, 304.0);

		auto sineLightPosition = Vec(2.0, 89.0);
		auto rampUpLightPosition = Vec(2.0, 102.0);
		auto squareLightPosition = Vec(2.0, 115.0);
		auto steppedLightPosition = Vec(2.0, 128.0);
		auto triangleLightPosition = Vec(24.0, 89.0);
		auto rampDownLightPosition = Vec(24.0, 102.0);
		auto pulseLightPosition = Vec(24.0, 115.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(frequencyParamPosition, module, LLFO::FREQUENCY_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(slowParamPosition, module, LLFO::SLOW_PARAM));
		addParam(createParam<StatefulButton9>(waveParamPosition, module, LLFO::WAVE_PARAM));
		addParam(createParam<Knob16>(offsetParamPosition, module, LLFO::OFFSET_PARAM));
		addParam(createParam<Knob16>(scaleParamPosition, module, LLFO::SCALE_PARAM));

		addInput(createInput<Port24>(pitchInputPosition, module, LLFO::PITCH_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, LLFO::RESET_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, LLFO::OUT_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(sineLightPosition, module, LLFO::SINE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(rampUpLightPosition, module, LLFO::RAMP_UP_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(squareLightPosition, module, LLFO::SQUARE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(steppedLightPosition, module, LLFO::STEPPED_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(triangleLightPosition, module, LLFO::TRIANGLE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(rampDownLightPosition, module, LLFO::RAMP_DOWN_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(pulseLightPosition, module, LLFO::PULSE_LIGHT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<LLFO*>(module);
		assert(m);

		menu->addChild(new LLFOSliderMenuItem<SampleQuantity>(m, "Output sampling"));
		menu->addChild(new LLFOSliderMenuItem<PWQuantity>(m, "Pulse width"));
		menu->addChild(new LLFOSliderMenuItem<SmoothQuantity>(m, "Smoothing"));
	}
};

Model* modelLLFO = bogaudio::createModel<LLFO, LLFOWidget>("Bogaudio-LLFO", "LLFO", "Low frequency oscillator", "LFO", "Random", "Polyphonic");
