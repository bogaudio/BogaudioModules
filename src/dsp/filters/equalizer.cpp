
#include <assert.h>

#include "filters/equalizer.hpp"

using namespace bogaudio::dsp;

void Equalizer::setParams(
	float sampleRate,
	float lowDb,
	float midDb,
	float highDb
) {
	assert(lowDb >= cutDb && lowDb <= gainDb);
	assert(midDb >= cutDb && midDb <= gainDb);
	assert(highDb >= cutDb && highDb <= gainDb);

	_lowAmp.setLevel(lowDb);
	_lowFilter.setParams(sampleRate, 100.0f, 0.0f);

	_midAmp.setLevel(midDb);
	_midFilter.setParams(sampleRate, 350.0f, 0.55f, MultimodeFilter::PITCH_BANDWIDTH_MODE);

	_highAmp.setLevel(highDb);
	_highFilter.setParams(sampleRate, 1000.0f, 0.0f);
}

float Equalizer::next(float sample) {
	float low = _lowAmp.next(_lowFilter.next(sample));
	float mid = _midAmp.next(_midFilter.next(sample));
	float high = _highAmp.next(_highFilter.next(sample));
	return low + mid + high;
}
