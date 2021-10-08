#pragma once

#include "lpg_common.hpp"
#include "dsp/filters/multimode.hpp"

extern Model* modelMegaGate;

namespace bogaudio {

struct MegaGate : LPGEnvBaseModule {
	enum ParamsIds {
		RISE_PARAM,
		RISE_SHAPE_PARAM,
		FALL_PARAM,
		FALL_SHAPE_PARAM,
		MINIMUM_GATE_PARAM,
		TILT_PARAM,
		GATE_TO_TRIGGER_PARAM,
		TIMES_10X_PARAM,
		FILTERS_SERIAL_PARAM,
		LINEAR_VCA_PARAM,
		LPF_ENV_PARAM,
		LPF_ENV_ATTENUATOR_PARAM,
		LPF_BIAS_PARAM,
		LPF_BIAS_ATTENUATOR_PARAM,
		LPF_POLES_PARAM,
		HPF_ENV_PARAM,
		HPF_ENV_ATTENUATOR_PARAM,
		HPF_BIAS_PARAM,
		HPF_BIAS_ATTENUATOR_PARAM,
		HPF_POLES_PARAM,
		VCA_ENV_PARAM,
		VCA_ENV_ATTENUATOR_PARAM,
		VCA_BIAS_PARAM,
		VCA_BIAS_ATTENUATOR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		RISE_INPUT,
		FALL_INPUT,
		MINIMUM_GATE_INPUT,
		TILT_INPUT,
		VELOCITY_INPUT,
		SHAPE_INPUT,
		LEFT_INPUT,
		RIGHT_INPUT,
		GATE_INPUT,
		LPF_ENV_INPUT,
		LPF_BIAS_INPUT,
		HPF_ENV_INPUT,
		HPF_BIAS_INPUT,
		VCA_ENV_INPUT,
		VCA_BIAS_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		ENV_OUTPUT,
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LPF_POLES_1_LIGHT,
		LPF_POLES_2_LIGHT,
		LPF_POLES_3_LIGHT,
		LPF_POLES_4_LIGHT,
		HPF_POLES_1_LIGHT,
		HPF_POLES_2_LIGHT,
		HPF_POLES_3_LIGHT,
		HPF_POLES_4_LIGHT,
		NUM_LIGHTS
	};

	static constexpr float maxFilterCutoff = 20000.0f;

	struct Engine {
		Trigger trigger;
		rack::dsp::PulseGenerator gatePulseGen;
		float gateSeconds = 0.0f;
		float gateElapsedSeconds = 0.0f;
		RiseFallShapedSlewLimiter slew;
		Amplifier velocityAmp;
		bogaudio::dsp::SlewLimiter velocitySL;
		bogaudio::dsp::SlewLimiter tiltSL;
		MultimodeFilter4 leftLpf, rightLpf;
		MultimodeFilter4 leftHpf, rightHpf;
		MultimodeFilter4 leftFinalHP, rightFinalHP;
		Amplifier leftVca, rightVca;
		bogaudio::dsp::SlewLimiter leftVcaSL, rightVcaSL;

		void reset();
		void setSampleRate(float sr);
	};

	Engine* _engines[maxChannels] {};
	float _sampleRate = 0.0f;
	float _sampleTime = 0.0f;
	const float _maxVelocityDb = 0.0f;
	float _minVelocityDb = -6.0f;

