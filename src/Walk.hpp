#pragma once

#include "bogaudio.hpp"
#include "dsp/noise.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelWalk;

namespace bogaudio {

struct Walk : BGModule {
	enum ParamsIds {
		RATE_PARAM,
		OFFSET_PARAM,
		SCALE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		RATE_INPUT,
		OFFSET_INPUT,
		SCALE_INPUT,
		JUMP_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum JumpMode {
		JUMP_JUMPMODE,
		TRACKHOLD_JUMPMODE,
		SAMPLEHOLD_JUMPMODE
	};

	float _offset[maxChannels] {};
	float _scale[maxChannels] {};
	Trigger _jumpTrigger[maxChannels];
	RandomWalk _walk[maxChannels];
	bogaudio::dsp::SlewLimiter _slew[maxChannels];
	float _lastOut[maxChannels] {};
	int _polyInputID = RATE_INPUT;
	JumpMode _jumpMode = JUMP_JUMPMODE;

	Walk() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(RATE_PARAM, 0.0f, 1.0f, 0.1f, "Rate", "%", 0.0f, 100.0f);
		configParam(OFFSET_PARAM, -1.0f, 1.0f, 0.0f, "Offset", " V", 0.0f, 5.0f);
		configParam(SCALE_PARAM, 0.0f, 1.0f, 1.0f, "Scale", "%", 0.0f, 100.0f);

		configInput(RATE_INPUT, "Rate CV");
		configInput(OFFSET_INPUT, "Offset CV");
		configInput(SCALE_INPUT, "Scale CV");
		configInput(JUMP_INPUT, "Trigger");

		configOutput(OUT_OUTPUT, "OUT");
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	int channels() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
