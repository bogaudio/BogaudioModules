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
	float deltaTheta = (_frequency / _sampleRate) * 2.0f * M_PI;
	for (int i = 0; i < _n; ++i) {
		_sinDeltaTheta[i] = sinf((i + 1) * deltaTheta);
		_cosDeltaTheta[i] = cosf((i + 1) * deltaTheta);
	}
}

float SineOscillator::_next() {
	++_sampleCount;

	if (_step == 0) {
		const int n1 = _n - 1;
		if (_sampleCount > _maxSamplesBeforeNormalize) {
			_sampleCount = 0;

			// https://dsp.stackexchange.com/questions/124/how-to-implement-a-digital-oscillator
			// float g = (3.0f - (_x[n1]*_x[n1] + _y[n1]*_y[n1])) / 2.0f; // would work if _amplitude was always 1.0?
			float g = _amplitude / sqrtf(_x[n1]*_x[n1] + _y[n1]*_y[n1]);
			_x[n1] *= g;
			_y[n1] *= g;
		}

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
	Phasor::_sampleRateChanged();

	_maxPartialFrequency = _maxPartialFrequencySRRatio * _sampleRate;
	for (Partial& p : _partials) {
		p.sine.setSampleRate(_sampleRate);
	}
}

void SineBankOscillator::_frequencyChanged() {
	Phasor::_frequencyChanged();

	for (Partial& p : _partials) {
		p.frequency = _frequency * p.frequencyRatio;
		p.sine.setFrequency(_frequency * p.frequencyRatio);
	}
}

float SineBankOscillator::_next() {
	Phasor::_next();

	if (++_steps >= _stepsToReset) {
		_steps = 0;
		// float phase = _phase * M_PI;
		// for (Partial& p : _partials) {
		// 	p.sine.setPhase(phase * p.frequencyRatio);
		// }
	}

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


void SineBankOscillator2::setPartial(int i, float frequencyRatio, float amplitude, float* phase) {
	if (i > (int)_partials.size()) {
		return;
	}

	Partial& p = _partials[i - 1];
	if (amplitude > 0.01) {
		p.enabled = true;
		p.frequencyRatio = frequencyRatio;
		p.amplitude = amplitude;
		if (phase) {
			p.phase = _radiansToPhase(*phase);
		}
		_updatePartial(p);
	}
	else {
		p.enabled = false;
	}
}

void SineBankOscillator2::_updatePartial(Partial& p) {
	const float d = (float)pow(10, resolution);
	p.frequency = _frequency * p.frequencyRatio;
	p.frequency *= d;
	p.frequency = roundf(p.frequency);
	p.deltaPhase = (phase_t)p.frequency;
	p.frequency /= d;
	// printf("UPDATE fr=%f f=%f dp=%u k=%d mp=%u f0=%f\n", p.frequencyRatio, p.frequency, p.deltaPhase, resolution, _maxPhase, _frequency);
}

void SineBankOscillator2::_sampleRateChanged() {
	_maxPartialFrequency = _maxPartialFrequencySRRatio * _sampleRate;
	_maxPhase = pow(10, resolution) * _sampleRate;
	for (Partial& p : _partials) {
		_updatePartial(p);
		// need to scale phase?
	}
}

void SineBankOscillator2::_frequencyChanged() {
	for (Partial& p : _partials) {
		_updatePartial(p);
	}
}

float SineBankOscillator2::_next() {
	float next = 0.0;
	for (Partial& p : _partials) {
		p.phase = (p.phase + p.deltaPhase) % _maxPhase;
		if (p.enabled && p.frequencyRatio * _frequency < _maxPartialFrequency) {
			next += p.amplitude * sinf(_phaseToRadians(p.phase));
		}
	}
	return next;
}
