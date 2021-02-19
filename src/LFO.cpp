
#include "LFO.hpp"

#define OFFSET_CV_TO_SMOOTHING "offset_cv_to_smoothing"

void LFO::Engine::reset() {
	resetTrigger.reset();
	sampleStep = phasor._sampleRate;
}

void LFO::Engine::sampleRateChange() {
	phasor.setSampleRate(APP->engine->getSampleRate());
	sampleStep = phasor._sampleRate;
}

void LFO::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void LFO::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

json_t* LFO::toJson(json_t* root) {
	json_object_set_new(root, OFFSET_CV_TO_SMOOTHING, json_boolean(_useOffsetCvForSmooth));
	return root;
}

void LFO::fromJson(json_t* root) {
	json_t* ocv = json_object_get(root, OFFSET_CV_TO_SMOOTHING);
	if (ocv) {
		_useOffsetCvForSmooth = json_boolean_value(ocv);
	}
}

bool LFO::active() {
	return (
		outputs[SINE_OUTPUT].isConnected() ||
		outputs[TRIANGLE_OUTPUT].isConnected() ||
		outputs[RAMP_UP_OUTPUT].isConnected() ||
		outputs[RAMP_DOWN_OUTPUT].isConnected() ||
		outputs[SQUARE_OUTPUT].isConnected() ||
		outputs[STEPPED_OUTPUT].isConnected()
	);
}

int LFO::channels() {
	return inputs[PITCH_INPUT].getChannels();
}

void LFO::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->sampleRateChange();
	if (c > 0) {
		_engines[c]->phasor.syncPhase(_engines[0]->phasor);
	}
}

void LFO::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void LFO::modulate() {
	_slowMode = params[SLOW_PARAM].getValue() > 0.5f;
}