	MegaGate()
	: LPGEnvBaseModule(GATE_TO_TRIGGER_PARAM, TIMES_10X_PARAM)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<TimeParamQuantity<300>>(RISE_PARAM, 0.0f, 1.0f, 0.57735f, "Rise time", " ms");
		configParam(RISE_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "Rise shape");
		configParam<TimeParamQuantity<1>>(FALL_PARAM, 0.0f, 1.0f, 0.5477226f, "Fall time", " s");
		configParam(FALL_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "Fall shape");
		configParam<TimeParamQuantity<1>>(MINIMUM_GATE_PARAM, 0.0f, 1.0f, 0.3162278f, "Minimum gate", " s");
		configParam(TILT_PARAM, -1.0f, 1.0f, 0.0f, "Tilt", "%", 0.0f, 100.0f);
		configButton(GATE_TO_TRIGGER_PARAM, "Gate to trigger mode");
		configButton(TIMES_10X_PARAM, "Timings 10X mode");
		configButton(FILTERS_SERIAL_PARAM, "Filters in series mode");
		configButton(LINEAR_VCA_PARAM, "Linear VCA mode");
		configParam(LPF_ENV_PARAM, -1.0f, 1.0f, 0.8f, "LPF envelope amount", "%", 0.0f, 100.0f);
		configParam(LPF_ENV_ATTENUATOR_PARAM, -1.0f, 1.0f, 0.0f, "LPF envelope amount CV", "%", 0.0f, 100.0f);
		configParam<ScaledSquaringParamQuantity<(int)maxFilterCutoff>>(LPF_BIAS_PARAM, 0.0f, 1.0f, 0.0f, "LPF cutoff", " HZ");
		configParam(LPF_BIAS_ATTENUATOR_PARAM, -1.0f, 1.0f, 0.0f, "LPF cutoff CV", "%", 0.0f, 100.0f);
		configSwitch(LPF_POLES_PARAM, 0.0f, 3.0f, 1.0f, "LPF poles", {"1", "2", "3", "4"});
		configParam(HPF_ENV_PARAM, -1.0f, 1.0f, 0.0f, "HPF envelope amount", "%", 0.0f, 100.0f);
		configParam(HPF_ENV_ATTENUATOR_PARAM, -1.0f, 1.0f, 0.0f, "HPF envelope amount CV", "%", 0.0f, 100.0f);
		configParam<ScaledSquaringParamQuantity<(int)maxFilterCutoff>>(HPF_BIAS_PARAM, 0.0f, 1.0f, 1.0f, "HPF cutoff", " HZ");
		configParam(HPF_BIAS_ATTENUATOR_PARAM, -1.0f, 1.0f, 0.0f, "HPF cutoff CV", "%", 0.0f, 100.0f);
		configSwitch(HPF_POLES_PARAM, 0.0f, 3.0f, 1.0f, "HPF poles", {"1", "2", "3", "4"});
		configParam(VCA_ENV_PARAM, -1.0f, 1.0f, 1.0f, "VCA envelope amount", "%", 0.0f, 100.0f);
		configParam(VCA_ENV_ATTENUATOR_PARAM, -1.0f, 1.0f, 0.0f, "VCA envelope amount CV", "%", 0.0f, 100.0f);
		configParam(VCA_BIAS_PARAM, 0.0f, 1.0f, 0.0f, "VCA level", "%", 0.0f, 100.0f);
		configParam(VCA_BIAS_ATTENUATOR_PARAM, -1.0f, 1.0f, 0.0f, "VCA level CV", "%", 0.0f, 100.0f);

		configInput(RISE_INPUT, "RISE");
		configInput(FALL_INPUT, "FALL");
		configInput(MINIMUM_GATE_INPUT, "MINIMUM_GATE");
		configInput(TILT_INPUT, "TILT");
		configInput(VELOCITY_INPUT, "VELOCITY");
		configInput(SHAPE_INPUT, "SHAPE");
		configInput(LEFT_INPUT, "LEFT");
		configInput(RIGHT_INPUT, "RIGHT");
		configInput(GATE_INPUT, "GATE");
		configInput(LPF_ENV_INPUT, "LPF_ENV");
		configInput(LPF_BIAS_INPUT, "LPF_BIAS");
		configInput(HPF_ENV_INPUT, "HPF_ENV");
		configInput(HPF_BIAS_INPUT, "HPF_BIAS");
		configInput(VCA_ENV_INPUT, "VCA_ENV");
		configInput(VCA_BIAS_INPUT, "VCA_BIAS");

		configOutput(ENV_OUTPUT, "ENV");
		configOutput(LEFT_OUTPUT, "LEFT");
		configOutput(RIGHT_OUTPUT, "RIGHT");
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
	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
