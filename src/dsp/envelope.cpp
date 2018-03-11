#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "envelope.hpp"

using namespace bogaudio::dsp;

void EnvelopeGenerator::setSampleRate(float sampleRate) {
	if (_sampleRate != sampleRate && sampleRate >= 1.0) {
		_sampleRate = sampleRate;
		_sampleTime = 1.0f / sampleRate;
		_sampleRateChanged();
	}
}


void ADSR::setGate(bool high) {
	_gated = high;
}

void ADSR::setAttack(float seconds) {
	assert(_attack >= 0.0f);
	_attack = seconds;
}

void ADSR::setDecay(float seconds) {
	assert(_decay >= 0.0f);
	_decay = seconds;
}

void ADSR::setSustain(float level) {
	assert(_sustain >= 0.0f);
	assert(_sustain <= 1.0f);
	_sustain = level;
}

void ADSR::setRelease(float seconds) {
	assert(_release >= 0.0f);
	_release = seconds;
}

float ADSR::_next() {
	if (_gated) {
		switch (_stage) {
			case STOPPED_STAGE: {
				_stage = ATTACK_STAGE;
				_stageProgress = 0.0f;
				break;
			}
			case ATTACK_STAGE: {
				if (_envelope >= 1.0) {
					_stage = DECAY_STAGE;
					_stageProgress = 0.0f;
				}
				break;
			}
			case DECAY_STAGE: {
				if (_stageProgress >= _decay) {
					_stage = SUSTAIN_STAGE;
					_stageProgress = 0.0f;
				}
				break;
			}
			case SUSTAIN_STAGE: {
				break;
			}
			case RELEASE_STAGE: {
				_stage = ATTACK_STAGE;
				_stageProgress = _attack * powf(_envelope, 2.0f);
				break;
			}
		}
	}
	else {
		switch (_stage) {
			case STOPPED_STAGE: {
				break;
			}
			case ATTACK_STAGE:
			case DECAY_STAGE:
			case SUSTAIN_STAGE: {
				_stage = RELEASE_STAGE;
				_stageProgress = 0.0f;
				_releaseLevel = _envelope;
				break;
			}
			case RELEASE_STAGE: {
				if (_stageProgress >= _release) {
					_stage = STOPPED_STAGE;
				}
				break;
			}
		}
	}

	switch (_stage) {
		case STOPPED_STAGE: {
			_envelope = 0.0f;
			break;
		}
		case ATTACK_STAGE: {
			_stageProgress += _sampleTime;
			_envelope = _stageProgress / _attack;
			_envelope = powf(_envelope, 0.5f);
			break;
		}
		case DECAY_STAGE: {
			_stageProgress += _sampleTime;
			_envelope = _stageProgress / _decay;
			_envelope = powf(_envelope, 0.5f);
			_envelope *= 1.0f - _sustain;
			_envelope = 1.0f - _envelope;
			break;
		}
		case SUSTAIN_STAGE: {
			_envelope = _sustain;
			break;
		}
		case RELEASE_STAGE: {
			_stageProgress += _sampleTime;
			_envelope = _stageProgress / _release;
			_envelope = powf(_envelope, 0.5f);
			_envelope *= _releaseLevel;
			_envelope = _releaseLevel - _envelope;
			break;
		}
	}

	return 10.0f * _envelope;
}
