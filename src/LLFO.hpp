#pragma once

#include "bogaudio.hpp"
#include "lfo_base.hpp"

using namespace bogaudio::dsp;

extern Model* modelLLFO;

namespace bogaudio {

struct LLFO : LFOBase {
	enum ParamsIds {
		FREQUENCY_PARAM,
		SLOW_PARAM,
		WAVE_PARAM,
		OFFSET_PARAM,
		SCALE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		PITCH_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		SINE_LIGHT,
		RAMP_UP_LIGHT,
		SQUARE_LIGHT,
		TRIANGLE_LIGHT,
		RAMP_DOWN_LIGHT,
		PULSE_LIGHT,
		STEPPED_LIGHT,
		NUM_LIGHTS
	};

	enum Wave {
		SINE_WAVE,
		TRIANGLE_WAVE,
		RAMP_UP_WAVE,
		RAMP_DOWN_WAVE,
		SQUARE_WAVE,
		PULSE_WAVE,
		STEPPED_WAVE
	};

	static constexpr float amplitude = 5.0f;
	static constexpr float defaultSample = 0.0f;
	static constexpr float defaultPulseWidth = -0.8510638; // 10% pulse.
	static constexpr float defaultSmooth = 0.0f;
	float _offset = 0.0f;
	float _scale = 0.0f;
	float _sample = defaultSample;
	float _pulseWidth = defaultPulseWidth;
	float _smooth = defaultSmooth;

	PositiveZeroCrossing _resetTrigger[maxChannels];
	Phasor _phasor[maxChannels];
	int _sampleSteps[maxChannels] {};
	int _sampleStep[maxChannels] {};
	float _currentSample[maxChannels] {};
	Smoother _smoother[maxChannels];

	SineTableOscillator _sine;
	TriangleOscillator _triangle;
	SawOscillator _ramp;
	SquareOscillator _square;
	SteppedRandomOscillator _stepped;
	bool _invert;
	Phasor* _oscillator;
	bool _samplingEnabled = false;

	LLFO()
	: LFOBase(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	, _invert(false)
	, _oscillator(&_sine)
	{
		configParam<LFOFrequencyParamQuantity>(FREQUENCY_PARAM, -5.0f, 8.0f, 0.0f, "Frequency", " Hz");
		configParam(WAVE_PARAM, 0.0f, 6.0f, 0.0f, "Waveform");
		configParam(SLOW_PARAM, 0.0f, 1.0f, 0.0f, "Slow mode");
		configParam(OFFSET_PARAM, -1.0f, 1.0f, 0.0f, "Offset", " V", 0.0f, 5.0f);
		configParam(SCALE_PARAM, 0.0f, 1.0f, 1.0f, "Scale", "%", 0.0f, 100.0f);
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
