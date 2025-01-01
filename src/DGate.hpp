#pragma once

#include "bogaudio.hpp"
#include "trigger_on_load.hpp"

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

	struct Engine {
		bool firstStep = true;
		Trigger trigger;
		rack::dsp::PulseGenerator triggerOuptutPulseGen;
		Stage stage;
		float stageProgress;
		float delayLight;
		float gateLight;

		void reset();
	};

	Engine *_engines[maxChannels] {};

	DGate() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(DELAY_PARAM, 0.0f, 1.0f, 0.0f, "Delay", " s");
		configParam<EnvelopeSegmentParamQuantity>(GATE_PARAM, 0.0f, 1.0f, SQUARE_ROOT_ONE_TENTH, "Gate", " s");
		configSwitch(LOOP_PARAM, 0.0f, 1.0f, 1.0f, "Loop", {"Loop", "Stop"});
		configButton(TRIGGER_PARAM, "Trigger");

		configInput(TRIGGER_INPUT, "Trigger");

		configOutput(GATE_OUTPUT, "Gate");
		configOutput(END_OUTPUT, "End-of-cycle trigger");
	}

	void reset() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcess(const ProcessArgs& args) override;
	bool stepStage(int c, Param& knob);
	bool shouldTriggerOnNextLoad() override;
};

} // namespace bogaudio
