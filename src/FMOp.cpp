
#include "FMOp.hpp"
#include "dsp/pitch.hpp"

#define LINEAR_LEVEL "linearLevel"
#define ANTIALIAS_FEEDBACK "antialias_feedback"
#define ANTIALIAS_DEPTH "antialias_depth"

float FMOp::RatioParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	if (v < 0.0f) {
		return std::max(1.0f + v, 0.01f);
	}
	v *= 9.0f;
	v += 1.0f;
	return v;
}

void FMOp::RatioParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}

	if (v < 1.0f) {
		v = v - 1.0f;
	}
	else {
		v -= 1.0f;
		v /= 9.0f;
	}
	setValue(v);
}

bool FMOp::LevelParamQuantity::isLinear() {
	return dynamic_cast<FMOp*>(module)->_linearLevel;
}

void FMOp::Engine::reset() {
	envelope.reset();
	gateTrigger.reset();
}

void FMOp::Engine::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	envelope.setSampleRate(sampleRate);
	phasor.setSampleRate(sampleRate);
	decimator.setParams(sampleRate, oversample);
	maxFrequency = 0.475f * sampleRate;
	feedbackSL.setParams(sampleRate, 5.0f, 1.0f);
	depthSL.setParams(sampleRate, 5.0f, 1.0f);
	levelSL.setParams(sampleRate, 10.0f, 1.0f);
	sustainSL.setParams(sampleRate, 1.0f, 1.0f);
}

json_t* FMOp::saveToJson(json_t* root) {
	json_object_set_new(root, LINEAR_LEVEL, json_boolean(_linearLevel));
	json_object_set_new(root, ANTIALIAS_FEEDBACK, json_boolean(_antiAliasFeedback));
	json_object_set_new(root, ANTIALIAS_DEPTH, json_boolean(_antiAliasDepth));
	return root;
}

void FMOp::loadFromJson(json_t* root) {
	json_t* ll = json_object_get(root, LINEAR_LEVEL);
	if (ll) {
		_linearLevel = json_is_true(ll);
	}

	json_t* aaf = json_object_get(root, ANTIALIAS_FEEDBACK);
	if (aaf) {
		_antiAliasFeedback = json_is_true(aaf);
	}

	json_t* aad = json_object_get(root, ANTIALIAS_DEPTH);
	if (aad) {
		_antiAliasDepth = json_is_true(aad);
	}
}

void FMOp::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void FMOp::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

bool FMOp::active() {
	return outputs[AUDIO_OUTPUT].isConnected();
}

int FMOp::channels() {
	return inputs[PITCH_INPUT].getChannels();
}

void FMOp::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->sampleRateChange();
	if (c > 0) {
		_engines[c]->phasor.syncPhase(_engines[0]->phasor);
	}
}

void FMOp::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void FMOp::modulate() {
	_levelEnvelopeOn = params[ENV_TO_LEVEL_PARAM].getValue() > 0.5f;
	_feedbackEnvelopeOn = params[ENV_TO_FEEDBACK_PARAM].getValue() > 0.5f;
	_depthEnvelopeOn = params[ENV_TO_DEPTH_PARAM].getValue() > 0.5f;
}

