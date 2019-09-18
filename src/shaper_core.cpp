
#include "shaper_core.hpp"

void ShaperCore::reset() {
	_trigger.reset();
	_triggerOuptutPulseGen.process(10.0);
	_stage = STOPPED_STAGE;
	_stageProgress = 0.0;
}

void ShaperCore::step(int c, int channels) {
	bool complete = false;
	bool slow = _speedParam.getValue() <= 0.0;
	if (
		_trigger.process(_triggerParam.getValue() + _triggerInput.getPolyVoltage(c)) ||
		(_firstStep && _triggerOnLoad && _shouldTriggerOnLoad && _loopParam.getValue() <= 0.0)
	) {
		_stage = ATTACK_STAGE;
		_stageProgress = 0.0;
	}
	else {
		switch (_stage) {
			case STOPPED_STAGE: {
				break;
			}
			case ATTACK_STAGE: {
				if (stepStage(_attackParam, _attackInput, slow, c)) {
					_stage = ON_STAGE;
					_stageProgress = 0.0;
				}
				break;
			}
			case ON_STAGE: {
				if (stepStage(_onParam, _onInput, slow, c)) {
					_stage = DECAY_STAGE;
					_stageProgress = 0.0;
				}
				break;
			}
			case DECAY_STAGE: {
				if (stepStage(_decayParam, _decayInput, slow, c)) {
					_stage = OFF_STAGE;
					_stageProgress = 0.0;
				}
				break;
			}
			case OFF_STAGE: {
				if (stepStage(_offParam, _offInput, slow, c)) {
					complete = true;
					if (_loopParam.getValue() <= 0.0 || _trigger.isHigh()) {
						_stage = ATTACK_STAGE;
						_stageProgress = 0.0;
					}
					else {
						_stage = STOPPED_STAGE;
					}
				}
				break;
			}
		}
	}

	float envelope = 0.0;
	switch (_stage) {
		case STOPPED_STAGE: {
			break;
		}
		case ATTACK_STAGE: {
			envelope = _stageProgress * 10.0;
			break;
		}
		case ON_STAGE: {
			envelope = 10.0;
			break;
		}
		case DECAY_STAGE: {
			envelope = (1.0 - _stageProgress) * 10.0;
			break;
		}
		case OFF_STAGE: {
			break;
		}
	}

	float signalLevel = levelParam(_signalParam, _signalCVInput, c);
	_signalOutput.setChannels(channels);
	_signalOutput.setVoltage(signalLevel * envelope * _signalInput.getPolyVoltage(c), c);

	float envLevel = levelParam(_envParam, _envInput, c);
	float envOutput = clamp(envLevel * envelope, 0.0f, 10.0f);
	_envOutput.setChannels(channels);
	_envOutput.setVoltage(envOutput, c);
	_invOutput.setChannels(channels);
	_invOutput.setVoltage(10.0 - envOutput, c);

	if (complete) {
		_triggerOuptutPulseGen.trigger(0.001);
	}
	_triggerOutput.setChannels(channels);
	_triggerOutput.setVoltage(_triggerOuptutPulseGen.process(APP->engine->getSampleTime()) ? 5.0 : 0.0, c);

	if (_attackOutput) {
		_attackOutput->setChannels(channels);
		_attackOutput->setVoltage(_stage == ATTACK_STAGE ? 5.0 : 0.0, c);
	}
	if (_onOutput) {
		_onOutput->setChannels(channels);
		_onOutput->setVoltage(_stage == ON_STAGE ? 5.0 : 0.0, c);
	}
	if (_decayOutput) {
		_decayOutput->setChannels(channels);
		_decayOutput->setVoltage(_stage == DECAY_STAGE ? 5.0 : 0.0, c);
	}
	if (_offOutput) {
		_offOutput->setChannels(channels);
		_offOutput->setVoltage(_stage == OFF_STAGE ? 5.0 : 0.0, c);
	}

	_attackLights[c] = _stage == ATTACK_STAGE;
	_onLights[c] = _stage == ON_STAGE;
	_decayLights[c] = _stage == DECAY_STAGE;
	_offLights[c] = _stage == OFF_STAGE;

	_firstStep = false;
}

bool ShaperCore::stepStage(Param& knob, Input* cv, bool slow, int c) {
	float t = levelParam(knob, cv, c);
	t = pow(t, 2);
	t = fmaxf(t, 0.001);
	t *= slow ? 100.0 : 10.0;
	_stageProgress += APP->engine->getSampleTime() / t;
	return _stageProgress > 1.0;
}

float ShaperCore::levelParam(Param& knob, Input* cv, int c) const {
	float v = clamp(knob.getValue(), 0.0f, 1.0f);
	if (cv && cv->isConnected()) {
		v *= clamp(cv->getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	return v;
}
