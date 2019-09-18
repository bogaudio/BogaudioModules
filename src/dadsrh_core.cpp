
#include "dadsrh_core.hpp"

void DADSRHCore::reset() {
	_trigger.reset();
	_triggerOuptutPulseGen.process(10.0);
	_stage = STOPPED_STAGE;
	_releaseLevel = _holdProgress = _stageProgress = _envelope = 0.0;
}

void DADSRHCore::step(int c, int channels) {
	const int SHAPE1 = 1;
	const int SHAPE2 = 2;
	const int SHAPE3 = 3;
	const float shapeExponent = 2.0;
	const float shapeInverseExponent = 0.5;

	bool slow = _speedParam.getValue() <= 0.5;
	if (
		_trigger.process(_triggerParam.getValue() + _triggerInput.getPolyVoltage(c)) ||
		(_firstStep && _triggerOnLoad && _shouldTriggerOnLoad && _loopParam.getValue() < 0.5 && _modeParam.getValue() < 0.5)
	) {
		if (_stage == STOPPED_STAGE || _retriggerParam.getValue() <= 0.5) {
			_stage = DELAY_STAGE;
			_holdProgress = _stageProgress = _envelope = 0.0;
		}
		else {
			switch (_stage) {
				case STOPPED_STAGE:
				case ATTACK_STAGE: {
					break;
				}

				case DELAY_STAGE: {
					_stage = ATTACK_STAGE;
					_stageProgress = _envelope = 0.0;

					// we're skipping the delay; subtract the full delay time from hold time so that env has the same shape as if we'd waited out the delay.
					float delayTime = knobTime(c, _delayParam, _delayInput, slow, true);
					float holdTime = knobTime(c, _holdParam, _holdInput, slow);
					_holdProgress = fminf(1.0, delayTime / holdTime);
					break;
				}

				case DECAY_STAGE:
				case SUSTAIN_STAGE:
				case RELEASE_STAGE: {
					_stage = ATTACK_STAGE;
					switch ((int)_attackShapeParam.getValue()) {
						case SHAPE2: {
							_stageProgress = _envelope;
							break;
						}
						case SHAPE3: {
							_stageProgress = pow(_envelope, shapeInverseExponent);
							break;
						}
						default: {
							_stageProgress = pow(_envelope, shapeExponent);
							break;
						}
					}

					// reset hold to what it would have been at this point in the attack the first time through.
					float delayTime = knobTime(c, _delayParam, _delayInput, slow, true);
					float attackTime = knobTime(c, _attackParam, _attackInput, slow);
					float holdTime = knobTime(c, _holdParam, _holdInput, slow);
					_holdProgress = fminf(1.0, (delayTime + _stageProgress * attackTime) / holdTime);
					break;
				}
			}
		}
	}
	else {
		switch (_stage) {
			case STOPPED_STAGE:
			case RELEASE_STAGE: {
				break;
			}

			case DELAY_STAGE:
			case ATTACK_STAGE:
			case DECAY_STAGE:
			case SUSTAIN_STAGE: {
				bool gateMode = _modeParam.getValue() > 0.5;
				bool holdComplete = _holdProgress >= 1.0;
				if (!holdComplete) {
					// run the hold accumulation even if we're not in hold mode, in case we switch mid-cycle.
					_holdProgress += stepAmount(c, _holdParam, _holdInput, slow);
					holdComplete = _holdProgress >= 1.0;
				}

				if (gateMode ? !_trigger.isHigh() : holdComplete) {
					_stage = RELEASE_STAGE;
					_stageProgress = 0.0;
					_releaseLevel = _envelope;
				}
				break;
			}
		}
	}

	bool complete = false;
	switch (_stage) {
		case STOPPED_STAGE: {
			break;
		}

		case DELAY_STAGE: {
			_stageProgress += stepAmount(c, _delayParam, _delayInput, slow, true);
			if (_stageProgress >= 1.0) {
				_stage = ATTACK_STAGE;
				_stageProgress = 0.0;
			}
			break;
		}

		case ATTACK_STAGE: {
			_stageProgress += stepAmount(c, _attackParam, _attackInput, slow);
			switch ((int)_attackShapeParam.getValue()) {
				case SHAPE2: {
					_envelope = _stageProgress;
					break;
				}
				case SHAPE3: {
					_envelope = pow(_stageProgress, shapeExponent);
					break;
				}
				default: {
					_envelope = pow(_stageProgress, shapeInverseExponent);
					break;
				}
			}
			if (_envelope >= 1.0) {
				_stage = DECAY_STAGE;
				_stageProgress = 0.0;
			}
			break;
		}

		case DECAY_STAGE: {
			float sustainLevel = knobAmount(c, _sustainParam, _sustainInput);
			_stageProgress += stepAmount(c, _decayParam, _decayInput, slow);
			switch ((int)_decayShapeParam.getValue()) {
				case SHAPE2: {
					_envelope = 1.0 - _stageProgress;
					break;
				}
				case SHAPE3: {
					_envelope = _stageProgress >= 1.0 ? 0.0 : pow(1.0 - _stageProgress, shapeInverseExponent);
					break;
				}
				default: {
					_envelope = _stageProgress >= 1.0 ? 0.0 : pow(1.0 - _stageProgress, shapeExponent);
					break;
				}
			}
			_envelope *= 1.0 - sustainLevel;
			_envelope += sustainLevel;
			if (_envelope <= sustainLevel) {
				_stage = SUSTAIN_STAGE;
			}
			break;
		}

		case SUSTAIN_STAGE: {
			_envelope = knobAmount(c, _sustainParam, _sustainInput);
			break;
		}

		case RELEASE_STAGE: {
			_stageProgress += stepAmount(c, _releaseParam, _releaseInput, slow);
			switch ((int)_releaseShapeParam.getValue()) {
				case SHAPE2: {
					_envelope = 1.0 - _stageProgress;
					break;
				}
				case SHAPE3: {
					_envelope = _stageProgress >= 1.0 ? 0.0 : pow(1.0 - _stageProgress, shapeInverseExponent);
					break;
				}
				default: {
					_envelope = _stageProgress >= 1.0 ? 0.0 : pow(1.0 - _stageProgress, shapeExponent);
					break;
				}
			}
			_envelope *= _releaseLevel;
			if (_envelope <= 0.001) {
				complete = true;
				_envelope = 0.0;
				if (_modeParam.getValue() < 0.5 && (_loopParam.getValue() <= 0.5 || _trigger.isHigh())) {
					_stage = DELAY_STAGE;
					_holdProgress = _stageProgress = 0.0;
				}
				else {
					_stage = STOPPED_STAGE;
				}
			}
			break;
		}
	}

	float env = _envelope * 10.0;
	_envOutput.setChannels(channels);
	_envOutput.setVoltage(env, c);
	_invOutput.setChannels(channels);
	_invOutput.setVoltage(10.0 - env, c);

	if (complete) {
		_triggerOuptutPulseGen.trigger(0.001);
	}
	_triggerOutput.setChannels(channels);
	_triggerOutput.setVoltage(_triggerOuptutPulseGen.process(APP->engine->getSampleTime()) ? 5.0 : 0.0, c);

	if (_delayOutput) {
		_delayOutput->setChannels(channels);
		_delayOutput->setVoltage(_stage == DELAY_STAGE ? 5.0 : 0.0, c);
	}
	if (_attackOutput) {
		_attackOutput->setChannels(channels);
		_attackOutput->setVoltage(_stage == ATTACK_STAGE ? 5.0 : 0.0, c);
	}
	if (_decayOutput) {
		_decayOutput->setChannels(channels);
		_decayOutput->setVoltage(_stage == DECAY_STAGE ? 5.0 : 0.0, c);
	}
	if (_sustainOutput) {
		_sustainOutput->setChannels(channels);
		_sustainOutput->setVoltage(_stage == SUSTAIN_STAGE ? 5.0 : 0.0, c);
	}
	if (_releaseOutput) {
		_releaseOutput->setChannels(channels);
		_releaseOutput->setVoltage(_stage == RELEASE_STAGE ? 5.0 : 0.0, c);
	}

	_delayLights[c] = _stage == DELAY_STAGE;
	_attackLights[c] = _stage == ATTACK_STAGE;
	_decayLights[c] = _stage == DECAY_STAGE;
	_sustainLights[c] = _stage == SUSTAIN_STAGE;
	_releaseLights[c] = _stage == RELEASE_STAGE;

	_attackShape1Light.value = (int)_attackShapeParam.value == SHAPE1;
	_attackShape2Light.value = (int)_attackShapeParam.value == SHAPE2;
	_attackShape3Light.value = (int)_attackShapeParam.value == SHAPE3;
	_decayShape1Light.value = (int)_decayShapeParam.value == SHAPE1;
	_decayShape2Light.value = (int)_decayShapeParam.value == SHAPE2;
	_decayShape3Light.value = (int)_decayShapeParam.value == SHAPE3;
	_releaseShape1Light.value = (int)_releaseShapeParam.value == SHAPE1;
	_releaseShape2Light.value = (int)_releaseShapeParam.value == SHAPE2;
	_releaseShape3Light.value = (int)_releaseShapeParam.value == SHAPE3;

	_firstStep = false;
}

float DADSRHCore::stepAmount(int c, Param& knob, Input* cv, bool slow, bool allowZero) {
	return APP->engine->getSampleTime() / knobTime(c, knob, cv, slow, allowZero);
}

float DADSRHCore::knobTime(int c, Param& knob, Input* cv, bool slow, bool allowZero) {
	float t = knobAmount(c, knob, cv);
	t = pow(t, 2.0);
	t = fmaxf(t, allowZero ? 0.0 : 0.001);
	return t * (slow ? 100.0 : 10.0);
}

float DADSRHCore::knobAmount(int c, Param& knob, Input* cv) const {
	float v = clamp(knob.getValue(), 0.0f, 1.0f);
	if (cv && cv->isConnected()) {
		v *= clamp(cv->getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	return v;
}
