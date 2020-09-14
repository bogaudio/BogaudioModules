#pragma once

#include "lpg_common.hpp"

extern Model* modelVish;

namespace bogaudio {

struct Vish : LPGEnvBaseModule {
	enum ParamsIds {
		RISE_PARAM,
		RISE_SHAPE_PARAM,
		FALL_PARAM,
		FALL_SHAPE_PARAM,
		MINIMUM_GATE_PARAM,
		GATE_TO_TRIGGER_PARAM,
		TIMES_10X_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		MINIMUM_GATE_INPUT,
		RISE_INPUT,
		VELOCITY_INPUT,
		FALL_INPUT,
		GATE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		Trigger trigger;
		rack::dsp::PulseGenerator gatePulseGen;
		float gateSeconds = 0.0f;
		float gateElapsedSeconds = 0.0f;
		RiseFallShapedSlewLimiter slew;
		Amplifier velocityAmp;
		bogaudio::dsp::SlewLimiter velocitySL;

		void reset();
		void setSampleRate(float sr);
	};

	Engine* _engines[maxChannels] {};
	float _sampleRate = 0.0f;
	float _sampleTime = 0.0f;

	Vish()
	: LPGEnvBaseModule(GATE_TO_TRIGGER_PARAM, TIMES_10X_PARAM)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<TimeParamQuantity<300>>(RISE_PARAM, 0.0f, 1.0f, 0.57735f, "Rise time", " ms");
		configParam(RISE_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "Rise shape");
		configParam<TimeParamQuantity<1>>(FALL_PARAM, 0.0f, 1.0f, 0.5477226f, "Fall time", " s");
		configParam(FALL_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "Fall shape");
		configParam<TimeParamQuantity<1>>(MINIMUM_GATE_PARAM, 0.0f, 1.0f, 0.3162278f, "Minimum gate", " s");
		configParam(GATE_TO_TRIGGER_PARAM, 0.0f, 1.0f, 1.0f, "Gate to trigger mode");
		configParam(TIMES_10X_PARAM, 0.0f, 1.0f, 0.0f, "Timings 10X mode");
	}

	void reset() override;
	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
