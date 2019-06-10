#pragma once

#include "bogaudio.hpp"
#include "dsp/envelope.hpp"

extern Model* modelADSR;

namespace bogaudio {

struct ADSR : Module {
	enum ParamsIds {
		ATTACK_PARAM,
		DECAY_PARAM,
		SUSTAIN_PARAM,
		RELEASE_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		GATE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		ATTACK_LIGHT,
		DECAY_LIGHT,
		SUSTAIN_LIGHT,
		RELEASE_LIGHT,
		LINEAR_LIGHT,
		NUM_LIGHTS
	};

	const int modulationSteps = 100;
	int _modulationStep = 0;
	bool _linearMode = false;
	Trigger _gateTrigger;
	bogaudio::dsp::ADSR _envelope;

	ADSR() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(ATTACK_PARAM, 0.0f, 1.0f, 0.141421f, "Attack", "s");
		configParam<EnvelopeSegmentParamQuantity>(DECAY_PARAM, 0.0f, 1.0f, 0.31623f, "Decay", "s");
		configParam(SUSTAIN_PARAM, 0.0f, 1.0f, 1.0f, "Sustain", "%", 0.0f, 100.0f);
		configParam<EnvelopeSegmentParamQuantity>(RELEASE_PARAM, 0.0f, 1.0f, 0.31623f, "Release", "s");
		configParam(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear");

		onReset();
		onSampleRateChange();
	}

	void onReset() override;
	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
