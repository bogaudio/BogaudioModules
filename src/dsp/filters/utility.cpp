
#include "filters/utility.hpp"
#include <cstdlib>

using namespace bogaudio::dsp;

float DCBlocker::next(float sample) {
	const float r = 0.999f;
	_lastOut = sample - _lastIn + r * _lastOut;
	_lastIn = sample;
	return _lastOut;
}


float AverageRectifiedValue::next(float sample) {
	return RunningAverage::next(fabsf(sample));
}


float FastRootMeanSquare::next(float sample) {
	return AverageRectifiedValue::next(_dcBlocker.next(sample));
}


float PureRootMeanSquare::next(float sample) {
	float a = RunningAverage::next(sample * sample);
	if (_sum <= 0.0) {
		return 0.0f;
	}
	return sqrtf(a);
}


void PucketteEnvelopeFollower::setParams(float sampleRate, float sensitivity) {
	const float maxCutoff = 10000.0f;
	const float minCutoff = 100.0f;
	assert(sensitivity >= 0.0f && sensitivity <= 1.0f);
	float cutoff = minCutoff + sensitivity * (maxCutoff - minCutoff);
	_filter.setParams(sampleRate, cutoff);
}

float PucketteEnvelopeFollower::next(float sample) {
	return _filter.next(fabsf(_dcBlocker.next(sample)));
}
