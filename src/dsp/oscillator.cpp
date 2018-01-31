#include <math.h>

#include "oscillator.hpp"

using namespace bogaudio::dsp;

void Phasor::updateDelta() {
	float sampleTime = 1.0 / _sampleRate;
	float cycleTime = 1.0 / _frequency;
	_delta = (sampleTime / cycleTime) * 2.0f;
}

float Phasor::_next() {
	_phase += _delta;
	if (_phase >= 2.0) {
		_phase -= 2.0;
	}
	return _phase;
}


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
	++_step;
	_step %= _n;
	return out;
}


float SawOscillator::_next() {
	Phasor::_next();
	return _amplitude * (_phase - 1.0f);
}


void SquareOscillator::setPulseWidth(float pw) {
	if (pw >= 0.9) {
		_pulseWidth = 0.9;
	}
	else if (pw <= 0.1) {
		_pulseWidth = 0.1;
	}
	else {
		_pulseWidth = pw;
	}
	_pulseWidth *= 2.0;
}

float SquareOscillator::_next() {
	Phasor::_next();
	if (positive) {
		if (_phase >= _pulseWidth) {
			positive = false;
			return _negativeAmplitude;
		}
		return _amplitude;
	}
	if (_phase < _pulseWidth) {
		positive = true;
		return _amplitude;
	}
	return _negativeAmplitude;
}


float TriangleOscillator::_next() {
	Phasor::_next();
	float p = 2.0 * _phase;
	if (_phase < 0.5) {
		return _amplitude * p;
	}
	if (_phase < 1.5) {
		return _amplitude * (2.0 - p);
	}
	return _amplitude * (p - 4.0);
}
