#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelSlew;

namespace bogaudio {

struct Slew : BGModule {
	enum ParamsIds {
		RISE_PARAM,
		RISE_SHAPE_PARAM,
		FALL_PARAM,
		FALL_SHAPE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		RISE_INPUT,
		FALL_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	float _riseShape = 0.0f;
	float _fallShape = 0.0f;
	bool _rising[maxChannels];
	float _last[maxChannels] {};
	ShapedSlewLimiter _rise[maxChannels];
	ShapedSlewLimiter _fall[maxChannels];

	Slew() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(RISE_PARAM, 0.0f, 1.0f, 0.31623f, "Rise", " s");
		configParam(RISE_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "Rise shape");
		configParam<EnvelopeSegmentParamQuantity>(FALL_PARAM, 0.0f, 1.0f, 0.31623f, "Fall", " s");
		configParam(FALL_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "Fall shape");

		for (int i = 0; i < maxChannels; ++i) {
			_rising[i] = true;
		}
	}

	bool active() override;
	int channels() override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
	float time(Param& param, Input& input, int c);
	float shape(Param& param);
};

} // namespace bogaudio
