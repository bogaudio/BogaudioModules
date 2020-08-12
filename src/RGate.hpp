#pragma once

#include "bogaudio.hpp"

extern Model* modelRGate;

namespace bogaudio {

struct RGate : BGModule {
	enum ParamsIds {
		LENGTH_PARAM,
		CLOCK_DIVIDE_PARAM,
		CLOCK_MULTIPLY_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LENGTH_INPUT,
		CLOCK_DIVIDE_INPUT,
		RUN_INPUT,
		CLOCK_MULTIPLE_INPUT,
		CLOCK_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		GATE_OUTPUT,
		NUM_OUTPUTS
	};

	enum RunMode {
		GATED_RUN_RUNMODE,
		GATED_RUN_RESET_SOFT_RUNMODE,
		GATED_RUN_RESET_HARD_RUNMODE,
		TRIGGERED_RUN_RUNMODE,
		TRIGGERED_RUN_RESET_SOFT_RUNMODE,
		TRIGGERED_RUN_RESET_HARD_RUNMODE,
		RESET_SOFT_RUNMODE,
		RESET_HARD_RUNMODE
	};
	static constexpr RunMode defaultRunMode = TRIGGERED_RUN_RESET_HARD_RUNMODE;

	enum Running {
		UNKNOWN_RUNNING,
		YES_RUNNING,
		NO_RUNNING
	};

	struct Engine {
		Trigger clockTrigger;
		Trigger runTrigger;
		rack::dsp::PulseGenerator initialGatePulseGen;
		float gatePercentage = 0.0f;
		int division = 1;
		int multiplication = 1;
		float secondsSinceLastClock = -1.0f;
		float clockSeconds = -1.0f;
		float dividedSeconds = -1.0f;
		float multipliedSeconds = -1.0f;
		float gateSeconds = 0.1f;
		int dividerCount = 0;
		float dividedProgressSeconds = 0.0f;

		void reset(bool triggers = true, bool hard = true);
	};

	Engine* _engines[maxChannels] {};
	float _sampleTime = 0.001f;
	RunMode _runMode = defaultRunMode;
	Running _running = UNKNOWN_RUNNING;
	float _initialPulseSeconds = 0.2f;
	int _polyInputID = CLOCK_INPUT;

	RGate() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LENGTH_PARAM, 0.0f, 1.0f, 0.5f, "Gate length", "%", 0.0f, 100.0f);
		configParam<RoundingParamQuantity<ScaledSquaringParamQuantity<63>>>(CLOCK_DIVIDE_PARAM, 0.0f, 1.0f, 0.0f, "Clock division", "", 0.0f, 1.0f, 1.0f);
		configParam<RoundingParamQuantity<ScaledSquaringParamQuantity<63>>>(CLOCK_MULTIPLY_PARAM, 0.0f, 1.0f, 0.0f, "Clock multiplication", "", 0.0f, 1.0f, 1.0f);
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
