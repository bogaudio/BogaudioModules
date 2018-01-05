#include <math.h>

#include "dsp.hpp"

using namespace bogaudio::dsp;

void SineOscillator::updateDeltaTheta() {
	float sampleTime = 1.0 / _sampleRate;
	float cycleTime = 1.0 / _frequency;
	float _deltaTheta = (sampleTime / cycleTime) * 2.0f * M_PI;
	_sinDeltaTheta = sinf(_deltaTheta);
	_cosDeltaTheta = cosf(_deltaTheta);
}

float SineOscillator::_next() {
	float x2 = _x*_cosDeltaTheta - _y*_sinDeltaTheta;
	_y = _x*_sinDeltaTheta + _y*_cosDeltaTheta;
	_x = x2;
	return _y;
}
