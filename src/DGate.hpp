#pragma once

#include "bogaudio.hpp"

extern Model* modelDGate;

namespace bogaudio {

struct DGate : TriggerOnLoadModule {
	enum ParamsIds {
		DELAY_PARAM,
		GATE_PARAM,
		LOOP_PARAM,
		TRIGGER_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TRIGGER_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		GATE_OUTPUT,
		END_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		DELAY_LIGHT,
		GATE_LIGHT,
		NUM_LIGHTS
	};

	enum Stage {
		STOPPED_STAGE,
		DELAY_STAGE,
		GATE_STAGE
	};

	bool _firstStep = true;
	Trigger _trigger;
	rack::dsp::PulseGenerator _triggerOuptutPulseGen;
	Stage _stage;
	float _stageProgress;

	DGate() : TriggerOnLoadModule(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		configParam<EnvelopeSegmentParamQuantity>(DELAY_PARAM, 0.0f, 1.0f, 0.0f, "Delay", " s");
		configParam<EnvelopeSegmentParamQuantity>(GATE_PARAM, 0.0f, 1.0f, 0.31623f, "Gate", " s");
		configParam(LOOP_PARAM, 0.0f, 1.0f, 1.0f, "Loop");
		configParam(TRIGGER_PARAM, 0.0f, 1.0f, 0.0f, "Trigger");

		onReset();
	}

	void onReset() override;
	void process(const ProcessArgs& args) override;
	bool stepStage(Param& knob);
	bool shouldTriggerOnNextLoad() override {
		return _stage != STOPPED_STAGE;
	};
};

} // namespace bogaudio
