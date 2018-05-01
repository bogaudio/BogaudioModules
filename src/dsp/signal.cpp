
#include <assert.h>
#include <algorithm>

#include "signal.hpp"

using namespace bogaudio::dsp;

const float Amplifier::minDecibels = -60.0f;
const float Amplifier::maxDecibels = 20.0f;
const float Amplifier::decibelsRange = maxDecibels - minDecibels;

void Amplifier::LevelTable::_generate() {
	const float rdb = 6.0f;
	const float tdb = Amplifier::minDecibels + rdb;
	const float ta = decibelsToAmplitude(tdb);
	_table[0] = 0.0f;
	for (int i = 1; i < _length; ++i) {
		float db = Amplifier::minDecibels + (i / (float)_length) * Amplifier::decibelsRange;
		if (db <= tdb) {
			_table[i] = ((db - minDecibels) / rdb) * ta;
		}
		else {
			_table[i] = decibelsToAmplitude(db);
		}
	}
}

void Amplifier::setLevel(float db) {
	if (_db != db) {
		_db = db;
		if (_db > minDecibels) {
			if (_db < maxDecibels) {
				_level = _table.value(((_db - minDecibels) / decibelsRange) * _table.length());
			}
			else {
				_level = decibelsToAmplitude(_db);
			}
		}
		else {
			_level = 0.0f;
		}
	}
}

float Amplifier::next(float s) {
	return _level * s;
}


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
