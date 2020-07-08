#pragma once

#include "PEQ6_shared.hpp"

namespace bogaudio {

struct PEQ6XF : ExpanderModule<PEQ6ExpanderMessage, PEQXFBase> {
	enum ParamsIds {
		DAMP_PARAM,
		GAIN_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		EF1_OUTPUT,
		EF2_OUTPUT,
		EF3_OUTPUT,
		EF4_OUTPUT,
		EF5_OUTPUT,
		EF6_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		RootMeanSquare efs[6];
	};

	Engine* _engines[maxChannels] {};
	float _response = -1.0f;
	Amplifier _gain;
	Saturator _saturator;
	PEQ6ExpanderMessage* _baseMessage = NULL;

	PEQ6XF() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(DAMP_PARAM, 0.0f, 1.0f, 0.3f, "Envelope follower response", "%", 0.0f, 100.0f);
		configParam<EFGainParamQuantity>(GAIN_PARAM, -1.0f, 1.0f, 0.0f, "Envelope follower gain", " dB");

		setBaseModelPredicate([](Model* m) { return m == modelPEQ6; });
	}

	void sampleRateChange() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
