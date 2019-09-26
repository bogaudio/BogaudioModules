
#include "XCO.hpp"
#include "dsp/pitch.hpp"

float XCO::XCOFrequencyParamQuantity::offset() {
	XCO* xco = dynamic_cast<XCO*>(module);
	return xco->_slowMode ? xco->_slowModeOffset : 0.0f;
}

void XCO::Engine::reset() {
	syncTrigger.reset();
}

void XCO::Engine::sampleRateChange(float sampleRate) {
	phasor.setSampleRate(sampleRate);
	square.setSampleRate(sampleRate);
	saw.setSampleRate(sampleRate);

	squareDecimator.setParams(sampleRate, oversample);
	sawDecimator.setParams(sampleRate, oversample);
	triangleDecimator.setParams(sampleRate, oversample);
	sineDecimator.setParams(sampleRate, oversample);

	fmDepthSL.setParams(sampleRate, 5.0f, 1.0f);
	squarePulseWidthSL.setParams(sampleRate, 0.1f, 2.0f);
	sawSaturationSL.setParams(sampleRate, 1.0f, 1.0f);
	triangleSampleWidthSL.setParams(sampleRate, 0.1f, 1.0f);
	sineFeedbackSL.setParams(sampleRate, 0.1f, 1.0f);
	squareMixSL.setParams(sampleRate, 5.0f, 1.0f);
	sawMixSL.setParams(sampleRate, 5.0f, 1.0f);
	triangleMixSL.setParams(sampleRate, 5.0f, 1.0f);
	sineMixSL.setParams(sampleRate, 5.0f, 1.0f);
}

void XCO::Engine::setFrequency(float f) {
	if (frequency != f && frequency < 0.475f * phasor._sampleRate) {
		frequency = f;
		phasor.setFrequency(frequency / (float)oversample);
		square.setFrequency(frequency);
		saw.setFrequency(frequency);
	}
}

void XCO::reset() {
	for (int c = 0; c < maxChannels; ++c) {
		if (_engines[c]) {
			_engines[c]->reset();
		}
	}
}

void XCO::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	_oversampleThreshold = 0.06f * sampleRate;

	for (int c = 0; c < maxChannels; ++c) {
		if (_engines[c]) {
			_engines[c]->sampleRateChange(sampleRate);
		}
	}
}

bool XCO::active() {
	return (
		outputs[MIX_OUTPUT].isConnected() ||
		outputs[SQUARE_OUTPUT].isConnected() ||
		outputs[SAW_OUTPUT].isConnected() ||
		outputs[TRIANGLE_OUTPUT].isConnected() ||
		outputs[SINE_OUTPUT].isConnected()
	);
}

int XCO::channels() {
	return std::max(1, inputs[PITCH_INPUT].getChannels());
}

void XCO::addEngine(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->sampleRateChange(APP->engine->getSampleRate());
}

