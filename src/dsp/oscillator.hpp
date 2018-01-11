#pragma once

#include "base.hpp"

namespace bogaudio {
namespace dsp {

struct OscillatorGenerator : Generator {
	float _sampleRate;
	float _frequency;

	OscillatorGenerator(
		float sampleRate,
		float frequency
	)
	: _sampleRate(sampleRate)
	, _frequency(frequency)
	{
	}

	void setSampleRate(float sampleRate) {
		if (_sampleRate != sampleRate) {
			_sampleRate = sampleRate;
			_sampleRateChanged();
		}
	}

	virtual void _sampleRateChanged() {}

	void setFrequency(float frequency) {
		if (_frequency != frequency) {
			_frequency = frequency;
			_frequencyChanged();
		}
	}

	virtual void _frequencyChanged() {}
};

struct SineOscillator : OscillatorGenerator {
	static const int _n = 4;
	int _step = 0;
	float _x[_n];
	float _y[_n];
	float _sinDeltaTheta[_n];
	float _cosDeltaTheta[_n];

	SineOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0
	)
	: OscillatorGenerator(sampleRate, frequency)
	{
		for (int i = 0; i < _n; ++i) {
			_x[i] = amplitude;
			_y[i] = 0.0;
		}
		updateDeltaTheta();
	}

	virtual void _sampleRateChanged() override {
		updateDeltaTheta();
	}

	virtual void _frequencyChanged() override {
		updateDeltaTheta();
	}

	void updateDeltaTheta();
	virtual float _next() override;
};

} // namespace dsp
} // namespace bogaudio
