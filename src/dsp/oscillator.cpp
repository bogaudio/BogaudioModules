#include <math.h>

#include "oscillator.hpp"

using namespace bogaudio::dsp;

void Phasor::setPhase(float radians) {
	_phase = radiansToPhase(radians);
}

float Phasor::nextFromPhasor(const Phasor& phasor, float offset) {
	float p = phasor._phase + offset;
	while (p >= maxPhase) {
		p -= maxPhase;
	}
	while (p < 0.0f) {
		p += maxPhase;
	}
	return _nextForPhase(p);
}

void Phasor::_update() {
	_delta = (_frequency / _sampleRate) * maxPhase;
}

void Phasor::advancePhase() {
	_phase += _delta;
	if (_phase >= maxPhase) {
		_phase -= maxPhase;
	}
	else if (_phase <= 0.0f && _delta != 0.0f) {
		_phase += maxPhase;
	}
}

float Phasor::_next() {
	advancePhase();
	return _nextForPhase(_phase);
}

float Phasor::_nextForPhase(float phase) {
	return phase;
}


float TablePhasor::_nextForPhase(float phase) {
	while (phase >= maxPhase) {
		phase -= maxPhase;
	}
	while (phase < 0.0f) {
		phase += maxPhase;
	}
	float fi = phase * (_table.length() / 2);
	int i = (int)fi;
	float v1 = _table.value(i);
	if (_table.length() >= 1024) {
		return v1 * _amplitude;
	}
	float v2 = _table.value(i + 1 == _table.length() ? 0 : i + 1);
	return _amplitude * (v1 + (fi - i)*(v2 - v1));
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


float SawOscillator::_nextForPhase(float phase) {
	return _amplitude * (phase - halfMaxPhase);
}


void SaturatingSawOscillator::setSaturation(float saturation) {
	if (_saturation != saturation) {
		assert(saturation >= 0.0f);
		_saturation = saturation;
		if (_saturation < 1.0f) {
			_saturationNormalization = 1.0f / tanhf(_saturation * M_PI);
		}
		else {
			_saturationNormalization = 1.0f;
		}
	}
}

float SaturatingSawOscillator::_nextForPhase(float phase) {
	float sample = SawOscillator::_nextForPhase(phase);
	if (_saturation >= 0.1f) {
		sample = tanhf(-sample * _saturation * M_PI) * _saturationNormalization;
	}
	return _amplitude2 * sample;
}


void BandLimitedSawOscillator::setQuality(int quality) {
	if (_quality != quality) {
		assert(quality >= 0);
		_quality = quality;
		_update();
	}
}

void BandLimitedSawOscillator::_update() {
	Phasor::_update();
	int q = std::min(_quality, (int)(0.5f * (_sampleRate / _frequency)));
	_qd = q * _delta;
}

float BandLimitedSawOscillator::_nextForPhase(float phase) {
	float sample = SaturatingSawOscillator::_nextForPhase(phase);
	if (phase > maxPhase - _qd) {
		float i = (maxPhase - phase) / _qd;
		i = (1.0f - i) * _halfTableLen;
		sample -= _amplitude * _table.value((int)i);
	}
	else if (phase < _qd) {
		float i = phase / _qd;
		i *= _halfTableLen - 1;
		i += _halfTableLen;
		sample -= _amplitude * _table.value((int)i);
	}
	return sample;
}


void SquareOscillator::setPulseWidth(float pw) {
	if (_pulseWidthInput == pw) {
		return;
	}
	_pulseWidthInput = pw;

	if (pw >= maxPulseWidth) {
		_pulseWidth = maxPulseWidth;
	}
	else if (pw <= minPulseWidth) {
		_pulseWidth = minPulseWidth;
	}
	else {
		_pulseWidth = pw;
	}
	_pulseWidth *= maxPhase;
}

float SquareOscillator::_nextForPhase(float phase) {
	if (positive) {
		if (phase >= _pulseWidth) {
			positive = false;
			return _negativeAmplitude;
		}
		return _amplitude;
	}
	if (phase < _pulseWidth) {
		positive = true;
		return _amplitude;
	}
	return _negativeAmplitude;
}


void BandLimitedSquareOscillator::setPulseWidth(float pw) {
	if (_pulseWidthInput == pw) {
		return;
	}
	_pulseWidthInput = pw;

	if (pw >= maxPulseWidth) {
		_pulseWidth = maxPulseWidth;
	}
	else if (pw <= minPulseWidth) {
		_pulseWidth = minPulseWidth;
	}
	else {
		_pulseWidth = pw;
	}
	_pulseWidth *= maxPhase;

	if (_pulseWidth >= 1.0f) {
		_offset = (_pulseWidth - 1.0f) * _amplitude;
	}
	else {
		_offset = -(1.0f - _pulseWidth) * _amplitude;
	}
}

float BandLimitedSquareOscillator::_nextForPhase(float phase) {
	float sample = -BandLimitedSawOscillator::_nextForPhase(phase);
	phase -= _pulseWidth;
	if (phase < 0.0f) {
		phase += maxPhase;
	}
	sample += BandLimitedSawOscillator::_nextForPhase(phase);
	return sample + _offset;
}


float TriangleOscillator::_nextForPhase(float phase) {
	float p = maxPhase * phase;
	if (phase < quarterMaxPhase) {
		return _amplitude * p;
	}
	if (phase < threeQuartersMaxPhase) {
		return _amplitude * (maxPhase - p);
	}
	return _amplitude * (p - twiceMaxPhase);
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

void SineBankOscillator::setPartialAmplitude(int i, float amplitude, bool envelope) {
	if (i <= (int)_partials.size()) {
		Partial& p = _partials[i - 1];
		if (envelope) {
			p.amplitudeTarget = amplitude;
			p.amplitudeStepDelta = (amplitude - p.amplitude) / (float)_amplitudeEnvelopeSamples;
			p.amplitudeSteps = _amplitudeEnvelopeSamples;
		}
		else if (p.amplitudeSteps > 0) {
			p.amplitudeTarget = amplitude;
			p.amplitudeStepDelta = (amplitude - p.amplitude) / (float)p.amplitudeSteps;
		}
		else {
			p.amplitude = amplitude;
		}
	}
}

void SineBankOscillator::syncToPhase(float phase) {
	for (Partial& p : _partials) {
		p.sine.setPhase(phase);
	}
}

void SineBankOscillator::_sampleRateChanged() {
	_maxPartialFrequency = _maxPartialFrequencySRRatio * _sampleRate;
	_amplitudeEnvelopeSamples = _sampleRate * (_amplitudeEnvelopeMS / 1000.0f);
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
		if (p.frequency < _maxPartialFrequency && (p.amplitude > 0.001 || p.amplitude < -0.001 || p.amplitudeSteps > 0)) {
			if (p.amplitudeSteps > 0) {
				if (p.amplitudeSteps == 1) {
					p.amplitude = p.amplitudeTarget;
				}
				else {
					p.amplitude += p.amplitudeStepDelta;
				}
				--p.amplitudeSteps;
			}
			next += p.sine.next() * p.amplitude;
		}
		else {
			p.sine.next(); // keep spinning, maintain phase.
		}
	}
	return next;
}
