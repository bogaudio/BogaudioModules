#include <math.h>

#include "oscillator.hpp"

using namespace bogaudio::dsp;

void Phasor::setPhase(float phase) {
	_phase = phase / M_PI;
}

void Phasor::updateDelta() {
	_delta = (_frequency / _sampleRate) * 2.0f;
}

float Phasor::_next() {
	_phase += _delta;
	if (_phase >= 2.0f) {
		_phase -= 2.0f;
	}
	else if (_phase <= 0.0f) {
		_phase += 2.0f;
	}
	return _phase;
}


// A New Recursive Quadrature Oscillator, Martin Vicanek, 2015 - http://vicanek.de/articles/QuadOsc.pdf
void SineOscillator::setPhase(double phase) {
	_x = cos(phase);
	_y = sin(phase);
}

void SineOscillator::update() {
	double w = (_frequency / _sampleRate) * 2.0 * M_PI;
	_k1 = tan(w / 2.0);
	_k2 = 2.0 * _k1 / (1 + _k1*_k1);
}

float SineOscillator::_next() {
	double t = _x - _k1*_y;
	_y = _y + _k2*t;
	_x = t - _k1*_y;
	return _amplitude * _y;
}


float SawOscillator::_next() {
	Phasor::_next();
	return _amplitude * (_phase - 1.0f);
}


void SquareOscillator::setPulseWidth(float pw) {
	if (pw >= 0.97f) {
		_pulseWidth = 0.97f;
	}
	else if (pw <= 0.03f) {
		_pulseWidth = 0.03f;
	}
	else {
		_pulseWidth = pw;
	}
	_pulseWidth *= 2.0f;
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
	float p = 2.0f * _phase;
	if (_phase < 0.5f) {
		return _amplitude * p;
	}
	if (_phase < 1.5f) {
		return _amplitude * (2.0f - p);
	}
	return _amplitude * (p - 4.0f);
}


void SineBankOscillator::setPartial(int i, float frequencyRatio, float amplitude) {
	setPartialFrequencyRatio(i, frequencyRatio);
	setPartialAmplitude(i, amplitude);
}

void SineBankOscillator::setPartialFrequencyRatio(int i, float frequencyRatio) {
	if (i <= (int)_partials.size()) {
		Partial& p = _partials[i - 1];
		p.frequencyRatio = frequencyRatio;
		p.frequency = _frequency * frequencyRatio;
		p.sine.setFrequency((double)_frequency * (double)frequencyRatio);
	}
}

void SineBankOscillator::setPartialAmplitude(int i, float amplitude) {
	if (i <= (int)_partials.size()) {
		_partials[i - 1].amplitude = amplitude;
	}
}

void SineBankOscillator::syncToPhase(float phase) {
	for (Partial& p : _partials) {
		p.sine.setPhase(phase);
	}
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
		if (p.frequency < _maxPartialFrequency && (p.amplitude > 0.001 || p.amplitude < -0.001)) {
			next += p.sine.next() * p.amplitude;
		}
		else {
			p.sine.next(); // keep spinning, maintain phase.
		}
	}
	return next;
}