void XCO::removeEngine(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void XCO::modulate() {
	_fmLinearMode = params[FM_TYPE_PARAM].getValue() < 0.5f;
}

void XCO::modulateChannel(int c) {
	_engines[c]->baseVOct = params[FREQUENCY_PARAM].getValue();
	_engines[c]->baseVOct += params[FINE_PARAM].getValue() / 12.0f;;
	if (inputs[PITCH_INPUT].isConnected()) {
		_engines[c]->baseVOct += clamp(inputs[PITCH_INPUT].getVoltage(c), -5.0f, 5.0f);
	}
	if (_slowMode) {
		_engines[c]->baseVOct += _slowModeOffset;
	}
	_engines[c]->baseHz = cvToFrequency(_engines[c]->baseVOct);

	float pw = params[SQUARE_PW_PARAM].getValue();
	if (inputs[SQUARE_PW_INPUT].isConnected()) {
		pw *= clamp(inputs[SQUARE_PW_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	pw *= 1.0f - 2.0f * _engines[c]->square.minPulseWidth;
	pw *= 0.5f;
	pw += 0.5f;
	_engines[c]->square.setPulseWidth(_engines[c]->squarePulseWidthSL.next(pw));

	float saturation = params[SAW_SATURATION_PARAM].getValue();
	if (inputs[SAW_SATURATION_INPUT].isConnected()) {
		saturation *= clamp(inputs[SAW_SATURATION_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	_engines[c]->saw.setSaturation(_engines[c]->sawSaturationSL.next(saturation) * 10.f);

	float tsw = params[TRIANGLE_SAMPLE_PARAM].getValue() * Phasor::maxSampleWidth;
	if (inputs[TRIANGLE_SAMPLE_INPUT].isConnected()) {
		tsw *= clamp(inputs[TRIANGLE_SAMPLE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	_engines[c]->triangleSampleWidth = _engines[c]->triangleSampleWidthSL.next(tsw);
	_engines[c]->triangle.setSampleWidth(_engines[c]->triangleSampleWidth);

	float sfb = params[SINE_FEEDBACK_PARAM].getValue();
	if (inputs[SINE_FEEDBACK_INPUT].isConnected()) {
		sfb *= clamp(inputs[SINE_FEEDBACK_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	_engines[c]->sineFeedback = _engines[c]->sineFeedbackSL.next(sfb);

	_engines[c]->fmDepth = params[FM_DEPTH_PARAM].getValue();
	if (inputs[FM_DEPTH_INPUT].isConnected()) {
		_engines[c]->fmDepth *= clamp(inputs[FM_DEPTH_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}

	_engines[c]->squarePhaseOffset = phaseOffset(c, params[SQUARE_PHASE_PARAM], inputs[SQUARE_PHASE_INPUT]);
	_engines[c]->sawPhaseOffset = phaseOffset(c, params[SAW_PHASE_PARAM], inputs[SAW_PHASE_INPUT]);
	_engines[c]->trianglePhaseOffset = phaseOffset(c, params[TRIANGLE_PHASE_PARAM], inputs[TRIANGLE_PHASE_INPUT]);
	_engines[c]->sinePhaseOffset = phaseOffset(c, params[SINE_PHASE_PARAM], inputs[SINE_PHASE_INPUT]);

	_engines[c]->squareMix = level(c, params[SQUARE_MIX_PARAM], inputs[SQUARE_MIX_INPUT]);
	_engines[c]->sawMix = level(c, params[SAW_MIX_PARAM], inputs[SAW_MIX_INPUT]);
	_engines[c]->triangleMix = level(c, params[TRIANGLE_MIX_PARAM], inputs[TRIANGLE_MIX_INPUT]);
	_engines[c]->sineMix = level(c, params[SINE_MIX_PARAM], inputs[SINE_MIX_INPUT]);
}

void XCO::always(const ProcessArgs& args) {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].getValue() > 0.5f;
}

void XCO::processChannel(const ProcessArgs& args, int c) {
	if (_engines[c]->syncTrigger.next(inputs[SYNC_INPUT].getPolyVoltage(c))) {
		_engines[c]->phasor.resetPhase();
	}

	float frequency = _engines[c]->baseHz;
	Phasor::phase_delta_t phaseOffset = 0;
	float fmd = _engines[c]->fmDepthSL.next(_engines[c]->fmDepth);
	if (inputs[FM_INPUT].isConnected() && fmd > 0.01f) {
		float fm = inputs[FM_INPUT].getPolyVoltage(c) * fmd;
		if (_fmLinearMode) {
			phaseOffset = Phasor::radiansToPhase(2.0f * fm);
		}
		else {
			frequency = cvToFrequency(_engines[c]->baseVOct + fm);
		}
	}
	_engines[c]->setFrequency(frequency);

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

	bool triangleSample = _engines[c]->triangleSampleWidth > 0.001f;
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
		 if (_engines[c]->sineFeedback > 0.001f) {
			 sineFeedbackOffset = Phasor::radiansToPhase(_engines[c]->sineFeedback * _engines[c]->sineFeedbackDelayedSample);
			 if (_engines[c]->sineOMix < 1.0f) {
				 _engines[c]->sineOMix += sineOversampleMixIncrement;
			 }
		 }
		 else if (_engines[c]->sineOMix > 0.0f) {
			 _engines[c]->sineOMix -= sineOversampleMixIncrement;
		 }
	}

	if (squareOversample || sawOversample || triangleOversample || _engines[c]->sineOMix > 0.0f) {
		for (int i = 0; i < Engine::oversample; ++i) {
			_engines[c]->phasor.advancePhase();
			if (squareOversample) {
				_engines[c]->squareBuffer[i] = _engines[c]->square.nextFromPhasor(_engines[c]->phasor, _engines[c]->squarePhaseOffset + phaseOffset);
			}
			if (sawOversample) {
				_engines[c]->sawBuffer[i] = _engines[c]->saw.nextFromPhasor(_engines[c]->phasor, _engines[c]->sawPhaseOffset + phaseOffset);
			}
			if (triangleOversample) {
				_engines[c]->triangleBuffer[i] = _engines[c]->triangle.nextFromPhasor(_engines[c]->phasor, _engines[c]->trianglePhaseOffset + phaseOffset);
			}
			if (_engines[c]->sineOMix > 0.0f) {
				_engines[c]->sineBuffer[i] = _engines[c]->sine.nextFromPhasor(_engines[c]->phasor, sineFeedbackOffset + _engines[c]->sinePhaseOffset + phaseOffset);
			}
		}
		if (squareOversample) {
			squareOut += oMix * amplitude * _engines[c]->squareDecimator.next(_engines[c]->squareBuffer);
		}
		if (sawOversample) {
			sawOut += oMix * amplitude * _engines[c]->sawDecimator.next(_engines[c]->sawBuffer);
		}
		if (triangleOversample) {
			triangleOut += amplitude * _engines[c]->triangleDecimator.next(_engines[c]->triangleBuffer);
			if (!triangleSample) {
				triangleOut *= oMix;
			}
		}
		if (_engines[c]->sineOMix > 0.0f) {
			sineOut += amplitude * _engines[c]->sineOMix * _engines[c]->sineDecimator.next(_engines[c]->sineBuffer);
		}
	}
	else {
		_engines[c]->phasor.advancePhase(Engine::oversample);
	}

	if (squareNormal) {
		squareOut += mix * amplitude * _engines[c]->square.nextFromPhasor(_engines[c]->phasor, _engines[c]->squarePhaseOffset + phaseOffset);
	}
	if (sawNormal) {
		sawOut += mix * amplitude * _engines[c]->saw.nextFromPhasor(_engines[c]->phasor, _engines[c]->sawPhaseOffset + phaseOffset);
	}
	if (triangleNormal) {
		triangleOut += mix * amplitude * _engines[c]->triangle.nextFromPhasor(_engines[c]->phasor, _engines[c]->trianglePhaseOffset + phaseOffset);
	}
	if (_engines[c]->sineOMix < 1.0f) {
		sineOut += amplitude * (1.0f - _engines[c]->sineOMix) * _engines[c]->sine.nextFromPhasor(_engines[c]->phasor, sineFeedbackOffset + _engines[c]->sinePhaseOffset + phaseOffset);
	}

	outputs[SQUARE_OUTPUT].setChannels(_channels);
	outputs[SQUARE_OUTPUT].setVoltage(squareOut, c);
	outputs[SAW_OUTPUT].setChannels(_channels);
	outputs[SAW_OUTPUT].setVoltage(sawOut, c);
	outputs[TRIANGLE_OUTPUT].setChannels(_channels);
	outputs[TRIANGLE_OUTPUT].setVoltage(triangleOut, c);
	outputs[SINE_OUTPUT].setChannels(_channels);
	outputs[SINE_OUTPUT].setVoltage(_engines[c]->sineFeedbackDelayedSample = sineOut, c);
	if (outputs[MIX_OUTPUT].isConnected()) {
		float mix = _engines[c]->squareMixSL.next(_engines[c]->squareMix) * squareOut;
		mix += _engines[c]->sawMixSL.next(_engines[c]->sawMix) * sawOut;
		mix += _engines[c]->triangleMixSL.next(_engines[c]->triangleMix) * triangleOut;
		mix += _engines[c]->sineMixSL.next(_engines[c]->sineMix) * sineOut;
		outputs[MIX_OUTPUT].setChannels(_channels);
		outputs[MIX_OUTPUT].setVoltage(mix, c);
	}
}

Phasor::phase_delta_t XCO::phaseOffset(int c, Param& param, Input& input) {
	float v = param.getValue();
	if (input.isConnected()) {
		v *= clamp(input.getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	return -v * Phasor::maxPhase / 2.0f;
}

float XCO::level(int c, Param& param, Input& input) {
	float v = param.getValue();
	if (input.isConnected()) {
		v *= clamp(input.getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	return v;
}

struct XCOWidget : ModuleWidget {
	static constexpr int hp = 20;

	XCOWidget(XCO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/XCO.svg")));
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

		addParam(createParam<Knob68>(frequencyParamPosition, module, XCO::FREQUENCY_PARAM));
		addParam(createParam<Knob16>(fineParamPosition, module, XCO::FINE_PARAM));
		addParam(createParam<StatefulButton9>(slowParamPosition, module, XCO::SLOW_PARAM));
		addParam(createParam<Knob38>(fmParamPosition, module, XCO::FM_DEPTH_PARAM));
		addParam(createParam<SliderSwitch2State14>(fmTypeParamPosition, module, XCO::FM_TYPE_PARAM));
		addParam(createParam<Knob16>(squarePwParamPosition, module, XCO::SQUARE_PW_PARAM));
		addParam(createParam<Knob16>(squarePhaseParamPosition, module, XCO::SQUARE_PHASE_PARAM));
		addParam(createParam<Knob16>(squareMixParamPosition, module, XCO::SQUARE_MIX_PARAM));
		addParam(createParam<Knob16>(sawSaturationParamPosition, module, XCO::SAW_SATURATION_PARAM));
		addParam(createParam<Knob16>(sawPhaseParamPosition, module, XCO::SAW_PHASE_PARAM));
		addParam(createParam<Knob16>(sawMixParamPosition, module, XCO::SAW_MIX_PARAM));
		addParam(createParam<Knob16>(triangleSampleParamPosition, module, XCO::TRIANGLE_SAMPLE_PARAM));
		addParam(createParam<Knob16>(trianglePhaseParamPosition, module, XCO::TRIANGLE_PHASE_PARAM));
		addParam(createParam<Knob16>(triangleMixParamPosition, module, XCO::TRIANGLE_MIX_PARAM));
		addParam(createParam<Knob16>(sineFeedbackParamPosition, module, XCO::SINE_FEEDBACK_PARAM));
		addParam(createParam<Knob16>(sinePhaseParamPosition, module, XCO::SINE_PHASE_PARAM));
		addParam(createParam<Knob16>(sineMixParamPosition, module, XCO::SINE_MIX_PARAM));

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

Model* modelXCO = bogaudio::createModel<XCO, XCOWidget>("Bogaudio-XCO", "XCO",  "oscillator with wave mixer", "Oscillator");
