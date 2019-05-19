#pragma once

#include "bogaudio.hpp"
#include "mixer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMix4;

namespace bogaudio {

struct Mix4 : Module {
	enum ParamsIds {
		LEVEL1_PARAM,
		PAN1_PARAM,
		MUTE1_PARAM,
		LEVEL2_PARAM,
		PAN2_PARAM,
		MUTE2_PARAM,
		LEVEL3_PARAM,
		PAN3_PARAM,
		MUTE3_PARAM,
		LEVEL4_PARAM,
		PAN4_PARAM,
		MUTE4_PARAM,
		MIX_PARAM,
		MIX_MUTE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		PAN1_INPUT,
		IN1_INPUT,
		CV2_INPUT,
		PAN2_INPUT,
		IN2_INPUT,
		CV3_INPUT,
		PAN3_INPUT,
		IN3_INPUT,
		CV4_INPUT,
		PAN4_INPUT,
		IN4_INPUT,
		MIX_CV_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		L_OUTPUT,
		R_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	MixerChannel _channel1;
	MixerChannel _channel2;
	MixerChannel _channel3;
	MixerChannel _channel4;
	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _slewLimiter;
	Saturator _saturator;
	RootMeanSquare _rms;
	float _rmsLevel = 0.0f;

	Mix4()
	: _channel1(params[LEVEL1_PARAM], params[PAN1_PARAM], params[MUTE1_PARAM], inputs[IN1_INPUT], inputs[CV1_INPUT], inputs[PAN1_INPUT])
	, _channel2(params[LEVEL2_PARAM], params[PAN2_PARAM], params[MUTE2_PARAM], inputs[IN2_INPUT], inputs[CV2_INPUT], inputs[PAN2_INPUT])
	, _channel3(params[LEVEL3_PARAM], params[PAN3_PARAM], params[MUTE3_PARAM], inputs[IN3_INPUT], inputs[CV3_INPUT], inputs[PAN3_INPUT])
	, _channel4(params[LEVEL4_PARAM], params[PAN4_PARAM], params[MUTE4_PARAM], inputs[IN4_INPUT], inputs[CV4_INPUT], inputs[PAN4_INPUT])
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		float levelDefault = fabsf(MixerChannel::minDecibels) / (MixerChannel::maxDecibels - MixerChannel::minDecibels));
		configParam(LEVEL1_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN1_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE1_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL2_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN2_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE2_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL3_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN3_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE3_PARAM, 0.0f, 3.0f, 0.0);
		configParam(LEVEL4_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(PAN4_PARAM, -1.0f, 1.0f, 0.0);
		configParam(MUTE4_PARAM, 0.0f, 3.0f, 0.0);
		configParam(MIX_PARAM, 0.0f, 1.0f, levelDefault);
		configParam(MIX_MUTE_PARAM, 0.0f, 3.0f, 0.0);

		onSampleRateChange();
		_rms.setSensitivity(0.05f);
	}

	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
