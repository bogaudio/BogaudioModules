#pragma once

#include "lpg_common.hpp"
#include "dsp/filters/multimode.hpp"

extern Model* modelLPG;

namespace bogaudio {

struct LPG : LPGEnvBaseModule {
	enum ParamsIds {
		RESPONSE_PARAM,
		LONG_PARAM,
		RISE_SHAPE_PARAM,
		FALL_SHAPE_PARAM,
		LPF_ENV_PARAM,
		LPF_BIAS_PARAM,
		VCA_ENV_PARAM,
		VCA_BIAS_PARAM,
		LINEAR_VCA_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		RESPONSE_INPUT,
		LPF_INPUT,
		VCA_INPUT,
		GATE_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	static constexpr float maxFilterCutoff = 20000.0f;

	struct Engine {
		Trigger trigger;
		float gateSeconds = 0.0f;
		float gateElapsedSeconds = 0.0f;
		RiseFallShapedSlewLimiter slew;
		MultimodeFilter4 lpf;
		MultimodeFilter4 finalHP;
		Amplifier vca;
		bogaudio::dsp::SlewLimiter vcaSL;

		void reset();
		void setSampleRate(float sr);
	};

	Engine* _engines[maxChannels] {};
	float _sampleRate = 0.0f;
	float _sampleTime = 0.0f;
	int _lpfPoles = 2;

	LPG()
	: LPGEnvBaseModule(-1, LONG_PARAM, 5.0f)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(RESPONSE_PARAM, 0.0f, 1.0f, 0.5f, "Response", "%", 0.0f, 100.0f);
		configButton(LONG_PARAM, "Long mode");
		configParam(RISE_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "Rise shape");
		configParam(FALL_SHAPE_PARAM, -1.0f, 1.0f, -0.5f, "Fall shape");
		configParam(LPF_ENV_PARAM, -1.0f, 1.0f, 0.8f, "LPF envelope amount", "%", 0.0f, 100.0f);
		configParam<ScaledSquaringParamQuantity<(int)maxFilterCutoff>>(LPF_BIAS_PARAM, 0.0f, 1.0f, 0.0f, "LPF cutoff", " HZ");
		configParam(VCA_ENV_PARAM, -1.0f, 1.0f, 1.0f, "VCA envelope amount", "%", 0.0f, 100.0f);
		configParam(VCA_BIAS_PARAM, 0.0f, 1.0f, 0.0f, "VCA level", "%", 0.0f, 100.0f);
		configButton(LINEAR_VCA_PARAM, "Linear VCA mode");

		configInput(RESPONSE_INPUT, "Response CV");
		configInput(LPF_INPUT, "LPF envelope response CV");
		configInput(VCA_INPUT, "VCA envelope response CV");
		configInput(GATE_INPUT, "Gate");
		configInput(IN_INPUT, "Signal");

		configOutput(OUT_OUTPUT, "Signal");
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
