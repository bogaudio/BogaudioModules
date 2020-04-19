#pragma once

#include "bogaudio.hpp"
#include "mixer_expander.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMix4;
extern Model* modelMix4x;

namespace bogaudio {

struct Mix4x;

typedef MixerExpanderMessage<4> Mix4ExpanderMessage;

struct Mix4 : ExpandableModule<Mix4ExpanderMessage, BGModule> {
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

	int _polyChannelOffset = -1;
	MixerChannel* _channels[4] {};
	Panner _panners[4];
	bogaudio::dsp::SlewLimiter _panSLs[4];
	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _slewLimiter;
	Saturator _saturator;
	RootMeanSquare _rms;
	float _rmsLevel = 0.0f;
	Mix4ExpanderMessage _dummyExpanderMessage;
	int _wasActive = 0;

	Mix4() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		float levelDefault = fabsf(MixerChannel::minDecibels) / (MixerChannel::maxDecibels - MixerChannel::minDecibels);
		configParam(LEVEL1_PARAM, 0.0f, 1.0f, levelDefault, "Channel 1 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1 panning", "%", 0.0f, 100.0f);
		configParam(MUTE1_PARAM, 0.0f, 3.0f, 0.0f, "Channel 1 mute");
		configParam(LEVEL2_PARAM, 0.0f, 1.0f, levelDefault, "Channel 2 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 panning", "%", 0.0f, 100.0f);
		configParam(MUTE2_PARAM, 0.0f, 3.0f, 0.0f, "Channel 2 mute");
		configParam(LEVEL3_PARAM, 0.0f, 1.0f, levelDefault, "Channel 3 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 panning", "%", 0.0f, 100.0f);
		configParam(MUTE3_PARAM, 0.0f, 3.0f, 0.0f, "Channel 3 mute");
		configParam(LEVEL4_PARAM, 0.0f, 1.0f, levelDefault, "Channel 4 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 panning", "%", 0.0f, 100.0f);
		configParam(MUTE4_PARAM, 0.0f, 3.0f, 0.0f, "Channel 4 mute");
		configParam(MIX_PARAM, 0.0f, 1.0f, levelDefault, "Master level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(MIX_MUTE_PARAM, 0.0f, 1.0f, 0.0f, "Master mute");

		_channels[0] = new MixerChannel(params[LEVEL1_PARAM], params[MUTE1_PARAM], inputs[CV1_INPUT]);
		_channels[1] = new MixerChannel(params[LEVEL2_PARAM], params[MUTE2_PARAM], inputs[CV2_INPUT]);
		_channels[2] = new MixerChannel(params[LEVEL3_PARAM], params[MUTE3_PARAM], inputs[CV3_INPUT]);
		_channels[3] = new MixerChannel(params[LEVEL4_PARAM], params[MUTE4_PARAM], inputs[CV4_INPUT]);

		sampleRateChange();
		_rms.setSensitivity(0.05f);
		setExpanderModel(modelMix4x);
	}
	virtual ~Mix4() {
		for (int i = 0; i < 4; ++i) {
			delete _channels[i];
		}
	}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
};

struct Mix4x : ExpanderModule<Mix4ExpanderMessage, BGModule> {
	enum ParamsIds {
		LOW1_PARAM,
		MID1_PARAM,
		HIGH1_PARAM,
		A1_PARAM,
		PRE_A1_PARAM,
		B1_PARAM,
		PRE_B1_PARAM,
		LOW2_PARAM,
		MID2_PARAM,
		HIGH2_PARAM,
		A2_PARAM,
		PRE_A2_PARAM,
		B2_PARAM,
		PRE_B2_PARAM,
		LOW3_PARAM,
		MID3_PARAM,
		HIGH3_PARAM,
		A3_PARAM,
		PRE_A3_PARAM,
		B3_PARAM,
		PRE_B3_PARAM,
		LOW4_PARAM,
		MID4_PARAM,
		HIGH4_PARAM,
		A4_PARAM,
		PRE_A4_PARAM,
		B4_PARAM,
		PRE_B4_PARAM,
		LEVEL_A_PARAM,
		LEVEL_B_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		A1_INPUT,
		B1_INPUT,
		A2_INPUT,
		B2_INPUT,
		A3_INPUT,
		B3_INPUT,
		A4_INPUT,
		B4_INPUT,
		L_A_INPUT,
		R_A_INPUT,
		LEVEL_A_INPUT,
		L_B_INPUT,
		R_B_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		SEND_A_OUTPUT,
		SEND_B_OUTPUT,
		NUM_OUTPUTS
	};

