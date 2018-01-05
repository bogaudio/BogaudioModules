#include <math.h>

#include "dsp.hpp"

using namespace bogaudio::dsp;

void SineOscillator::updateDeltaTheta() {
	float sampleTime = 1.0 / _sampleRate;
	float cycleTime = 1.0 / _frequency;
	float _deltaTheta = (sampleTime / cycleTime) * 2.0f * M_PI;
	for (int i = 0; i < _n; ++i) {
		_sinDeltaTheta[i] = sinf((i + 1) * _deltaTheta);
		_cosDeltaTheta[i] = cosf((i + 1) * _deltaTheta);
	}
}

float SineOscillator::_next() {
	if (_step == 0) {
		const int n1 = _n - 1;
		for (int i = 0; i < n1; ++i) {
			_x[i] = _x[n1];
			_y[i] = _y[n1];
		}

		float t[_n];
		for (int i = 0; i < _n; ++i) {
			t[i] = _x[i]*_cosDeltaTheta[i] - _y[i]*_sinDeltaTheta[i];
			_y[i] = _x[i]*_sinDeltaTheta[i] + _y[i]*_cosDeltaTheta[i];
			_x[i] = t[i];
		}
	}
	float out = _y[_step];
	_step = ++_step % _n;
	return out;
}
