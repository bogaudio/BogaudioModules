#pragma once

#include "bogaudio.hpp"
#include "dsp/pitch.hpp"

using namespace bogaudio::dsp;

extern Model* modelStack;

namespace bogaudio {

struct Stack : BGModule {
	enum ParamsIds {
		SEMIS_PARAM,
		OCTAVE_PARAM,
		FINE_PARAM,
		QUANTIZE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		THRU_OUTPUT,
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	const float _minCVOut = semitoneToCV(24.0); // C1
	const float _maxCVOut = semitoneToCV(120.0); // C9

	float _semitones[maxChannels] {};
	float _lastSemitones[maxChannels];
	float _lastInCV[maxChannels];
	float _lastFine[maxChannels];
	float _outCV[maxChannels] {};

	Stack() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(SEMIS_PARAM, 0.0f, 11.0f, 0.0f, "Semitones");
		paramQuantities[SEMIS_PARAM]->snapEnabled = true;
		configParam(OCTAVE_PARAM, -3.0f, 3.0f, 0.0f, "Octaves");
		paramQuantities[OCTAVE_PARAM]->snapEnabled = true;
		configParam(FINE_PARAM, -0.99f, 0.99f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);
		configSwitch(QUANTIZE_PARAM, 0.0f, 1.0f, 1.0f, "Quantize", {"Disabled", "Enabled"});

		configInput(CV_INPUT, "CV");
		configInput(IN_INPUT, "IN");

		configOutput(THRU_OUTPUT, "THRU");
		configOutput(OUT_OUTPUT, "OUT");

		for (int i = 0; i < maxChannels; ++i) {
			_lastSemitones[i] = -1000.0f;
			_lastInCV[i] = -1000.0f;
			_lastFine[i] = -1000.0f;
		}
	}

	bool active() override;
	int channels() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void processBypass(const ProcessArgs& args) override;
};

} // namespace bogaudio
