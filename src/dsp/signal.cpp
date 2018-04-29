
#include <assert.h>

#include "signal.hpp"

using namespace bogaudio::dsp;

bool PositiveZeroCrossing::next(float sample) {
	switch (_state) {
		case NEGATIVE_STATE: {
			if (sample > positiveThreshold) {
				_state = POSITIVE_STATE;
				return true;
			}
			break;
		}
		case POSITIVE_STATE: {
			if (sample < negativeThreshold) {
				_state = NEGATIVE_STATE;
			}
			else if (sample < positiveThreshold && _triggerable) {
				_state = COUNT_ZEROES_STATE;
				_zeroCount = 1;
			}
			break;
		}
		case COUNT_ZEROES_STATE: {
			if (sample >= negativeThreshold) {
				if (++_zeroCount >= zeroesForReset) {
					_state = NEGATIVE_STATE;
				}
			}
			else {
				_state = NEGATIVE_STATE;
			}
			break;
		}
	}
	return false;
}

void PositiveZeroCrossing::reset() {
	_state = NEGATIVE_STATE;
}


void SlewLimiter::setParams(float sampleRate, float milliseconds) {
	assert(sampleRate > 0.0f);
	assert(milliseconds >= 0.0f);
	_sampleRate = sampleRate;
	_milliseconds = milliseconds;
	_samples = (_milliseconds / 1000.0f) * _sampleRate;
}

float SlewLimiter::next(float sample) {
	if (_samples < 2 || (sample > _current - 0.01f && sample < _current + 0.01f)) {
		_current = sample;
		return sample;
	}
	if (_target != sample) {
		_target = sample;
		_delta = (sample - _current) / _samples;
		_steps = _samples;
	}
	else if (_steps <= 1) {
		_current = sample;
		return sample;
	}
	_current += _delta;
	--_steps;
	return _current;
}


void CrossFader::setMix(float mix) {
	assert(mix >= -1.0f && mix <= 1.0f);
	if (_mix != mix) {
		_mix = mix;
		if (_mix < 0.0f) {
			_aMix = 1.0f;
			_bMix = 1.0f + _mix;
		}
		else {
			_aMix = 1.0f - _mix;
			_bMix = 1.0f;
		}
	}
}

float CrossFader::next(float a, float b) {
	return _aMix * a + _bMix * b;
}
