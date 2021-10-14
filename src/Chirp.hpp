#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

extern Model* modelChirp;

using namespace bogaudio::dsp;

namespace bogaudio {

struct Chirp : BGModule {
	enum ParamsIds {
		TIME_PARAM,
		FREQUENCY1_PARAM,
		FREQUENCY2_PARAM,
		TRIGGER_PARAM,
		EXPONENTIAL_PARAM,
		LOOP_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		FREQUENCY1_INPUT,
		FREQUENCY2_INPUT,
		TIME_INPUT,
		TRIGGER_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		EOC_OUTPUT,
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	static constexpr float minTimeSeconds = 0.05f;
	static constexpr float maxTimeSeconds = 10.0f;
	static constexpr float minFrequency = 1.0f;
	static constexpr float maxFrequencyNyquistRatio = 0.49f;
	static constexpr float cycleSlewSeconds = 0.01f;

	struct TimeParamQuantity : ParamQuantity {
		float getDisplayValue() override {
			float v = getValue();
			if (!module) {
				return v;
			}

			float vv = v * v;
			vv *= Chirp::maxTimeSeconds - Chirp::minTimeSeconds;
			vv += Chirp::minTimeSeconds;
			return vv;
		}

		void setDisplayValue(float displayValue) override {
			if (!module) {
				return;
			}
			displayValue -= Chirp::minTimeSeconds;
			displayValue = std::max(0.0f, displayValue);
			float v = displayValue / (Chirp::maxTimeSeconds - Chirp::minTimeSeconds);
			v = powf(v, 0.5f);
			setValue(v);
		}
	};

	struct Engine {
		ChirpOscillator chirp;
		Trigger trigger;
		rack::dsp::PulseGenerator eocPulseGen;
		Amplifier amp;
		bogaudio::dsp::SlewLimiter ampSL;
		float targetAmplitude = 0.0f;

		void reset();
		void sampleRateChange(float sr);
	};

	Engine* _engines[maxChannels] {};
	float _sampleTime;
	bool _run = false;
	bool _exponential = false;
	bool _loop = false;

	Chirp() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<TimeParamQuantity>(TIME_PARAM, 0.0f, 1.0f, 0.30899415, "Time", "s");
		configParam<FrequencyParamQuantity>(FREQUENCY1_PARAM, -3.0f, 6.0f, -3.0f, "Frequency 1", " Hz");
		configParam<FrequencyParamQuantity>(FREQUENCY2_PARAM, -3.0f, 6.0f, 6.0f, "Frequency 2", " Hz");
		configButton(TRIGGER_PARAM, "Trigger");
		configButton(EXPONENTIAL_PARAM, "Exponential");
		configButton(LOOP_PARAM, "Loop");

		configInput(FREQUENCY1_INPUT, "Frequency 1 (1V/octave)");
		configInput(FREQUENCY2_INPUT, "Frequency 2 (1V/octave)");
		configInput(TIME_INPUT, "Time CV");
		configInput(TRIGGER_INPUT, "Trigger");

		configOutput(EOC_OUTPUT, "End-of-cycle trigger");
		configOutput(OUT_OUTPUT, "Signal");
	}

	void reset() override;
	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
