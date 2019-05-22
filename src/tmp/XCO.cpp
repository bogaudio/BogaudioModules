
#include "XCO.hpp"
#include "dsp/pitch.hpp"

void XCO::onReset() {
	_syncTrigger.reset();
	_modulationStep = modulationSteps;
}

void XCO::onSampleRateChange() {
	setSampleRate(engineGetSampleRate());
	_modulationStep = modulationSteps;
}

void XCO::step() {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].getValue() > 0.5f;
	_fmLinearMode = params[FM_TYPE_PARAM].getValue() < 0.5f;

	if (!(
		outputs[MIX_OUTPUT].isConnected() ||
		outputs[SQUARE_OUTPUT].isConnected() ||
		outputs[SAW_OUTPUT].isConnected() ||
		outputs[TRIANGLE_OUTPUT].isConnected() ||
		outputs[SINE_OUTPUT].isConnected()
	)) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		_baseVOct = params[FREQUENCY_PARAM].getValue();
		_baseVOct += params[FINE_PARAM].getValue() / 12.0f;;
		if (inputs[PITCH_INPUT].isConnected()) {
			_baseVOct += clamp(inputs[PITCH_INPUT].getVoltage(), -5.0f, 5.0f);
		}
		if (_slowMode) {
			_baseVOct -= 7.0f;
		}
		_baseHz = cvToFrequency(_baseVOct);

		float pw = params[SQUARE_PW_PARAM].getValue();
		if (inputs[SQUARE_PW_INPUT].isConnected()) {
			pw *= clamp(inputs[SQUARE_PW_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
		}
		pw *= 1.0f - 2.0f * _square.minPulseWidth;
		pw *= 0.5f;
		pw += 0.5f;
		_square.setPulseWidth(_squarePulseWidthSL.next(pw));

		float saturation = params[SAW_SATURATION_PARAM].getValue();
		if (inputs[SAW_SATURATION_INPUT].isConnected()) {
			saturation *= clamp(inputs[SAW_SATURATION_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		_saw.setSaturation(_sawSaturationSL.next(saturation) * 10.f);

		float tsw = params[TRIANGLE_SAMPLE_PARAM].getValue() * Phasor::maxSampleWidth;
		if (inputs[TRIANGLE_SAMPLE_INPUT].isConnected()) {
			tsw *= clamp(inputs[TRIANGLE_SAMPLE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		_triangleSampleWidth = _triangleSampleWidthSL.next(tsw);
		_triangle.setSampleWidth(_triangleSampleWidth);

		float sfb = params[SINE_FEEDBACK_PARAM].getValue();
		if (inputs[SINE_FEEDBACK_INPUT].isConnected()) {
			sfb *= clamp(inputs[SINE_FEEDBACK_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		_sineFeedback = _sineFeedbackSL.next(sfb);

		_fmDepth = params[FM_DEPTH_PARAM].getValue();
		if (inputs[FM_DEPTH_INPUT].isConnected()) {
			_fmDepth *= clamp(inputs[FM_DEPTH_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}

		_squarePhaseOffset = phaseOffset(params[SQUARE_PHASE_PARAM], inputs[SQUARE_PHASE_INPUT]);
		_sawPhaseOffset = phaseOffset(params[SAW_PHASE_PARAM], inputs[SAW_PHASE_INPUT]);
		_trianglePhaseOffset = phaseOffset(params[TRIANGLE_PHASE_PARAM], inputs[TRIANGLE_PHASE_INPUT]);
		_sinePhaseOffset = phaseOffset(params[SINE_PHASE_PARAM], inputs[SINE_PHASE_INPUT]);

		_squareMix = level(params[SQUARE_MIX_PARAM], inputs[SQUARE_MIX_INPUT]);
		_sawMix = level(params[SAW_MIX_PARAM], inputs[SAW_MIX_INPUT]);
		_triangleMix = level(params[TRIANGLE_MIX_PARAM], inputs[TRIANGLE_MIX_INPUT]);
		_sineMix = level(params[SINE_MIX_PARAM], inputs[SINE_MIX_INPUT]);
	}

	if (_syncTrigger.next(inputs[SYNC_INPUT].getVoltage())) {
		_phasor.resetPhase();
	}

	float frequency = _baseHz;
	Phasor::phase_delta_t phaseOffset = 0;
	float fmd = _fmDepthSL.next(_fmDepth);
	if (inputs[FM_INPUT].isConnected() && fmd > 0.01f) {
		float fm = inputs[FM_INPUT].getVoltage() * fmd;
		if (_fmLinearMode) {
			phaseOffset = Phasor::radiansToPhase(2.0f * fm);
		}
		else {
			frequency = cvToFrequency(_baseVOct + fm);
		}
	}
	setFrequency(frequency);

	const float oversampleWidth = 100.0f;
	float mix, oMix;
	if (frequency > _oversampleThreshold) {
		if (frequency > _oversampleThreshold + oversampleWidth) {
			mix = 0.0f;
			oMix = 1.0f;
		}
		else {
			oMix = (frequency - _oversampleThreshold) / oversampleWidth;
			mix = 1.0f - oMix;
		}
	}
	else {
		mix = 1.0f;
		oMix = 0.0f;
	}

	bool triangleSample = _triangleSampleWidth > 0.001f;
	bool squareActive = outputs[MIX_OUTPUT].isConnected() || outputs[SQUARE_OUTPUT].isConnected();
	bool sawActive = outputs[MIX_OUTPUT].isConnected() || outputs[SAW_OUTPUT].isConnected();
	bool triangleActive = outputs[MIX_OUTPUT].isConnected() || outputs[TRIANGLE_OUTPUT].isConnected();
	bool sineActive = outputs[MIX_OUTPUT].isConnected() || outputs[SINE_OUTPUT].isConnected();
	bool squareOversample = squareActive && oMix > 0.0f;
	bool sawOversample = sawActive && oMix > 0.0f;
	bool triangleOversample = triangleActive && (triangleSample || oMix > 0.0f);
	bool squareNormal = squareActive && mix > 0.0f;
	bool sawNormal = sawActive && mix > 0.0f;
	bool triangleNormal = triangleActive && !triangleSample && mix > 0.0f;
	float squareOut = 0.0f;
	float sawOut = 0.0f;
	float triangleOut = 0.0f;
	float sineOut = 0.0f;

	Phasor::phase_delta_t sineFeedbackOffset = 0;
	if (sineActive) {
		 if (_sineFeedback > 0.001f) {
			 sineFeedbackOffset = Phasor::radiansToPhase(_sineFeedback * _sineFeedbackDelayedSample);
			 if (_sineOMix < 1.0f) {
				 _sineOMix += sineOversampleMixIncrement;
			 }
		 }
		 else if (_sineOMix > 0.0f) {
			 _sineOMix -= sineOversampleMixIncrement;
		 }
	}

	if (squareOversample || sawOversample || triangleOversample || _sineOMix > 0.0f) {
		for (int i = 0; i < oversample; ++i) {
			_phasor.advancePhase();
			if (squareOversample) {
				_squareBuffer[i] = _square.nextFromPhasor(_phasor, _squarePhaseOffset + phaseOffset);
			}
			if (sawOversample) {
				_sawBuffer[i] = _saw.nextFromPhasor(_phasor, _sawPhaseOffset + phaseOffset);
			}
			if (triangleOversample) {
				_triangleBuffer[i] = _triangle.nextFromPhasor(_phasor, _trianglePhaseOffset + phaseOffset);
			}
			if (_sineOMix > 0.0f) {
				_sineBuffer[i] = _sine.nextFromPhasor(_phasor, sineFeedbackOffset + _sinePhaseOffset + phaseOffset);
			}
		}
		if (squareOversample) {
			squareOut += oMix * amplitude * _squareDecimator.next(_squareBuffer);
		}
		if (sawOversample) {
			sawOut += oMix * amplitude * _sawDecimator.next(_sawBuffer);
		}
		if (triangleOversample) {
			triangleOut += amplitude * _triangleDecimator.next(_triangleBuffer);
			if (!triangleSample) {
				triangleOut *= oMix;
			}
		}
		if (_sineOMix > 0.0f) {
			sineOut += amplitude * _sineOMix * _sineDecimator.next(_sineBuffer);
		}
	}
	else {
		_phasor.advancePhase(oversample);
	}

	if (squareNormal) {
		squareOut += mix * amplitude * _square.nextFromPhasor(_phasor, _squarePhaseOffset + phaseOffset);
	}
	if (sawNormal) {
		sawOut += mix * amplitude * _saw.nextFromPhasor(_phasor, _sawPhaseOffset + phaseOffset);
	}
	if (triangleNormal) {
		triangleOut += mix * amplitude * _triangle.nextFromPhasor(_phasor, _trianglePhaseOffset + phaseOffset);
	}
	if (_sineOMix < 1.0f) {
		sineOut += amplitude * (1.0f - _sineOMix) * _sine.nextFromPhasor(_phasor, sineFeedbackOffset + _sinePhaseOffset + phaseOffset);
	}

	outputs[SQUARE_OUTPUT].setVoltage(squareOut);
	outputs[SAW_OUTPUT].setVoltage(sawOut);
	outputs[TRIANGLE_OUTPUT].setVoltage(triangleOut);
	outputs[SINE_OUTPUT].setVoltage(_sineFeedbackDelayedSample = sineOut);
	if (outputs[MIX_OUTPUT].isConnected()) {
		outputs[MIX_OUTPUT].setVoltage(_squareMixSL.next(_squareMix) * squareOut + _sawMixSL.next(_sawMix) * sawOut + _triangleMixSL.next(_triangleMix) * triangleOut + _sineMixSL.next(_sineMix) * sineOut);
	}
}

Phasor::phase_delta_t XCO::phaseOffset(Param& param, Input& input) {
	float v = param.value;
	if (input.active) {
		v *= clamp(input.value / 5.0f, -1.0f, 1.0f);
	}
	return -v * Phasor::maxPhase / 2.0f;
}

float XCO::level(Param& param, Input& input) {
	float v = param.value;
	if (input.active) {
		v *= clamp(input.value / 10.0f, 0.0f, 1.0f);
	}
	return v;
}

void XCO::setSampleRate(float sampleRate) {
	_oversampleThreshold = 0.06f * sampleRate;

	_phasor.setSampleRate(sampleRate);
	_square.setSampleRate(sampleRate);
	_saw.setSampleRate(sampleRate);

	_squareDecimator.setParams(sampleRate, oversample);
	_sawDecimator.setParams(sampleRate, oversample);
	_triangleDecimator.setParams(sampleRate, oversample);
	_sineDecimator.setParams(sampleRate, oversample);

	_fmDepthSL.setParams(sampleRate, 5.0f, 1.0f);
	_squarePulseWidthSL.setParams(sampleRate, 0.1f, 2.0f);
	_sawSaturationSL.setParams(sampleRate, 1.0f, 1.0f);
	_triangleSampleWidthSL.setParams(sampleRate, 0.1f, 1.0f);
	_sineFeedbackSL.setParams(sampleRate, 0.1f, 1.0f);
	_squareMixSL.setParams(sampleRate, 5.0f, 1.0f);
	_sawMixSL.setParams(sampleRate, 5.0f, 1.0f);
	_triangleMixSL.setParams(sampleRate, 5.0f, 1.0f);
	_sineMixSL.setParams(sampleRate, 5.0f, 1.0f);
}

void XCO::setFrequency(float frequency) {
	if (_frequency != frequency && frequency < 0.475f * _phasor._sampleRate) {
		_frequency = frequency;
		_phasor.setFrequency(_frequency / (float)oversample);
		_square.setFrequency(_frequency);
		_saw.setFrequency(_frequency);
	}
}

struct XCOWidget : ModuleWidget {
	static constexpr int hp = 20;

	XCOWidget(XCO* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/XCO.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(40.0, 45.0);
		auto fineParamPosition = Vec(47.0, 153.0);
		auto slowParamPosition = Vec(121.0, 157.2);
		auto fmParamPosition = Vec(55.0, 194.0);
		auto fmTypeParamPosition = Vec(101.5, 256.5);
		auto squarePwParamPosition = Vec(147.0, 60.0);
		auto squarePhaseParamPosition = Vec(147.0, 148.0);
		auto squareMixParamPosition = Vec(147.0, 237.0);
		auto sawSaturationParamPosition = Vec(187.0, 60.0);
		auto sawPhaseParamPosition = Vec(187.0, 148.0);
		auto sawMixParamPosition = Vec(187.0, 237.0);
		auto triangleSampleParamPosition = Vec(227.0, 60.0);
		auto trianglePhaseParamPosition = Vec(227.0, 148.0);
		auto triangleMixParamPosition = Vec(227.0, 237.0);
		auto sineFeedbackParamPosition = Vec(267.0, 60.0);
		auto sinePhaseParamPosition = Vec(267.0, 148.0);
		auto sineMixParamPosition = Vec(267.0, 237.0);

		auto fmInputPosition = Vec(29.0, 251.0);
		auto fmDepthInputPosition = Vec(62.0, 251.0);
		auto squarePwInputPosition = Vec(143.0, 95.0);
		auto squarePhaseInputPosition = Vec(143.0, 183.0);
		auto squareMixInputPosition = Vec(143.0, 272.0);
		auto sawSaturationInputPosition = Vec(183.0, 95.0);
		auto sawPhaseInputPosition = Vec(183.0, 183.0);
		auto sawMixInputPosition = Vec(183.0, 272.0);
		auto triangleSampleInputPosition = Vec(223.0, 95.0);
		auto trianglePhaseInputPosition = Vec(223.0, 183.0);
		auto triangleMixInputPosition = Vec(223.0, 272.0);
		auto sineFeedbackInputPosition = Vec(263.0, 95.0);
		auto sinePhaseInputPosition = Vec(263.0, 183.0);
		auto sineMixInputPosition = Vec(263.0, 272.0);
		auto pitchInputPosition = Vec(17.0, 318.0);
		auto syncInputPosition = Vec(50.0, 318.0);

		auto squareOutputPosition = Vec(143.0, 318.0);
		auto sawOutputPosition = Vec(183.0, 318.0);
		auto triangleOutputPosition = Vec(223.0, 318.0);
		auto sineOutputPosition = Vec(263.0, 318.0);
		auto mixOutputPosition = Vec(103.0, 318.0);

		auto slowLightPosition = Vec(81.0, 158.5);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob68>(frequencyParamPosition, module, XCO::FREQUENCY_PARAM, -3.0, 6.0, 0.0));
		addParam(createParam<Knob16>(fineParamPosition, module, XCO::FINE_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<StatefulButton9>(slowParamPosition, module, XCO::SLOW_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Knob38>(fmParamPosition, module, XCO::FM_DEPTH_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<SliderSwitch2State14>(fmTypeParamPosition, module, XCO::FM_TYPE_PARAM, 0.0, 1.0, 1.0));
		addParam(createParam<Knob16>(squarePwParamPosition, module, XCO::SQUARE_PW_PARAM, -0.97, 0.97, 0.0));
		addParam(createParam<Knob16>(squarePhaseParamPosition, module, XCO::SQUARE_PHASE_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob16>(squareMixParamPosition, module, XCO::SQUARE_MIX_PARAM, 0.0, 1.0, 1.0));
		addParam(createParam<Knob16>(sawSaturationParamPosition, module, XCO::SAW_SATURATION_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Knob16>(sawPhaseParamPosition, module, XCO::SAW_PHASE_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob16>(sawMixParamPosition, module, XCO::SAW_MIX_PARAM, 0.0, 1.0, 1.0));
		addParam(createParam<Knob16>(triangleSampleParamPosition, module, XCO::TRIANGLE_SAMPLE_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Knob16>(trianglePhaseParamPosition, module, XCO::TRIANGLE_PHASE_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob16>(triangleMixParamPosition, module, XCO::TRIANGLE_MIX_PARAM, 0.0, 1.0, 1.0));
		addParam(createParam<Knob16>(sineFeedbackParamPosition, module, XCO::SINE_FEEDBACK_PARAM, 0.0, 1.0, 0.0));
		addParam(createParam<Knob16>(sinePhaseParamPosition, module, XCO::SINE_PHASE_PARAM, -1.0, 1.0, 0.0));
		addParam(createParam<Knob16>(sineMixParamPosition, module, XCO::SINE_MIX_PARAM, 0.0, 1.0, 1.0));

		addInput(createInput<Port24>(fmInputPosition, module, XCO::FM_INPUT));
		addInput(createInput<Port24>(fmDepthInputPosition, module, XCO::FM_DEPTH_INPUT));
		addInput(createInput<Port24>(squarePwInputPosition, module, XCO::SQUARE_PW_INPUT));
		addInput(createInput<Port24>(squarePhaseInputPosition, module, XCO::SQUARE_PHASE_INPUT));
		addInput(createInput<Port24>(squareMixInputPosition, module, XCO::SQUARE_MIX_INPUT));
		addInput(createInput<Port24>(sawSaturationInputPosition, module, XCO::SAW_SATURATION_INPUT));
		addInput(createInput<Port24>(sawPhaseInputPosition, module, XCO::SAW_PHASE_INPUT));
		addInput(createInput<Port24>(sawMixInputPosition, module, XCO::SAW_MIX_INPUT));
		addInput(createInput<Port24>(triangleSampleInputPosition, module, XCO::TRIANGLE_SAMPLE_INPUT));
		addInput(createInput<Port24>(trianglePhaseInputPosition, module, XCO::TRIANGLE_PHASE_INPUT));
		addInput(createInput<Port24>(triangleMixInputPosition, module, XCO::TRIANGLE_MIX_INPUT));
		addInput(createInput<Port24>(sineFeedbackInputPosition, module, XCO::SINE_FEEDBACK_INPUT));
		addInput(createInput<Port24>(sinePhaseInputPosition, module, XCO::SINE_PHASE_INPUT));
		addInput(createInput<Port24>(sineMixInputPosition, module, XCO::SINE_MIX_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, XCO::PITCH_INPUT));
		addInput(createInput<Port24>(syncInputPosition, module, XCO::SYNC_INPUT));

		addOutput(createOutput<Port24>(squareOutputPosition, module, XCO::SQUARE_OUTPUT));
		addOutput(createOutput<Port24>(sawOutputPosition, module, XCO::SAW_OUTPUT));
		addOutput(createOutput<Port24>(triangleOutputPosition, module, XCO::TRIANGLE_OUTPUT));
		addOutput(createOutput<Port24>(sineOutputPosition, module, XCO::SINE_OUTPUT));
		addOutput(createOutput<Port24>(mixOutputPosition, module, XCO::MIX_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(slowLightPosition, module, XCO::SLOW_LIGHT));
	}
};

Model* modelXCO = createModel<XCO, XCOWidget>("Bogaudio-XCO", "XCO",  "oscillator with wave mixer", OSCILLATOR_TAG);
