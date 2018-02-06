#include <math.h>

#include "oscillator.hpp"

using namespace bogaudio::dsp;

void Phasor::setPhase(float phase) {
	_phase = phase / (2.0 * M_PI);
}

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


void SineOscillator::setPhase(float phase) {
	float x = cosf(phase) * _amplitude;
	float y = sinf(phase) * _amplitude;
	for (int i = 0; i < _n; ++i) {
		_x[i] = x;
		_y[i] = y;
	}
	_step = 0;
}

void SineOscillator::updateDeltaTheta() {
	float deltaTheta = 0.0f;
	if (_sampleRate > 0.0f && _frequency > 0.0f) {
		float sampleTime = 1.0f / _sampleRate;
		float cycleTime = 1.0f / _frequency;
		deltaTheta = (sampleTime / cycleTime) * 2.0f * M_PI;
	}
	for (int i = 0; i < _n; ++i) {
		_sinDeltaTheta[i] = sinf((i + 1) * deltaTheta);
		_cosDeltaTheta[i] = cosf((i + 1) * deltaTheta);
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


void SineBankOscillator::setPartial(int i, float frequencyRatio, float amplitude, float* phase) {
	if (i > (int)_partials.size()) {
		return;
	}

	if (amplitude > 0.01) {
		_partials[i - 1].enabled = true;
		_partials[i - 1].frequencyRatio = frequencyRatio;
		_partials[i - 1].frequency = _frequency * frequencyRatio;
		_partials[i - 1].sine.setFrequency(_frequency * frequencyRatio);
		_partials[i - 1].amplitude = amplitude;
		if (phase) {
			_partials[i - 1].sine.setPhase(*phase);
		}
	}
	else {
		_partials[i - 1].enabled = false;
	}
}

void SineBankOscillator::disablePartial(int i) {
	if (i > (int)_partials.size()) {
		return;
	}
	_partials[i - 1].enabled = false;
}

void SineBankOscillator::_sampleRateChanged() {
	_maxPartialFrequency = _maxPartialFrequencySRRatio * _sampleRate;
	for (Partial& p : _partials) {
		p.sine.setSampleRate(_sampleRate);
	}
}

void SineBankOscillator::_frequencyChanged() {
	for (Partial& p : _partials) {
		p.frequency = _frequency * p.frequencyRatio;
		p.sine.setFrequency(_frequency * p.frequencyRatio);
	}
}

float SineBankOscillator::_next() {
	float next = 0.0;
	for (Partial& p : _partials) {
		if (p.enabled && p.frequency < _maxPartialFrequency) {
			next += p.sine.next() * p.amplitude;
		}
		else {
			p.sine.next(); // keep spinning, maintain phase.
		}
	}
	return next;
}
