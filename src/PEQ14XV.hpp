#pragma once

#include "PEQ14_shared.hpp"

namespace bogaudio {

struct PEQ14XV : ExpanderModule<PEQ14ExpanderMessage, ExpandableModule<PEQ14ExpanderMessage, PEQXFBase>> {
	enum ParamsIds {
		EF_DAMP_PARAM,
		EF_GAIN_PARAM,
		TRANSPOSE_PARAM,
		OUTPUT_GAIN_PARAM,
		BAND14_MIX_PARAM,
		BAND1_ENABLE_PARAM,
		BAND14_ENABLE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		EF_DAMP_INPUT,
		EF_GAIN_INPUT,
		TRANSPOSE_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		ODDS_OUTPUT,
		OUT_OUTPUT,
		EVENS_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		MultimodeFilter* filters[14] {};
		Amplifier amplifiers[14];
		EnvelopeFollower efs[14];
		float response = -1.0f;
		Amplifier efGain;
		float transposeSemitones = 0.0f;

		Engine();
		~Engine();
	};

	static constexpr float maxOutputGain = 24.0f;

	Engine* _engines[maxChannels] {};
	Amplifier _outputGain;
	Amplifier _band14Mix;
	bool _band1Enable = true;
	bool _band14Enable = true;
	Saturator _saturator;
	PEQ14ExpanderMessage* _baseMessage = NULL;

	PEQ14XV() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(EF_DAMP_PARAM, 0.0f, 1.0f, 0.3f, "Envelope follower smoothing", "%", 0.0f, 100.0f);
		configParam<EFGainParamQuantity>(EF_GAIN_PARAM, -1.0f, 1.0f, 0.0f, "Envelope follower gain", " dB");
		configParam(TRANSPOSE_PARAM, -1.0f, 1.0f, 0.0f, "Transpose", " semitones", 0.0f, 24.0f);
		configParam(OUTPUT_GAIN_PARAM, 0.0f, 1.0f, 0.0f, "Output gain", " dB", 0.0f, maxOutputGain);
		configParam<AmplifierParamQuantity>(BAND14_MIX_PARAM, 0.0f, 1.0f, 0.0f, "Band 14 audio mix");
		configParam(BAND1_ENABLE_PARAM, 0.0f, 1.0f, 1.0f, "Band 1 enable");
		configParam(BAND14_ENABLE_PARAM, 0.0f, 1.0f, 1.0f, "Band 14 enable");

		configInput(EF_DAMP_INPUT, "Envelope follower damp CV");
		configInput(EF_GAIN_INPUT, "Envelope follower gain CV");
		configInput(TRANSPOSE_INPUT, "Transpose (1V/octave)");
		configInput(IN_INPUT, "Signal");

		configOutput(ODDS_OUTPUT, "Odd channels mix");
		configOutput(OUT_OUTPUT, "All channels mix");
		configOutput(EVENS_OUTPUT, "Even channels mix");

		setBaseModelPredicate([](Model* m) { return m == modelPEQ14 || m == modelPEQ14XF || m == modelPEQ14XR || m == modelPEQ14XV; });
		setExpanderModelPredicate([](Model* m) { return m == modelPEQ14XF || m == modelPEQ14XR || m == modelPEQ14XV; });
	}

	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
