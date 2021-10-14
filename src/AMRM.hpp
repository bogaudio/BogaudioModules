#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelAMRM;

namespace bogaudio {

struct AMRM : BGModule {
	enum ParamsIds {
		RECTIFY_PARAM,
		DRYWET_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		MODULATOR_INPUT,
		CARRIER_INPUT,
		RECTIFY_INPUT,
		DRYWET_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		RECTIFY_OUTPUT,
		NUM_OUTPUTS
	};

	Saturator _saturator;
	int _polyInputID = CARRIER_INPUT;

	AMRM() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(RECTIFY_PARAM, 0.0f, 1.0f, 0.0f, "Rectification", "%", 0.0f, 100.0f);
		configParam(DRYWET_PARAM, 0.0f, 1.0f, 1.0f, "Wet mix", "%", 0.0f, 100.0f);
		configBypass(CARRIER_INPUT, OUT_OUTPUT);

		configInput(MODULATOR_INPUT, "Modulator signal");
		configInput(CARRIER_INPUT, "Carrier signal");
		configInput(RECTIFY_INPUT, "Rectify CV");
		configInput(DRYWET_INPUT, "Dry/wet CV");

		configOutput(OUT_OUTPUT, "Modulated carrier signal");
		configOutput(RECTIFY_OUTPUT, "Rectified modulator signal");
	}

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
