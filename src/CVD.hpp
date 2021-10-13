#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelCVD;

namespace bogaudio {

struct CVD : BGModule {
	enum ParamsIds {
		TIME_PARAM,
		TIME_SCALE_PARAM,
		MIX_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TIME_INPUT,
		MIX_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		DelayLine delay;
		CrossFader mix;

		Engine() : delay(1000.0f, 10000.0f) {}

		void sampleRateChange();
	};
	Engine* _engines[maxChannels] {};

	CVD() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(TIME_PARAM, 0.0f, 1.0f, 0.5f, "Time base", " s");
		configParam(TIME_SCALE_PARAM, 0.0f, 2.0f, 1.0f, "Time scale", "", 10.0f, 0.1f);
		paramQuantities[TIME_SCALE_PARAM]->snapEnabled = true;
		configParam(MIX_PARAM, -1.0f, 1.0f, 0.0f, "Dry wet mix", "%", 0.0f, 100.0f);
		configBypass(IN_INPUT, OUT_OUTPUT);

		configInput(TIME_INPUT, "Time CV");
		configInput(MIX_INPUT, "Dry/wet CV");
		configInput(IN_INPUT, "Signal");

		configOutput(OUT_OUTPUT, "OUT");
	}

	void sampleRateChange() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
