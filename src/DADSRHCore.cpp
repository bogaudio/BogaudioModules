
#include <math.h>
#include "DADSRHCore.hpp"

void DADSRHCore::reset() {
	_trigger.reset();
	_stage = STOPPED_STAGE;
	_releaseLevel = _holdProgress = _stageProgress = _envelope = 0.0;

	_delayLight = 0.0;
	_attackLight = 0.0;
	_decayLight = 0.0;
	_sustainLight = 0.0;
	_releaseLight = 0.0;
	_attackShape1Light = 0.0;
	_attackShape2Light = 0.0;
	_attackShape3Light = 0.0;
	_decayShape1Light = 0.0;
	_decayShape2Light = 0.0;
	_decayShape3Light = 0.0;
	_releaseShape1Light = 0.0;
	_releaseShape2Light = 0.0;
	_releaseShape3Light = 0.0;
}

void DADSRHCore::step() {
  const int SHAPE1 = 1;
  const int SHAPE2 = 2;
  const int SHAPE3 = 3;
  const float shapeExponent = 2.0;
  const float shapeInverseExponent = 0.5;

	bool slow = _speedParam.value <= 0.5;
  if (_trigger.process(_triggerParam.value + _triggerInput.value)) {
    if (_stage == STOPPED_STAGE || _retriggerParam.value <= 0.5) {
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
          float delayTime = knobTime(_delayParam, _delayInput, slow, true);
          float holdTime = knobTime(_holdParam, _holdInput, slow);
          _holdProgress = fminf(1.0, delayTime / holdTime);
          break;
        }

        case DECAY_STAGE:
        case SUSTAIN_STAGE:
        case RELEASE_STAGE: {
          _stage = ATTACK_STAGE;
          switch ((int)_attackShapeParam.value) {
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
          float delayTime = knobTime(_delayParam, _delayInput, slow, true);
          float attackTime = knobTime(_attackParam, _attackInput, slow);
          float holdTime = knobTime(_holdParam, _holdInput, slow);
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
        bool gateMode = _modeParam.value > 0.5;
        bool holdComplete = _holdProgress >= 1.0;
        if (!holdComplete) {
          // run the hold accumulation even if we're not in hold mode, in case we switch mid-cycle.
          _holdProgress += stepAmount(_holdParam, _holdInput, slow);
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
      _stageProgress += stepAmount(_delayParam, _delayInput, slow, true);
			if (_stageProgress >= 1.0) {
				_stage = ATTACK_STAGE;
        _stageProgress = 0.0;
      }
			break;
		}

		case ATTACK_STAGE: {
      _stageProgress += stepAmount(_attackParam, _attackInput, slow);
      switch ((int)_attackShapeParam.value) {
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
      float sustainLevel = knobAmount(_sustainParam, _sustainInput);
      _stageProgress += stepAmount(_decayParam, _decayInput, slow);
      switch ((int)_decayShapeParam.value) {
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
      _envelope = knobAmount(_sustainParam, _sustainInput);
      break;
    }

		case RELEASE_STAGE: {
      _stageProgress += stepAmount(_releaseParam, _releaseInput, slow);
      switch ((int)_releaseShapeParam.value) {
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
				if (_loopParam.value <= 0.5) {
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
	_envOutput.value = env;
	_invOutput.value = 10.0 - env;
	_triggerOutput.value = complete ? 5.0 : 0.0;

  if (_delayOutput) {
    _delayOutput->value = _stage == DELAY_STAGE ? 5.0 : 0.0;
	}
  if (_attackOutput) {
    _attackOutput->value = _stage == ATTACK_STAGE ? 5.0 : 0.0;
	}
  if (_decayOutput) {
    _decayOutput->value = _stage == DECAY_STAGE ? 5.0 : 0.0;
	}
  if (_sustainOutput) {
    _sustainOutput->value = _stage == SUSTAIN_STAGE ? 5.0 : 0.0;
	}
  if (_releaseOutput) {
	  _releaseOutput->value = _stage == RELEASE_STAGE ? 5.0 : 0.0;
	}

	_delayLight = _stage == DELAY_STAGE;
	_attackLight = _stage == ATTACK_STAGE;
	_decayLight = _stage == DECAY_STAGE;
	_sustainLight = _stage == SUSTAIN_STAGE;
	_releaseLight = _stage == RELEASE_STAGE;

	_attackShape1Light = ((int)_attackShapeParam.value) == SHAPE1;
	_attackShape2Light = ((int)_attackShapeParam.value) == SHAPE2;
	_attackShape3Light = ((int)_attackShapeParam.value) == SHAPE3;
	_decayShape1Light = ((int)_decayShapeParam.value) == SHAPE1;
	_decayShape2Light = ((int)_decayShapeParam.value) == SHAPE2;
	_decayShape3Light = ((int)_decayShapeParam.value) == SHAPE3;
	_releaseShape1Light = ((int)_releaseShapeParam.value) == SHAPE1;
	_releaseShape2Light = ((int)_releaseShapeParam.value) == SHAPE2;
	_releaseShape3Light = ((int)_releaseShapeParam.value) == SHAPE3;
}

float DADSRHCore::stepAmount(const Param& knob, const Input* cv, bool slow, bool allowZero) {
	return (1.0 / gSampleRate) / knobTime(knob, cv, slow, allowZero);
}

float DADSRHCore::knobTime(const Param& knob, const Input* cv, bool slow, bool allowZero) {
	float t = knobAmount(knob, cv);
  t = pow(t, 2.0);
	t = fmaxf(t, allowZero ? 0.0 : 0.001);
	return t * (slow ? 100.0 : 10.0);
}

float DADSRHCore::knobAmount(const Param& knob, const Input* cv) const {
  float v = clampf(knob.value, 0.0, 1.0);
  if (cv && cv->active) {
    v *= clampf(cv->value / 10.0, 0.0, 1.0);
	}
  return v;
}
