#pragma once

#include <math.h>

#include "table.hpp"

namespace bogaudio {
namespace dsp {

// "amplitude" is 0-whatever here, with 1 (=0db) meaning unity gain.
inline float decibelsToAmplitude(float db) {
	return powf(10.0f, db * 0.05f);
}

inline float amplitudeToDecibels(float amplitude) {
	return 20.0f * log10f(amplitude);
}

struct Amplifier {
	static const float minDecibels;
	static const float maxDecibels;
	static const float decibelsRange;
	struct LevelTable : Table {
		LevelTable(int n) : Table(n) {}
		virtual void _generate() override;
	};
	struct StaticLevelTable : StaticTable<LevelTable, 11> {};

	float _db = 0.0f;
	float _level;
	const Table& _table;

	Amplifier() : _table(StaticLevelTable::table())	{
		setLevel(minDecibels);
	}

	void setLevel(float db);
	float next(float s);
};

struct RootMeanSquare {
	const float maxDelayMS = 300.0;
	const float minDelayMS = 0.2;
	float _sampleRate = -1.0f;
	float _sensitivity = -1.0f;

	bool _initialized = false;
	float* _buffer = NULL;
	int _bufferN = 0;
	int _sumN = 0;
	int _leadI = 0;
	int _trailI = 0;
	double _sum = 0;

	RootMeanSquare(float sampleRate = 1000.0f, float sensitivity = 1.0f) {
		setSampleRate(sampleRate);
		setSensitivity(sensitivity);
	}
	~RootMeanSquare() {
		if (_buffer) {
			delete[] _buffer;
		}
	}

	void setSampleRate(float sampleRate);
	void setSensitivity(float sensitivity);
	float next(float sample);
};

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
