#pragma once

#include "bogaudio.hpp"
#include "Mix8_shared.hpp"

using namespace bogaudio::dsp;
namespace bogaudio {

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
		configParam<EQParamQuantity>(LOW5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 low", " dB");
		configParam<EQParamQuantity>(MID5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 mid", " dB");
		configParam<EQParamQuantity>(HIGH5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 high", " dB");
		configParam<AmplifierParamQuantity>(A5_PARAM, 0.0f, 1.0f, 0.0f, "Channel 5 A send");
		configSwitch(PRE_A5_PARAM, 0.0f, 1.0f, 0.0f, "Channel 5 A send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(B5_PARAM, 0.0f, 1.0f, 0.0f, "Channel 5 B send");
		configSwitch(PRE_B5_PARAM, 0.0f, 1.0f, 0.0f, "Channel 5 B send", {"Post-fader", "Pre-fader"});
		configParam<EQParamQuantity>(LOW6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 low", " dB");
		configParam<EQParamQuantity>(MID6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 mid", " dB");
		configParam<EQParamQuantity>(HIGH6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 high", " dB");
		configParam<AmplifierParamQuantity>(A6_PARAM, 0.0f, 1.0f, 0.0f, "Channel 6 A send");
		configSwitch(PRE_A6_PARAM, 0.0f, 1.0f, 0.0f, "Channel 6 A send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(B6_PARAM, 0.0f, 1.0f, 0.0f, "Channel 6 B send");
		configSwitch(PRE_B6_PARAM, 0.0f, 1.0f, 0.0f, "Channel 6 B send", {"Post-fader", "Pre-fader"});
		configParam<EQParamQuantity>(LOW7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 low", " dB");
		configParam<EQParamQuantity>(MID7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 mid", " dB");
		configParam<EQParamQuantity>(HIGH7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 high", " dB");
		configParam<AmplifierParamQuantity>(A7_PARAM, 0.0f, 1.0f, 0.0f, "Channel 7 A send");
		configSwitch(PRE_A7_PARAM, 0.0f, 1.0f, 0.0f, "Channel 7 A send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(B7_PARAM, 0.0f, 1.0f, 0.0f, "Channel 7 B send");
		configSwitch(PRE_B7_PARAM, 0.0f, 1.0f, 0.0f, "Channel 7 B send", {"Post-fader", "Pre-fader"});
		configParam<EQParamQuantity>(LOW8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 low", " dB");
		configParam<EQParamQuantity>(MID8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 mid", " dB");
		configParam<EQParamQuantity>(HIGH8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 high", " dB");
		configParam<AmplifierParamQuantity>(A8_PARAM, 0.0f, 1.0f, 0.0f, "Channel 8 A send");
		configSwitch(PRE_A8_PARAM, 0.0f, 1.0f, 0.0f, "Channel 8 A send", {"Post-fader", "Pre-fader"});
		configParam<AmplifierParamQuantity>(B8_PARAM, 0.0f, 1.0f, 0.0f, "Channel 8 B send");
		configSwitch(PRE_B8_PARAM, 0.0f, 1.0f, 0.0f, "Channel 8 B send", {"Post-fader", "Pre-fader"});
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
		configInput(A5_INPUT, "Channel 5 send A CV");
		configInput(B5_INPUT, "Channel 5 send B CV");
		configInput(A6_INPUT, "Channel 6 send A CV");
		configInput(B6_INPUT, "Channel 6 send B CV");
		configInput(A7_INPUT, "Channel 7 send A CV");
		configInput(B7_INPUT, "Channel 7 send B CV");
		configInput(A8_INPUT, "Channel 8 send A CV");
		configInput(B8_INPUT, "Channel 8 send B CV");
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
		_channels[4] = new MixerExpanderChannel(params[LOW5_PARAM], params[MID5_PARAM], params[HIGH5_PARAM], params[A5_PARAM], params[B5_PARAM], params[PRE_A5_PARAM], params[PRE_B5_PARAM], inputs[A5_INPUT], inputs[B5_INPUT]);
		_channels[5] = new MixerExpanderChannel(params[LOW6_PARAM], params[MID6_PARAM], params[HIGH6_PARAM], params[A6_PARAM], params[B6_PARAM], params[PRE_A6_PARAM], params[PRE_B6_PARAM], inputs[A6_INPUT], inputs[B6_INPUT]);
		_channels[6] = new MixerExpanderChannel(params[LOW7_PARAM], params[MID7_PARAM], params[HIGH7_PARAM], params[A7_PARAM], params[B7_PARAM], params[PRE_A7_PARAM], params[PRE_B7_PARAM], inputs[A7_INPUT], inputs[B7_INPUT]);
		_channels[7] = new MixerExpanderChannel(params[LOW8_PARAM], params[MID8_PARAM], params[HIGH8_PARAM], params[A8_PARAM], params[B8_PARAM], params[PRE_A8_PARAM], params[PRE_B8_PARAM], inputs[A8_INPUT], inputs[B8_INPUT]);

		setBaseModelPredicate([](Model* m) { return m == modelMix8; });
	}
	virtual ~Mix8x() {
		for (int i = 0; i < 8; ++i) {
			delete _channels[i];
		}
	}

	void sampleRateChange() override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
