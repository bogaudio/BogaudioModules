#pragma once

#include "bogaudio.hpp"
#include "disable_output_limit.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVCM;

namespace bogaudio {

struct VCM : DisableOutputLimitModule {
	enum ParamsIds {
		LEVEL1_PARAM,
		LEVEL2_PARAM,
		LEVEL3_PARAM,
		LEVEL4_PARAM,
		MIX_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN1_INPUT,
		CV1_INPUT,
		IN2_INPUT,
		CV2_INPUT,
		IN3_INPUT,
		CV3_INPUT,
		IN4_INPUT,
		CV4_INPUT,
		MIX_CV_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		MIX_OUTPUT,
		NUM_OUTPUTS
	};

	Amplifier _amplifier1[maxChannels];
	Amplifier _amplifier2[maxChannels];
	Amplifier _amplifier3[maxChannels];
	Amplifier _amplifier4[maxChannels];

	struct LevelParamQuantity : AmplifierParamQuantity {
		bool isLinear() override;
	};

	VCM() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<LevelParamQuantity>(LEVEL1_PARAM, 0.0f, 1.0f, 0.8f, "Level 1");
		configParam<LevelParamQuantity>(LEVEL2_PARAM, 0.0f, 1.0f, 0.8f, "Level 2");
		configParam<LevelParamQuantity>(LEVEL3_PARAM, 0.0f, 1.0f, 0.8f, "Level 3");
		configParam<LevelParamQuantity>(LEVEL4_PARAM, 0.0f, 1.0f, 0.8f, "Level 4");
		configParam<LevelParamQuantity>(MIX_PARAM, 0.0f, 1.0f, 0.8f, "Mix level");
		configParam(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear");
	}

	inline bool isLinear() { return params[LINEAR_PARAM].getValue() > 0.5f; }
	bool active() override;
	int channels() override;
	void processChannel(const ProcessArgs& args, int c) override;
	float channelStep(int c, Input& input, Param& knob, Input& cv, Amplifier& amplifier, bool linear);
};

} // namespace bogaudio
