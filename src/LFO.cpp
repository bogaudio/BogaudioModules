
#include "LFO.hpp"

void LFO::onReset() {
	_resetTrigger.reset();
	_modulationStep = modulationSteps;
	_sampleStep = _phasor._sampleRate;
}

void LFO::onSampleRateChange() {
	_phasor.setSampleRate(APP->engine->getSampleRate());
	_modulationStep = modulationSteps;
	_sampleStep = _phasor._sampleRate;
}

void LFO::process(const ProcessArgs& args) {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].getValue() > 0.5f;
	if (!(
		outputs[SINE_OUTPUT].isConnected() ||
		outputs[TRIANGLE_OUTPUT].isConnected() ||
		outputs[RAMP_UP_OUTPUT].isConnected() ||
		outputs[RAMP_DOWN_OUTPUT].isConnected() ||
		outputs[SQUARE_OUTPUT].isConnected()
	)) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		setFrequency(_slowMode, params[FREQUENCY_PARAM], inputs[PITCH_INPUT], _phasor);

		float pw = params[PW_PARAM].getValue();
		if (inputs[PW_INPUT].isConnected()) {
			pw *= clamp(inputs[PW_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
		}
		pw *= 1.0f - 2.0f * _square.minPulseWidth;
		pw *= 0.5f;
		pw += 0.5f;
		_square.setPulseWidth(pw);

		float sample = params[SAMPLE_PARAM].getValue();
		if (inputs[SAMPLE_INPUT].isConnected()) {
			sample *= clamp(inputs[SAMPLE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		float maxSampleSteps = (_phasor._sampleRate / _phasor._frequency) / 4.0f;
		_sampleSteps = clamp((int)(sample * maxSampleSteps), 1, (int)maxSampleSteps);

		_offset = params[OFFSET_PARAM].getValue();
		if (inputs[OFFSET_INPUT].isConnected()) {
			_offset *= clamp(inputs[OFFSET_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
		}
		_offset *= 5.0f;

		_scale = params[SCALE_PARAM].getValue();
		if (inputs[SCALE_INPUT].isConnected()) {
			_scale *= clamp(inputs[SCALE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
	}

	if (_resetTrigger.next(inputs[RESET_INPUT].getVoltage())) {
		_phasor.resetPhase();
	}

	_phasor.advancePhase();
	bool useSample = false;
	if (_sampleSteps > 1) {
		++_sampleStep;
		if (_sampleStep >= _sampleSteps) {
			_sampleStep = 0;
		}
		else {
			useSample = true;
		}
	}
	updateOutput(_sine, useSample, false, outputs[SINE_OUTPUT], _sineSample, _sineActive);
	updateOutput(_triangle, useSample, false, outputs[TRIANGLE_OUTPUT], _triangleSample, _triangleActive);
	updateOutput(_ramp, useSample, false, outputs[RAMP_UP_OUTPUT], _rampUpSample, _rampUpActive);
	updateOutput(_ramp, useSample, true, outputs[RAMP_DOWN_OUTPUT], _rampDownSample, _rampDownActive);
	updateOutput(_square, false, false, outputs[SQUARE_OUTPUT], _squareSample, _squareActive);
}

void LFO::updateOutput(Phasor& wave, bool useSample, bool invert, Output& output, float& sample, bool& active) {
	if (output.active) {
		if (useSample && active) {
			output.value = sample;
		}
		else {
			sample = wave.nextFromPhasor(_phasor) * amplitude * _scale;
			if (invert) {
				sample = -sample;
			}
			sample += _offset;
			output.value = sample;
		}
		active = true;
	}
	else {
		active = false;
	}
}

struct LFOWidget : LFOBaseWidget {
	static constexpr int hp = 10;

	LFOWidget(LFO* module)
	: LFOBaseWidget(
		module,
		new SVGPanel(),
		"res/LFO-classic.svg",
		"res/LFO.svg"
	) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		_panel->box.size = box.size;
		addChild(_panel);

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(41.0, 45.0);
		auto slowParamPosition = Vec(120.0, 249.0);
		auto sampleParamPosition = Vec(37.0, 150.0);
		auto pwParamPosition = Vec(102.0, 150.0);
		auto offsetParamPosition = Vec(42.0, 196.0);
		auto scaleParamPosition = Vec(107.0, 196.0);

		auto sampleInputPosition = Vec(15.0, 230.0);
		auto pwInputPosition = Vec(47.0, 230.0);
		auto offsetInputPosition = Vec(15.0, 274.0);
		auto scaleInputPosition = Vec(47.0, 274.0);
		auto pitchInputPosition = Vec(15.0, 318.0);
		auto resetInputPosition = Vec(47.0, 318.0);

		auto rampDownOutputPosition = Vec(79.0, 230.0);
		auto rampUpOutputPosition = Vec(79.0, 274.0);
		auto squareOutputPosition = Vec(111.0, 274.0);
		auto triangleOutputPosition = Vec(79.0, 318.0);
		auto sineOutputPosition = Vec(111.0, 318.0);

		auto slowLightPosition = Vec(111.0, 240.0);
		// end generated by svg_widgets.rb

		_frequencyKnob = createParam<Knob68>(frequencyParamPosition, module, LFO::FREQUENCY_PARAM);
		addParam(_frequencyKnob);
		addParam(createParam<StatefulButton9>(slowParamPosition, module, LFO::SLOW_PARAM));
		addParam(createParam<Knob26>(sampleParamPosition, module, LFO::SAMPLE_PARAM));
		addParam(createParam<Knob26>(pwParamPosition, module, LFO::PW_PARAM));
		addParam(createParam<Knob16>(offsetParamPosition, module, LFO::OFFSET_PARAM));
		addParam(createParam<Knob16>(scaleParamPosition, module, LFO::SCALE_PARAM));

		addInput(createInput<Port24>(sampleInputPosition, module, LFO::SAMPLE_INPUT));
		addInput(createInput<Port24>(pwInputPosition, module, LFO::PW_INPUT));
		addInput(createInput<Port24>(offsetInputPosition, module, LFO::OFFSET_INPUT));
		addInput(createInput<Port24>(scaleInputPosition, module, LFO::SCALE_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, LFO::PITCH_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, LFO::RESET_INPUT));

		addOutput(createOutput<Port24>(rampUpOutputPosition, module, LFO::RAMP_UP_OUTPUT));
		addOutput(createOutput<Port24>(rampDownOutputPosition, module, LFO::RAMP_DOWN_OUTPUT));
		addOutput(createOutput<Port24>(squareOutputPosition, module, LFO::SQUARE_OUTPUT));
		addOutput(createOutput<Port24>(triangleOutputPosition, module, LFO::TRIANGLE_OUTPUT));
		addOutput(createOutput<Port24>(sineOutputPosition, module, LFO::SINE_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(slowLightPosition, module, LFO::SLOW_LIGHT));
	}
};

Model* modelLFO = bogaudio::createModel<LFO, LFOWidget>("Bogaudio-LFO", "LFO",  "low-frequency oscillator", LFO_TAG);
