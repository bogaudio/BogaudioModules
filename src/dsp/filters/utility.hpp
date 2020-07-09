#pragma once

#include "filters/filter.hpp"
#include "signal.hpp"

namespace bogaudio {
namespace dsp {

struct DCBlocker : Filter {
	float _lastIn = 0.0f;
	float _lastOut = 0.0f;

	float next(float sample) override;
};

struct AverageRectifiedValue : RunningAverage {
	AverageRectifiedValue(float sampleRate = 1000.0f, float sensitivity = 1.0f, float maxDelayMS = 300.0f)
	: RunningAverage(sampleRate, sensitivity, maxDelayMS)
	{
	}

	float next(float sample) override;
};

struct FastRootMeanSquare : AverageRectifiedValue {
	DCBlocker _dcBlocker;

	FastRootMeanSquare(float sampleRate = 1000.0f, float sensitivity = 1.0f, float maxDelayMS = 300.0f)
	: AverageRectifiedValue(sampleRate, sensitivity, maxDelayMS)
	{
	}

	float next(float sample) override;
};

struct PureRootMeanSquare : RunningAverage {
	PureRootMeanSquare(float sampleRate = 1000.0f, float sensitivity = 1.0f, float maxDelayMS = 300.0f)
	: RunningAverage(sampleRate, sensitivity, maxDelayMS)
	{
	}

	float next(float sample) override;
};

typedef FastRootMeanSquare RootMeanSquare;

// Puckette 2007, "Theory and Technique"
struct PucketteEnvelopeFollower {
	DCBlocker _dcBlocker;
	LowPassFilter _filter;

	void setParams(float sampleRate, float sensitivity);
	float next(float sample);
};

typedef PucketteEnvelopeFollower EnvelopeFollower;

} // namespace dsp
} // namespace bogaudio
