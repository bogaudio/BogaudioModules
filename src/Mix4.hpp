#pragma once

#include "Mix4_shared.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct Mix4 : ExpandableModule<Mix4ExpanderMessage, DimmableMixerModule> {
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
		MIX_DIM_PARAM,
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
	bool _channelActive[4] {};
	Panner _panners[4];
	bogaudio::dsp::SlewLimiter _panSLs[4];
	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _slewLimiter;
	Saturator _saturator;
	RootMeanSquare _rms;
	float _rmsLevel = 0.0f;
	Mix4ExpanderMessage _dummyExpanderMessage;
	int _wasActive = 0;
	bogaudio::dsp::SlewLimiter _levelCVSL;

	Mix4() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		float levelDefault = fabsf(MixerChannel::minDecibels) / (MixerChannel::maxDecibels - MixerChannel::minDecibels);
		configParam(LEVEL1_PARAM, 0.0f, 1.0f, levelDefault, "Channel 1 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE1_PARAM, 0.0f, 3.0f, 0.0f, "Channel 1 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL2_PARAM, 0.0f, 1.0f, levelDefault, "Channel 2 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE2_PARAM, 0.0f, 3.0f, 0.0f, "Channel 2 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL3_PARAM, 0.0f, 1.0f, levelDefault, "Channel 3 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE3_PARAM, 0.0f, 3.0f, 0.0f, "Channel 3 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(LEVEL4_PARAM, 0.0f, 1.0f, levelDefault, "Channel 4 level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(PAN4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 panning", "%", 0.0f, 100.0f);
		configSwitch(MUTE4_PARAM, 0.0f, 3.0f, 0.0f, "Channel 4 mute", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configParam(MIX_PARAM, 0.0f, 1.0f, levelDefault, "Master level", " dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configSwitch(MIX_MUTE_PARAM, 0.0f, 1.0f, 0.0f, "Master mute", {"Unmuted", "Muted"});
		configSwitch<DimSwitchQuantity>(MIX_DIM_PARAM, 0.0f, 1.0f, 0.0f, "Master dim", {"Disabled", "Enabled"});
		getParamQuantity(MUTE1_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE2_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE3_PARAM)->randomizeEnabled = false;
		getParamQuantity(MUTE4_PARAM)->randomizeEnabled = false;
		getParamQuantity(MIX_MUTE_PARAM)->randomizeEnabled = false;
		getParamQuantity(MIX_DIM_PARAM)->randomizeEnabled = false;

		configInput(CV1_INPUT, "Channel 1 level CV");
		configInput(PAN1_INPUT, "Channel 1 pan CV");
		configInput(IN1_INPUT, "Channel 1");
		configInput(CV2_INPUT, "Channel 2 level CV");
		configInput(PAN2_INPUT, "Channel 2 pan CV");
		configInput(IN2_INPUT, "Channel 2");
		configInput(CV3_INPUT, "Channel 3 level CV");
		configInput(PAN3_INPUT, "Channel 3 pan CV");
		configInput(IN3_INPUT, "Channel 3");
		configInput(CV4_INPUT, "Channel 4 level CV");
		configInput(PAN4_INPUT, "Channel 4 pan CV");
		configInput(IN4_INPUT, "Channel 4");
		configInput(MIX_CV_INPUT, "Mix level CV");

		configOutput(L_OUTPUT, "L");
		configOutput(R_OUTPUT, "R");

		_channels[0] = new MixerChannel(params[LEVEL1_PARAM], params[MUTE1_PARAM], inputs[CV1_INPUT]);
		_channels[1] = new MixerChannel(params[LEVEL2_PARAM], params[MUTE2_PARAM], inputs[CV2_INPUT]);
		_channels[2] = new MixerChannel(params[LEVEL3_PARAM], params[MUTE3_PARAM], inputs[CV3_INPUT]);
		_channels[3] = new MixerChannel(params[LEVEL4_PARAM], params[MUTE4_PARAM], inputs[CV4_INPUT]);

		sampleRateChange();
		_rms.setSensitivity(0.05f);
		setExpanderModelPredicate([](Model* m) { return m == modelMix4x; });
	}
	virtual ~Mix4() {
		for (int i = 0; i < 4; ++i) {
			delete _channels[i];
		}
	}

	void onRandomize(const RandomizeEvent& e) override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
