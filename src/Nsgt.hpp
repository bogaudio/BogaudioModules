#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/utility.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelNsgt;

namespace bogaudio {

struct Nsgt : BGModule {
	enum ParamsIds {
		THRESHOLD_PARAM,
		RATIO_PARAM,
		KNEE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LEFT_INPUT,
		RIGHT_INPUT,
		THRESHOLD_INPUT,
		RATIO_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		float thresholdDb = 0.0f;
		float ratio = 0.0f;
		float ratioKnob = -1.0f;
		float lastEnv = 0.0f;

		bogaudio::dsp::SlewLimiter attackSL;
		bogaudio::dsp::SlewLimiter releaseSL;
		RootMeanSquare detector;
		NoiseGate noiseGate;
		Amplifier amplifier;
		Saturator saturator;

		void sampleRateChange();
	};

	static constexpr float defaultAttackMs = 150.0f;
	static constexpr float maxAttackMs = 500.0f;
	static constexpr float defaultReleaseMs = 600.0f;
	static constexpr float maxReleaseMs = 2000.0f;

	Engine* _engines[maxChannels] {};
	bool _softKnee = true;
	float _attackMs = defaultAttackMs;
	float _releaseMs = defaultReleaseMs;

	Nsgt() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(THRESHOLD_PARAM, 0.0f, 1.0f, 0.8f, "Threshold", " dB", 0.0f, 30.0f, -24.0f);
		configParam<DynamicsRatioParamQuantity>(RATIO_PARAM, 0.0f, 1.0f, 0.55159f, "Ratio");
		configParam(KNEE_PARAM, 0.0f, 1.0f, 1.0f, "Knee");
	}

	void sampleRateChange() override;
	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
