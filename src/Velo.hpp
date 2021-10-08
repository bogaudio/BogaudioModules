#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVelo;

namespace bogaudio {

struct Velo : BGModule {
	enum ParamsIds {
		LEVEL_PARAM,
		LEVEL_ATTENUATOR_PARAM,
		VELOCITY_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LEVEL_INPUT,
		CV_INPUT,
		VELOCITY_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	struct LevelParamQuantity : AmplifierParamQuantity {
		bool isLinear() override;
	};

	Amplifier _amplifier[maxChannels];
	bogaudio::dsp::SlewLimiter _levelSL[maxChannels];
	Amplifier _velocity[maxChannels];
	bogaudio::dsp::SlewLimiter _velocitySL[maxChannels];
	Saturator _saturator[maxChannels];
	float _velocityDb = 0.0f;
	bool _levelScalesCV = true;

	Velo() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<LevelParamQuantity>(LEVEL_PARAM, 0.0f, 1.0f, 0.8f, "level");
		configParam(LEVEL_ATTENUATOR_PARAM, -1.0f, 1.0f, 0.0f, "Level CV");
		configParam<ScaledSquaringParamQuantity<-60>>(VELOCITY_PARAM, 0.0f, 1.0f, 0.3162278f, "Velocity range", " dB");
		configSwitch(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear", {"Disabled", "Enabled"});
		configBypass(IN_INPUT, OUT_OUTPUT);

		configInput(LEVEL_INPUT, "LEVEL");
		configInput(CV_INPUT, "CV");
		configInput(VELOCITY_INPUT, "VELOCITY");
		configInput(IN_INPUT, "IN");

		configOutput(OUT_OUTPUT, "OUT");
	}

	inline bool isLinear() { return params[LINEAR_PARAM].getValue() > 0.5f; }
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