void LFO::modulateChannel(int c) {
	Engine& e = *_engines[c];

	setFrequency(params[FREQUENCY_PARAM], inputs[PITCH_INPUT], e.phasor, c);

	float pw = params[PW_PARAM].getValue();
	if (inputs[PW_INPUT].isConnected()) {
		pw *= clamp(inputs[PW_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	pw *= 1.0f - 2.0f * e.square.minPulseWidth;
	pw *= 0.5f;
	pw += 0.5f;
	e.square.setPulseWidth(pw);

	float sample = params[SAMPLE_PARAM].getValue();
	if (inputs[SAMPLE_INPUT].isConnected()) {
		sample *= clamp(inputs[SAMPLE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	float maxSampleSteps = (e.phasor._sampleRate / e.phasor._frequency) / 4.0f;
	e.sampleSteps = clamp((int)(sample * maxSampleSteps), 1, (int)maxSampleSteps);

	float smooth = params[SMOOTH_PARAM].getValue();
	if (_useOffsetCvForSmooth && inputs[OFFSET_INPUT].isConnected()) {
		smooth *= clamp(inputs[OFFSET_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	float sr = APP->engine->getSampleRate();
	e.sineSmoother.setParams(sr, e.phasor._frequency, smooth);
	e.triangleSmoother.setParams(sr, e.phasor._frequency, smooth);
	e.rampUpSmoother.setParams(sr, e.phasor._frequency, smooth);
	e.rampDownSmoother.setParams(sr, e.phasor._frequency, smooth);
	e.squareSmoother.setParams(sr, e.phasor._frequency, smooth);
	e.steppedSmoother.setParams(sr, e.phasor._frequency, smooth);

	e.offset = params[OFFSET_PARAM].getValue();
	if (!_useOffsetCvForSmooth && inputs[OFFSET_INPUT].isConnected()) {
		e.offset *= clamp(inputs[OFFSET_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	e.offset *= 5.0f;

	e.scale = params[SCALE_PARAM].getValue();
	if (inputs[SCALE_INPUT].isConnected()) {
		e.scale *= clamp(inputs[SCALE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
}

void LFO::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	if (e.resetTrigger.next(inputs[RESET_INPUT].getPolyVoltage(c))) {
		e.phasor.resetPhase();
	}

	e.phasor.advancePhase();
	bool useSample = false;
	if (e.sampleSteps > 1) {
		++e.sampleStep;
		if (e.sampleStep >= e.sampleSteps) {
			e.sampleStep = 0;
		}
		else {
			useSample = true;
		}
	}
	updateOutput(c, e.sine, useSample, false, outputs[SINE_OUTPUT], e.sineSample, e.sineActive, e.sineSmoother);
	updateOutput(c, e.triangle, useSample, false, outputs[TRIANGLE_OUTPUT], e.triangleSample, e.triangleActive, e.triangleSmoother);
	updateOutput(c, e.ramp, useSample, false, outputs[RAMP_UP_OUTPUT], e.rampUpSample, e.rampUpActive, e.rampUpSmoother);
	updateOutput(c, e.ramp, useSample, true, outputs[RAMP_DOWN_OUTPUT], e.rampDownSample, e.rampDownActive, e.rampDownSmoother);
	updateOutput(c, e.square, false, false, outputs[SQUARE_OUTPUT], e.squareSample, e.squareActive, e.squareSmoother);
	updateOutput(c, e.stepped, false, false, outputs[STEPPED_OUTPUT], e.steppedSample, e.steppedActive, e.steppedSmoother);
}

void LFO::updateOutput(int c, Phasor& wave, bool useSample, bool invert, Output& output, float& sample, bool& active, Smoother& smoother) {
	if (output.isConnected()) {
		output.setChannels(_channels);
		if (!useSample || !active) {
			sample = wave.nextFromPhasor(_engines[c]->phasor) * amplitude * _engines[c]->scale;
			if (invert) {
				sample = -sample;
			}
			sample += _engines[c]->offset;
		}
		output.setVoltage(smoother.next(sample), c);
		active = true;
	}
	else {
		active = false;
	}
}

struct LFOWidget : BGModuleWidget {
	static constexpr int hp = 10;

	LFOWidget(LFO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "LFO");
		createScrews();

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(41.0, 45.0);
		auto slowParamPosition = Vec(12.5, 96.5);
		auto sampleParamPosition = Vec(37.0, 150.0);
		auto pwParamPosition = Vec(102.0, 150.0);
		auto smoothParamPosition = Vec(27.0, 196.0);
		auto offsetParamPosition = Vec(73.0, 196.0);
		auto scaleParamPosition = Vec(119.0, 196.0);

		auto sampleInputPosition = Vec(15.0, 230.0);
		auto pwInputPosition = Vec(47.0, 230.0);
		auto offsetInputPosition = Vec(15.0, 274.0);
		auto scaleInputPosition = Vec(47.0, 274.0);
		auto pitchInputPosition = Vec(15.0, 318.0);
		auto resetInputPosition = Vec(47.0, 318.0);

		auto rampDownOutputPosition = Vec(79.0, 230.0);
		auto steppedOutputPosition = Vec(111.0, 230.0);
		auto rampUpOutputPosition = Vec(79.0, 274.0);
		auto squareOutputPosition = Vec(111.0, 274.0);
		auto triangleOutputPosition = Vec(79.0, 318.0);
		auto sineOutputPosition = Vec(111.0, 318.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob68>(frequencyParamPosition, module, LFO::FREQUENCY_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(slowParamPosition, module, LFO::SLOW_PARAM));
		addParam(createParam<Knob26>(sampleParamPosition, module, LFO::SAMPLE_PARAM));
		addParam(createParam<Knob26>(pwParamPosition, module, LFO::PW_PARAM));
		addParam(createParam<Knob16>(smoothParamPosition, module, LFO::SMOOTH_PARAM));
		addParam(createParam<Knob16>(offsetParamPosition, module, LFO::OFFSET_PARAM));
		addParam(createParam<Knob16>(scaleParamPosition, module, LFO::SCALE_PARAM));

		addInput(createInput<Port24>(sampleInputPosition, module, LFO::SAMPLE_INPUT));
		addInput(createInput<Port24>(pwInputPosition, module, LFO::PW_INPUT));
		addInput(createInput<Port24>(offsetInputPosition, module, LFO::OFFSET_INPUT));
		addInput(createInput<Port24>(scaleInputPosition, module, LFO::SCALE_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, LFO::PITCH_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, LFO::RESET_INPUT));

		addOutput(createOutput<Port24>(rampUpOutputPosition, module, LFO::RAMP_UP_OUTPUT));
		addOutput(createOutput<Port24>(steppedOutputPosition, module, LFO::STEPPED_OUTPUT));
		addOutput(createOutput<Port24>(rampDownOutputPosition, module, LFO::RAMP_DOWN_OUTPUT));
		addOutput(createOutput<Port24>(squareOutputPosition, module, LFO::SQUARE_OUTPUT));
		addOutput(createOutput<Port24>(triangleOutputPosition, module, LFO::TRIANGLE_OUTPUT));
		addOutput(createOutput<Port24>(sineOutputPosition, module, LFO::SINE_OUTPUT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<LFO*>(module);
		assert(m);

		OptionsMenuItem* uo = new OptionsMenuItem("OFF/SM input routing");
		uo->addItem(OptionMenuItem("To offset (OFF)", [m]() { return !m->_useOffsetCvForSmooth; }, [m]() { m->_useOffsetCvForSmooth = false; }));
		uo->addItem(OptionMenuItem("To smoothing (SM)", [m]() { return m->_useOffsetCvForSmooth; }, [m]() { m->_useOffsetCvForSmooth = true; }));
		OptionsMenuItem::addToMenu(uo, menu);
	}
};

Model* modelLFO = bogaudio::createModel<LFO, LFOWidget>("Bogaudio-LFO", "LFO", "Low frequency oscillator", "LFO", "Random", "Polyphonic");
