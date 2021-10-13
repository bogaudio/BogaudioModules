#pragma once

#include "bogaudio.hpp"
#include "output_range.hpp"

extern Model* modelRGate;

namespace bogaudio {

struct RGate : OutputRangeModule<BGModule> {
	enum ParamsIds {
		LENGTH_PARAM,
		CLOCK_DIVIDE_PARAM,
		CLOCK_MULTIPLY_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LENGTH_INPUT,
		CLOCK_DIVIDE_INPUT,
		RESET_INPUT,
		CLOCK_MULTIPLE_INPUT,
		CLOCK_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		GATE_OUTPUT,
		NUM_OUTPUTS
	};

	enum ResetMode {
		HARD_RESETMODE,
		SOFT_RESETMODE
	};
	static constexpr ResetMode defaultResetMode = HARD_RESETMODE;

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

		void reset(bool triggers, bool hard, float initialClock);
	};

	static constexpr float defaultInitialClockPeriod = 0.5f;

	Engine* _engines[maxChannels] {};
	float _sampleTime = 0.001f;
	ResetMode _resetMode = defaultResetMode;
	float _initialClockPeriod = defaultInitialClockPeriod;
	int _polyInputID = CLOCK_INPUT;

	RGate() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LENGTH_PARAM, 0.0f, 1.0f, 0.5f, "Gate length", "%", 0.0f, 100.0f);
		configParam<RoundingParamQuantity<ScaledSquaringParamQuantity<63>>>(CLOCK_DIVIDE_PARAM, 0.0f, 1.0f, 0.0f, "Clock division", "", 0.0f, 1.0f, 1.0f);
		configParam<RoundingParamQuantity<ScaledSquaringParamQuantity<63>>>(CLOCK_MULTIPLY_PARAM, 0.0f, 1.0f, 0.0f, "Clock multiplication", "", 0.0f, 1.0f, 1.0f);

		configInput(LENGTH_INPUT, "Length CV");
		configInput(CLOCK_DIVIDE_INPUT, "Clock divider CV");
		configInput(RESET_INPUT, "Reset");
		configInput(CLOCK_MULTIPLE_INPUT, "Clock multiplier CV");
		configInput(CLOCK_INPUT, "Clock");

		configOutput(GATE_OUTPUT, "GATE");

		_rangeOffset = 1.0f;
		_rangeScale = 5.0f;
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
