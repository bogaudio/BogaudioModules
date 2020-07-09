#pragma once

#include "PEQ14_shared.hpp"
#include "dsp/oscillator.hpp"

namespace bogaudio {

struct PEQ14XR : ExpanderModule<PEQ14ExpanderMessage, ExpandableModule<PEQ14ExpanderMessage, PEQXFBase>> {
	enum ParamsIds {
		DAMP_PARAM,
		GAIN_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		DAMP_INPUT,
		GAIN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		struct BandOscillator {
			Phasor _phasor;
			TriangleOscillator _oscillator;
			// SineTableOscillator _oscillator;
			// BandLimitedSawOscillator _oscillator;

			inline void setSampleRate(float sr) { _phasor.setSampleRate(sr); }
			inline void setFrequency(float f) { _phasor.setFrequency(f); }
			inline float next() {
				_phasor.advancePhase();
				return _oscillator.nextFromPhasor(_phasor);
			}
		};

		BandOscillator oscillators[14];
		Amplifier amplifiers[14];
		EnvelopeFollower efs[14];
		float response = -1.0f;
		Amplifier efGain;

		void setSampleRate(float sr);
	};

	Engine* _engines[maxChannels] {};
	Saturator _saturator;
	PEQ14ExpanderMessage* _baseMessage = NULL;

	PEQ14XR() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(DAMP_PARAM, 0.0f, 1.0f, 0.3f, "Envelope follower smoothing", "%", 0.0f, 100.0f);
		configParam<EFGainParamQuantity>(GAIN_PARAM, -1.0f, 1.0f, 0.0f, "Envelope follower gain", " dB");

		setBaseModelPredicate([](Model* m) { return m == modelPEQ14 || m == modelPEQ14XF || m == modelPEQ14XR || m == modelPEQ14XV; });
		setExpanderModelPredicate([](Model* m) { return m == modelPEQ14XF || m == modelPEQ14XR || m == modelPEQ14XV; });
	}

	void sampleRateChange() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
