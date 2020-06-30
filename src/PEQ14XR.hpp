#pragma once

#include "PEQ14_shared.hpp"
#include "dsp/oscillator.hpp"

namespace bogaudio {

struct PEQ14XR : ExpanderModule<PEQ14ExpanderMessage, ExpandableModule<PEQ14ExpanderMessage, BGModule>> {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
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

		void setSampleRate(float sr);
	};

	Engine* _engines[maxChannels] {};
	Saturator _saturator;
	PEQ14ExpanderMessage* _baseMessage = NULL;

	PEQ14XR() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		setBaseModelPredicate([](Model* m) { return m == modelPEQ14 || m == modelPEQ14XO || m == modelPEQ14XR || m == modelPEQ14XV; });
		setExpanderModelPredicate([](Model* m) { return m == modelPEQ14XO || m == modelPEQ14XR || m == modelPEQ14XV; });
	}

	void sampleRateChange() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
