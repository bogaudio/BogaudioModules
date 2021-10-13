#pragma once

#include "bogaudio.hpp"
#include "Mix4_shared.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

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
		configSwitch(PRE_A1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 A send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(B1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 B send");
		configSwitch(PRE_B1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 B send", {"Post-fader", "Pre-fader"});
		configParam<EQParamQuantity>(LOW2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 low", " dB");
		configParam<EQParamQuantity>(MID2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 mid", " dB");
		configParam<EQParamQuantity>(HIGH2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 high", " dB");
		configParam<AmplifierParamQuantity>(A2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 A send");
		configSwitch(PRE_A2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 A send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(B2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 B send");
		configSwitch(PRE_B2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 B send", {"Post-fader", "Pre-fader"});
		configParam<EQParamQuantity>(LOW3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 low", " dB");
		configParam<EQParamQuantity>(MID3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 mid", " dB");
		configParam<EQParamQuantity>(HIGH3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 high", " dB");
		configParam<AmplifierParamQuantity>(A3_PARAM, 0.0f, 1.0f, 0.0f, "Channel 3 A send");
		configSwitch(PRE_A3_PARAM, 0.0f, 1.0f, 0.0f, "Channel 3 A send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(B3_PARAM, 0.0f, 1.0f, 0.0f, "Channel 3 B send");
		configSwitch(PRE_B3_PARAM, 0.0f, 1.0f, 0.0f, "Channel 3 B send", {"Post-fader", "Pre-fader"});
		configParam<EQParamQuantity>(LOW4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 low", " dB");
		configParam<EQParamQuantity>(MID4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 mid", " dB");
		configParam<EQParamQuantity>(HIGH4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 high", " dB");
		configParam<AmplifierParamQuantity>(A4_PARAM, 0.0f, 1.0f, 0.0f, "Channel 4 A send");
		configSwitch(PRE_A4_PARAM, 0.0f, 1.0f, 0.0f, "Channel 4 A send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(B4_PARAM, 0.0f, 1.0f, 0.0f, "Channel 4 B send");
		configSwitch(PRE_B4_PARAM, 0.0f, 1.0f, 0.0f, "Channel 4 B send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(LEVEL_A_PARAM, 0.0f, 1.0f, 0.8f, "A return level");
		configParam<AmplifierParamQuantity>(LEVEL_B_PARAM, 0.0f, 1.0f, 0.8f, "B return level");

		configInput(A1_INPUT, "Channel 1 send A CV");
		configInput(B1_INPUT, "Channel 1 send B CV");
		configInput(A2_INPUT, "Channel 2 send A CV");
		configInput(B2_INPUT, "Channel 2 send B CV");
		configInput(A3_INPUT, "Channel 3 send A CV");
		configInput(B3_INPUT, "Channel 3 send B CV");
		configInput(A4_INPUT, "Channel 4 send A CV");
		configInput(B4_INPUT, "Channel 4 send B CV");
		configInput(L_A_INPUT, "Return A left");
		configInput(R_A_INPUT, "Return A right");
		configInput(LEVEL_A_INPUT, "Return A level CV");
		configInput(L_B_INPUT, "Return B left");
		configInput(R_B_INPUT, "Return B right");

		configOutput(SEND_A_OUTPUT, "SEND_A");
		configOutput(SEND_B_OUTPUT, "SEND_B");

		_channels[0] = new MixerExpanderChannel(params[LOW1_PARAM], params[MID1_PARAM], params[HIGH1_PARAM], params[A1_PARAM], params[B1_PARAM], params[PRE_A1_PARAM], params[PRE_B1_PARAM], inputs[A1_INPUT], inputs[B1_INPUT]);
		_channels[1] = new MixerExpanderChannel(params[LOW2_PARAM], params[MID2_PARAM], params[HIGH2_PARAM], params[A2_PARAM], params[B2_PARAM], params[PRE_A2_PARAM], params[PRE_B2_PARAM], inputs[A2_INPUT], inputs[B2_INPUT]);
		_channels[2] = new MixerExpanderChannel(params[LOW3_PARAM], params[MID3_PARAM], params[HIGH3_PARAM], params[A3_PARAM], params[B3_PARAM], params[PRE_A3_PARAM], params[PRE_B3_PARAM], inputs[A3_INPUT], inputs[B3_INPUT]);
		_channels[3] = new MixerExpanderChannel(params[LOW4_PARAM], params[MID4_PARAM], params[HIGH4_PARAM], params[A4_PARAM], params[B4_PARAM], params[PRE_A4_PARAM], params[PRE_B4_PARAM], inputs[A4_INPUT], inputs[B4_INPUT]);

		setBaseModelPredicate([](Model* m) { return m == modelMix4; });
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
