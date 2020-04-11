#pragma once

#include "bogaudio.hpp"
#include "mixer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMix8;
extern Model* modelMix8x;

namespace bogaudio {

struct Mix8x;

typedef MixerExpanderMessage<8> Mix8ExpanderMessage;

struct Mix8 : ExpandableModule<Mix8ExpanderMessage, BGModule> {
	enum ParamsIds {
		LEVEL1_PARAM,
		MUTE1_PARAM,
		PAN1_PARAM,
		LEVEL2_PARAM,
		MUTE2_PARAM,
		PAN2_PARAM,
		LEVEL3_PARAM,
		MUTE3_PARAM,
		PAN3_PARAM,
		LEVEL4_PARAM,
		MUTE4_PARAM,
		PAN4_PARAM,
		LEVEL5_PARAM,
		MUTE5_PARAM,
		PAN5_PARAM,
		LEVEL6_PARAM,
		MUTE6_PARAM,
		PAN6_PARAM,
		LEVEL7_PARAM,
		MUTE7_PARAM,
		PAN7_PARAM,
		LEVEL8_PARAM,
		MUTE8_PARAM,
		PAN8_PARAM,
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
		CV5_INPUT,
		PAN5_INPUT,
		IN5_INPUT,
		CV6_INPUT,
		PAN6_INPUT,
		IN6_INPUT,
		CV7_INPUT,
		PAN7_INPUT,
		IN7_INPUT,
		CV8_INPUT,
		PAN8_INPUT,
		IN8_INPUT,
		MIX_CV_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		L_OUTPUT,
		R_OUTPUT,
		NUM_OUTPUTS
	};

	int _polyChannelOffset = -1;
	MixerChannel* _channels[8] {};
	Panner _panners[8];
	bogaudio::dsp::SlewLimiter _panSLs[8];
	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _slewLimiter;
	Saturator _saturator;
	RootMeanSquare _rms;
	float _rmsLevel = 0.0f;
	Mix8ExpanderMessage _dummyExpanderMessage;