	MixerExpanderChannel* _channels[4] {};
	Saturator _saturatorA, _saturatorB;
	Amplifier _returnAAmp, _returnBAmp;
	bogaudio::dsp::SlewLimiter _returnASL, _returnBSL;

	Mix4x() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<EQParamQuantity>(LOW1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1 low", " dB");
		configParam<EQParamQuantity>(MID1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1 mid", " dB");
		configParam<EQParamQuantity>(HIGH1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1 high", " dB");
		configParam<AmplifierParamQuantity>(A1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 A send");
		configParam(PRE_A1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 A send pre/post");
		configParam<AmplifierParamQuantity>(B1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 B send");
		configParam(PRE_B1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 B send pre/post");
		configParam<EQParamQuantity>(LOW2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 low", " dB");
		configParam<EQParamQuantity>(MID2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 mid", " dB");
		configParam<EQParamQuantity>(HIGH2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 high", " dB");
		configParam<AmplifierParamQuantity>(A2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 A send");
		configParam(PRE_A2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 A send pre/post");
		configParam<AmplifierParamQuantity>(B2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 B send");
		configParam(PRE_B2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 B send pre/post");
		configParam<EQParamQuantity>(LOW3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 low", " dB");
		configParam<EQParamQuantity>(MID3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 mid", " dB");
		configParam<EQParamQuantity>(HIGH3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 high", " dB");
		configParam<AmplifierParamQuantity>(A3_PARAM, 0.0f, 1.0f, 0.0f, "Channel 3 A send");
		configParam(PRE_A3_PARAM, 0.0f, 1.0f, 0.0f, "Channel 3 A send pre/post");
		configParam<AmplifierParamQuantity>(B3_PARAM, 0.0f, 1.0f, 0.0f, "Channel 3 B send");
		configParam(PRE_B3_PARAM, 0.0f, 1.0f, 0.0f, "Channel 3 B send pre/post");
		configParam<EQParamQuantity>(LOW4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 low", " dB");
		configParam<EQParamQuantity>(MID4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 mid", " dB");
		configParam<EQParamQuantity>(HIGH4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 high", " dB");
		configParam<AmplifierParamQuantity>(A4_PARAM, 0.0f, 1.0f, 0.0f, "Channel 4 A send");
		configParam(PRE_A4_PARAM, 0.0f, 1.0f, 0.0f, "Channel 4 A send pre/post");
		configParam<AmplifierParamQuantity>(B4_PARAM, 0.0f, 1.0f, 0.0f, "Channel 4 B send");
		configParam(PRE_B4_PARAM, 0.0f, 1.0f, 0.0f, "Channel 4 B send pre/post");
		configParam<AmplifierParamQuantity>(LEVEL_A_PARAM, 0.0f, 1.0f, 0.8f, "A return level");
		configParam<AmplifierParamQuantity>(LEVEL_B_PARAM, 0.0f, 1.0f, 0.8f, "B return level");

		_channels[0] = new MixerExpanderChannel(params[LOW1_PARAM], params[MID1_PARAM], params[HIGH1_PARAM], params[A1_PARAM], params[B1_PARAM], params[PRE_A1_PARAM], params[PRE_B1_PARAM], inputs[A1_INPUT], inputs[B1_INPUT]);
		_channels[1] = new MixerExpanderChannel(params[LOW2_PARAM], params[MID2_PARAM], params[HIGH2_PARAM], params[A2_PARAM], params[B2_PARAM], params[PRE_A2_PARAM], params[PRE_B2_PARAM], inputs[A2_INPUT], inputs[B2_INPUT]);
		_channels[2] = new MixerExpanderChannel(params[LOW3_PARAM], params[MID3_PARAM], params[HIGH3_PARAM], params[A3_PARAM], params[B3_PARAM], params[PRE_A3_PARAM], params[PRE_B3_PARAM], inputs[A3_INPUT], inputs[B3_INPUT]);
		_channels[3] = new MixerExpanderChannel(params[LOW4_PARAM], params[MID4_PARAM], params[HIGH4_PARAM], params[A4_PARAM], params[B4_PARAM], params[PRE_A4_PARAM], params[PRE_B4_PARAM], inputs[A4_INPUT], inputs[B4_INPUT]);

		setBaseModel(modelMix4);
	}
	virtual ~Mix4x() {
		for (int i = 0; i < 4; ++i) {
			delete _channels[i];
		}
	}

	void sampleRateChange() override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
