#pragma once

namespace bogaudio {
namespace dsp {

struct PositiveZeroCrossing {
	const float positiveThreshold = 0.01f;
	const float negativeThreshold = -positiveThreshold;
	const int zeroesForReset = 20;

	enum State {
		NEGATIVE_STATE,
		POSITIVE_STATE,
		COUNT_ZEROES_STATE
	};

	State _state;
	bool _triggerable;
	int _zeroCount = 0;

	PositiveZeroCrossing(bool triggerable = true)
	: _triggerable(triggerable)
	{
		reset();
	}

	bool next(float sample);
	void reset();
};

struct SlewLimiter {
	float _sampleRate;
	float _milliseconds;
	float _samples;
	float _current = 0.0f;
	float _target = 0.0f;
	int _steps = 0;
	float _delta = 0.0f;

	SlewLimiter(float sampleRate = 1000.0f, float milliseconds = 1.0f) {
		setParams(sampleRate, milliseconds);
	}

	void setParams(float sampleRate, float milliseconds);
	float next(float sample);
};

struct CrossFader {
	float _mix = 2.0f;
	float _aMix;
	float _bMix;

	CrossFader() {
		setMix(0.0f);
	}

	void setMix(float mix); // -1 to 1, 0 for equal output of both inputs.
	float next(float a, float b);
};

} // namespace dsp
} // namespace bogaudio