	Mix8() {
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
		configParam(LEVEL5_PARAM, 0.0f, 1.0f, levelDefault, "Channel 5 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 panning", "%", 0.0f, 100.0f);
		configParam(MUTE5_PARAM, 0.0f, 3.0f, 0.0f, "Channel 5 mute");
		configParam(LEVEL6_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 panning", "%", 0.0f, 100.0f);
		configParam(MUTE6_PARAM, 0.0f, 3.0f, 0.0f, "Channel 6 mute");
		configParam(LEVEL7_PARAM, 0.0f, 1.0f, levelDefault, "Channel 7 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 panning", "%", 0.0f, 100.0f);
		configParam(MUTE7_PARAM, 0.0f, 3.0f, 0.0f, "Channel 7 mute");
		configParam(LEVEL8_PARAM, 0.0f, 1.0f, levelDefault, "Channel 8 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 panning", "%", 0.0f, 100.0f);
		configParam(MUTE8_PARAM, 0.0f, 3.0f, 0.0f, "Channel 8 mute");
		configParam(MIX_PARAM, 0.0f, 1.0f, levelDefault, "Master level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(MIX_MUTE_PARAM, 0.0f, 1.0f, 0.0f, "Master mute");

		_channels[0] = new MixerChannel(params[LEVEL1_PARAM], params[MUTE1_PARAM], inputs[CV1_INPUT]);
		_channels[1] = new MixerChannel(params[LEVEL2_PARAM], params[MUTE2_PARAM], inputs[CV2_INPUT]);
		_channels[2] = new MixerChannel(params[LEVEL3_PARAM], params[MUTE3_PARAM], inputs[CV3_INPUT]);
		_channels[3] = new MixerChannel(params[LEVEL4_PARAM], params[MUTE4_PARAM], inputs[CV4_INPUT]);
		_channels[4] = new MixerChannel(params[LEVEL5_PARAM], params[MUTE5_PARAM], inputs[CV5_INPUT]);
		_channels[5] = new MixerChannel(params[LEVEL6_PARAM], params[MUTE6_PARAM], inputs[CV6_INPUT]);
		_channels[6] = new MixerChannel(params[LEVEL7_PARAM], params[MUTE7_PARAM], inputs[CV7_INPUT]);
		_channels[7] = new MixerChannel(params[LEVEL8_PARAM], params[MUTE8_PARAM], inputs[CV8_INPUT]);

		sampleRateChange();
		_rms.setSensitivity(0.05f);
		setExpanderModel(modelMix8x);
	}
	virtual ~Mix8() {
		for (int i = 0; i < 8; ++i) {
			delete _channels[i];
		}
	}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
};

struct Mix8x : ExpanderModule<Mix8ExpanderMessage, BGModule> {
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
		LOW5_PARAM,
		MID5_PARAM,
		HIGH5_PARAM,
		A5_PARAM,
		PRE_A5_PARAM,
		B5_PARAM,
		PRE_B5_PARAM,
		LOW6_PARAM,
		MID6_PARAM,
		HIGH6_PARAM,
		A6_PARAM,
		PRE_A6_PARAM,
		B6_PARAM,
		PRE_B6_PARAM,
		LOW7_PARAM,
		MID7_PARAM,
		HIGH7_PARAM,
		A7_PARAM,
		PRE_A7_PARAM,
		B7_PARAM,
		PRE_B7_PARAM,
		LOW8_PARAM,
		MID8_PARAM,
		HIGH8_PARAM,
		A8_PARAM,
		PRE_A8_PARAM,
		B8_PARAM,
		PRE_B8_PARAM,
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
		A5_INPUT,
		B5_INPUT,
		A6_INPUT,
		B6_INPUT,
		A7_INPUT,
		B7_INPUT,
		A8_INPUT,
		B8_INPUT,
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

	MixerExpanderChannel* _channels[8] {};
	Saturator _saturatorA, _saturatorB;
	Amplifier _returnAAmp, _returnBAmp;
	bogaudio::dsp::SlewLimiter _returnASL, _returnBSL;

	Mix8x() {
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
		configParam<EQParamQuantity>(LOW5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 low", " dB");
		configParam<EQParamQuantity>(MID5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 mid", " dB");
		configParam<EQParamQuantity>(HIGH5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 high", " dB");
		configParam<AmplifierParamQuantity>(A5_PARAM, 0.0f, 1.0f, 0.0f, "Channel 5 A send");
		configParam(PRE_A5_PARAM, 0.0f, 1.0f, 0.0f, "Channel 5 A send pre/post");
		configParam<AmplifierParamQuantity>(B5_PARAM, 0.0f, 1.0f, 0.0f, "Channel 5 B send");
		configParam(PRE_B5_PARAM, 0.0f, 1.0f, 0.0f, "Channel 5 B send pre/post");
		configParam<EQParamQuantity>(LOW6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 low", " dB");
		configParam<EQParamQuantity>(MID6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 mid", " dB");
		configParam<EQParamQuantity>(HIGH6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 high", " dB");
		configParam<AmplifierParamQuantity>(A6_PARAM, 0.0f, 1.0f, 0.0f, "Channel 6 A send");
		configParam(PRE_A6_PARAM, 0.0f, 1.0f, 0.0f, "Channel 6 A send pre/post");
		configParam<AmplifierParamQuantity>(B6_PARAM, 0.0f, 1.0f, 0.0f, "Channel 6 B send");
		configParam(PRE_B6_PARAM, 0.0f, 1.0f, 0.0f, "Channel 6 B send pre/post");
		configParam<EQParamQuantity>(LOW7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 low", " dB");
		configParam<EQParamQuantity>(MID7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 mid", " dB");
		configParam<EQParamQuantity>(HIGH7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 high", " dB");
		configParam<AmplifierParamQuantity>(A7_PARAM, 0.0f, 1.0f, 0.0f, "Channel 7 A send");
		configParam(PRE_A7_PARAM, 0.0f, 1.0f, 0.0f, "Channel 7 A send pre/post");
		configParam<AmplifierParamQuantity>(B7_PARAM, 0.0f, 1.0f, 0.0f, "Channel 7 B send");
		configParam(PRE_B7_PARAM, 0.0f, 1.0f, 0.0f, "Channel 7 B send pre/post");
		configParam<EQParamQuantity>(LOW8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 low", " dB");
		configParam<EQParamQuantity>(MID8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 mid", " dB");
		configParam<EQParamQuantity>(HIGH8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 high", " dB");
		configParam<AmplifierParamQuantity>(A8_PARAM, 0.0f, 1.0f, 0.0f, "Channel 8 A send");
		configParam(PRE_A8_PARAM, 0.0f, 1.0f, 0.0f, "Channel 8 A send pre/post");
		configParam<AmplifierParamQuantity>(B8_PARAM, 0.0f, 1.0f, 0.0f, "Channel 8 B send");
		configParam(PRE_B8_PARAM, 0.0f, 1.0f, 0.0f, "Channel 8 B send pre/post");
		configParam<AmplifierParamQuantity>(LEVEL_A_PARAM, 0.0f, 1.0f, 0.8f, "A return level");
		configParam<AmplifierParamQuantity>(LEVEL_B_PARAM, 0.0f, 1.0f, 0.8f, "B return level");

		_channels[0] = new MixerExpanderChannel(params[LOW1_PARAM], params[MID1_PARAM], params[HIGH1_PARAM], params[A1_PARAM], params[B1_PARAM], params[PRE_A1_PARAM], params[PRE_B1_PARAM], inputs[A1_INPUT], inputs[B1_INPUT]);
		_channels[1] = new MixerExpanderChannel(params[LOW2_PARAM], params[MID2_PARAM], params[HIGH2_PARAM], params[A2_PARAM], params[B2_PARAM], params[PRE_A2_PARAM], params[PRE_B2_PARAM], inputs[A2_INPUT], inputs[B2_INPUT]);
		_channels[2] = new MixerExpanderChannel(params[LOW3_PARAM], params[MID3_PARAM], params[HIGH3_PARAM], params[A3_PARAM], params[B3_PARAM], params[PRE_A3_PARAM], params[PRE_B3_PARAM], inputs[A3_INPUT], inputs[B3_INPUT]);
		_channels[3] = new MixerExpanderChannel(params[LOW4_PARAM], params[MID4_PARAM], params[HIGH4_PARAM], params[A4_PARAM], params[B4_PARAM], params[PRE_A4_PARAM], params[PRE_B4_PARAM], inputs[A4_INPUT], inputs[B4_INPUT]);
		_channels[4] = new MixerExpanderChannel(params[LOW5_PARAM], params[MID5_PARAM], params[HIGH5_PARAM], params[A5_PARAM], params[B5_PARAM], params[PRE_A5_PARAM], params[PRE_B5_PARAM], inputs[A5_INPUT], inputs[B5_INPUT]);
		_channels[5] = new MixerExpanderChannel(params[LOW6_PARAM], params[MID6_PARAM], params[HIGH6_PARAM], params[A6_PARAM], params[B6_PARAM], params[PRE_A6_PARAM], params[PRE_B6_PARAM], inputs[A6_INPUT], inputs[B6_INPUT]);
		_channels[6] = new MixerExpanderChannel(params[LOW7_PARAM], params[MID7_PARAM], params[HIGH7_PARAM], params[A7_PARAM], params[B7_PARAM], params[PRE_A7_PARAM], params[PRE_B7_PARAM], inputs[A7_INPUT], inputs[B7_INPUT]);
		_channels[7] = new MixerExpanderChannel(params[LOW8_PARAM], params[MID8_PARAM], params[HIGH8_PARAM], params[A8_PARAM], params[B8_PARAM], params[PRE_A8_PARAM], params[PRE_B8_PARAM], inputs[A8_INPUT], inputs[B8_INPUT]);

		setBaseModel(modelMix8);
	}
	virtual ~Mix8x() {
		for (int i = 0; i < 8; ++i) {
			delete _channels[i];
		}
	}

	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