void FMOp::modulateChannel(int c) {
	Engine& e = *_engines[c];

	float pitchIn = 0.0f;
	if (inputs[PITCH_INPUT].isConnected()) {
		pitchIn = inputs[PITCH_INPUT].getVoltage(c);
	}
	float ratio = params[RATIO_PARAM].getValue();
	if (ratio < 0.0f) {
		ratio = std::max(1.0f + ratio, 0.01f);
	}
	else {
		ratio *= 9.0f;
		ratio += 1.0f;
	}

	float frequency = pitchIn;
	frequency += params[FINE_PARAM].getValue() / 12.0f;
	frequency = cvToFrequency(frequency);
	frequency *= ratio;
	frequency = clamp(frequency, -e.maxFrequency, e.maxFrequency);
	e.phasor.setFrequency(frequency / (float)oversample);

	bool envelopeOn = _levelEnvelopeOn || _feedbackEnvelopeOn || _depthEnvelopeOn;
	if (envelopeOn && !e.envelopeOn) {
		e.envelope.reset();
	}
	e.envelopeOn = envelopeOn;

	if (e.envelopeOn) {
		float sustain = params[SUSTAIN_PARAM].getValue();
		if (inputs[SUSTAIN_INPUT].isConnected()) {
			sustain *= clamp(inputs[SUSTAIN_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		e.envelope.setAttack(powf(params[ATTACK_PARAM].getValue(), 2.0f) * 10.f);
		e.envelope.setDecay(powf(params[DECAY_PARAM].getValue(), 2.0f) * 10.f);
		e.envelope.setSustain(e.sustainSL.next(sustain));
		e.envelope.setRelease(powf(params[RELEASE_PARAM].getValue(), 2.0f) * 10.f);
	}

	e.feedback = params[FEEDBACK_PARAM].getValue();
	if (inputs[FEEDBACK_INPUT].isConnected()) {
		e.feedback *= clamp(inputs[FEEDBACK_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}

	e.depth = params[DEPTH_PARAM].getValue();
	if (inputs[DEPTH_INPUT].isConnected()) {
		e.depth *= clamp(inputs[DEPTH_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}

	e.level = params[LEVEL_PARAM].getValue();
	if (inputs[LEVEL_INPUT].isConnected()) {
		e.level *= clamp(inputs[LEVEL_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
}

void FMOp::processAlways(const ProcessArgs& args) {
	_attackLightSum = _decayLightSum = _sustainLightSum = _releaseLightSum = 0;
}

void FMOp::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	float envelope = 0.0f;
	if (e.envelopeOn) {
		float gateIn = 0.0f;
		if (inputs[GATE_INPUT].isConnected()) {
			gateIn = inputs[GATE_INPUT].getPolyVoltage(c);
		}
		e.gateTrigger.process(gateIn);
		e.envelope.setGate(e.gateTrigger.isHigh());
		envelope = e.envelope.next();
	}

	float feedback = e.feedbackSL.next(e.feedback);
	if (_feedbackEnvelopeOn) {
		feedback *= envelope;
	}
	bool feedbackOn = feedback > 0.001f;

	float out = e.levelSL.next(e.level);
	if (_levelEnvelopeOn) {
		out *= envelope;
	}

	float offset = 0.0f;
	if (feedbackOn) {
		offset = feedback * e.feedbackDelayedSample;
	}

	bool depthOn = false;
	if (inputs[FM_INPUT].isConnected()) {
		float depth = e.depthSL.next(e.depth);
		if (_depthEnvelopeOn) {
			depth *= envelope;
		}
		offset += inputs[FM_INPUT].getPolyVoltage(c) * depth * 2.0f;
		depthOn = depth > 0.001f;
	}

	float sample = 0.0f;
	if (out > 0.0001f) {
		Phasor::phase_delta_t o = Phasor::radiansToPhase(offset);
		if ((feedbackOn && _antiAliasFeedback) || (depthOn && _antiAliasDepth)) {
			if (e.oversampleMix < 1.0f) {
				e.oversampleMix += oversampleMixIncrement;
			}
		}
		else if (e.oversampleMix > 0.0f) {
			e.oversampleMix -= oversampleMixIncrement;
		}

		if (e.oversampleMix > 0.0f) {
			for (int i = 0; i < oversample; ++i) {
				e.phasor.advancePhase();
				e.buffer[i] = e.sineTable.nextFromPhasor(e.phasor, o);
			}
			sample = e.oversampleMix * e.decimator.next(e.buffer);
		}
		else {
			e.phasor.advancePhase(oversample);
		}
		if (e.oversampleMix < 1.0f) {
			sample += (1.0f - e.oversampleMix) * e.sineTable.nextFromPhasor(e.phasor, o);
		}

		if (_linearLevel) {
			sample *= out;
		}
		else {
			out = (1.0f - out) * Amplifier::minDecibels;
			e.amplifier.setLevel(out);
			sample = e.amplifier.next(sample);
		}
	}
	else {
		e.phasor.advancePhase(oversample);
	}

	outputs[AUDIO_OUTPUT].setChannels(_channels);
	outputs[AUDIO_OUTPUT].setVoltage(e.feedbackDelayedSample = amplitude * sample, c);

	_attackLightSum += e.envelope.isStage(dsp::ADSR::ATTACK_STAGE);
	_decayLightSum += e.envelope.isStage(dsp::ADSR::DECAY_STAGE);
	_sustainLightSum += e.envelope.isStage(dsp::ADSR::SUSTAIN_STAGE);
	_releaseLightSum += e.envelope.isStage(dsp::ADSR::RELEASE_STAGE);
}

void FMOp::postProcessAlways(const ProcessArgs& args) {
	lights[ATTACK_LIGHT].value = _attackLightSum * _inverseChannels;
	lights[DECAY_LIGHT].value = _decayLightSum * _inverseChannels;
	lights[SUSTAIN_LIGHT].value = _sustainLightSum * _inverseChannels;
	lights[RELEASE_LIGHT].value = _releaseLightSum * _inverseChannels;
}

struct FMOpWidget : BGModuleWidget {
	static constexpr int hp = 10;

	FMOpWidget(FMOp* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "FMOp");
		createScrews();

		// generated by svg_widgets.rb
		auto ratioParamPosition = Vec(30.0, 45.0);
		auto fineParamPosition = Vec(112.0, 57.0);
		auto attackParamPosition = Vec(107.0, 94.0);
		auto decayParamPosition = Vec(107.0, 139.0);
		auto sustainParamPosition = Vec(107.0, 184.0);
		auto releaseParamPosition = Vec(107.0, 229.0);
		auto depthParamPosition = Vec(36.0, 106.0);
		auto envToDepthParamPosition = Vec(54.5, 139.7);
		auto feedbackParamPosition = Vec(36.0, 162.0);
		auto envToFeedbackParamPosition = Vec(54.5, 195.7);
		auto levelParamPosition = Vec(36.0, 218.0);
		auto envToLevelParamPosition = Vec(54.5, 251.7);

		auto depthInputPosition = Vec(15.0, 274.0);
		auto feedbackInputPosition = Vec(47.0, 274.0);
		auto levelInputPosition = Vec(79.0, 274.0);
		auto sustainInputPosition = Vec(111.0, 274.0);
		auto pitchInputPosition = Vec(15.0, 318.0);
		auto fmInputPosition = Vec(47.0, 318.0);
		auto gateInputPosition = Vec(79.0, 318.0);

		auto audioOutputPosition = Vec(111.0, 318.0);

		auto attackLightPosition = Vec(118.5, 123.0);
		auto decayLightPosition = Vec(118.5, 168.0);
		auto sustainLightPosition = Vec(118.5, 213.0);
		auto releaseLightPosition = Vec(118.5, 258.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(ratioParamPosition, module, FMOp::RATIO_PARAM));
		addParam(createParam<Knob16>(fineParamPosition, module, FMOp::FINE_PARAM));
		addParam(createParam<Knob26>(attackParamPosition, module, FMOp::ATTACK_PARAM));
		addParam(createParam<Knob26>(decayParamPosition, module, FMOp::DECAY_PARAM));
		addParam(createParam<Knob26>(sustainParamPosition, module, FMOp::SUSTAIN_PARAM));
		addParam(createParam<Knob26>(releaseParamPosition, module, FMOp::RELEASE_PARAM));
		addParam(createParam<Knob26>(depthParamPosition, module, FMOp::DEPTH_PARAM));
		addParam(createParam<Knob26>(feedbackParamPosition, module, FMOp::FEEDBACK_PARAM));
		addParam(createParam<Knob26>(levelParamPosition, module, FMOp::LEVEL_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(envToLevelParamPosition, module, FMOp::ENV_TO_LEVEL_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(envToFeedbackParamPosition, module, FMOp::ENV_TO_FEEDBACK_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(envToDepthParamPosition, module, FMOp::ENV_TO_DEPTH_PARAM));

		addInput(createInput<Port24>(sustainInputPosition, module, FMOp::SUSTAIN_INPUT));
		addInput(createInput<Port24>(depthInputPosition, module, FMOp::DEPTH_INPUT));
		addInput(createInput<Port24>(feedbackInputPosition, module, FMOp::FEEDBACK_INPUT));
		addInput(createInput<Port24>(levelInputPosition, module, FMOp::LEVEL_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, FMOp::PITCH_INPUT));
		addInput(createInput<Port24>(gateInputPosition, module, FMOp::GATE_INPUT));
		addInput(createInput<Port24>(fmInputPosition, module, FMOp::FM_INPUT));

		addOutput(createOutput<Port24>(audioOutputPosition, module, FMOp::AUDIO_OUTPUT));

		addChild(createLight<BGTinyLight<GreenLight>>(attackLightPosition, module, FMOp::ATTACK_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(decayLightPosition, module, FMOp::DECAY_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(sustainLightPosition, module, FMOp::SUSTAIN_LIGHT));
		addChild(createLight<BGTinyLight<GreenLight>>(releaseLightPosition, module, FMOp::RELEASE_LIGHT));
	}

	void contextMenu(Menu* menu) override {
		auto fmop = dynamic_cast<FMOp*>(module);
		assert(fmop);
		menu->addChild(new BoolOptionMenuItem("Linear level response", [fmop]() { return &fmop->_linearLevel; }));

		menu->addChild(new BoolOptionMenuItem("Anti-alias feedback", [fmop]() { return &fmop->_antiAliasFeedback; }));
		menu->addChild(new BoolOptionMenuItem("Anti-alias external FM", [fmop]() { return &fmop->_antiAliasDepth; }));
	}
};

Model* modelFMOp = bogaudio::createModel<FMOp, FMOpWidget>("Bogaudio-FMOp", "FM-OP", "FM operator / oscillator / synth voice", "Oscillator", "Synth voice", "Polyphonic");
