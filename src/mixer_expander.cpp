
#include "mixer_expander.hpp"

void MixerExpanderChannel::setSampleRate(float sampleRate) {
	_sendASL.setParams(sampleRate, MixerChannel::levelSlewTimeMS, MixerChannel::maxDecibels - MixerChannel::minDecibels);
	_sendBSL.setParams(sampleRate, MixerChannel::levelSlewTimeMS, MixerChannel::maxDecibels - MixerChannel::minDecibels);
}

float MixerExpanderChannel::knobToDb(Param& p) {
	float v = clamp(p.getValue(), -1.0f, 1.0f);
	if (v < 0.0f) {
		return -v * Equalizer::cutDb;
	}
	return v * Equalizer::gainDb;
}

void MixerExpanderChannel::modulate() {
	_eq.setParams(
		APP->engine->getSampleRate(),
		knobToDb(_lowParam),
		knobToDb(_midParam),
		knobToDb(_highParam)
	);
}

void MixerExpanderChannel::next(float preFader, float postFader, bool sendAActive, bool sendBActive) {
	postEQ = _eq.next(postFader);

	if (sendAActive) {
		float level = clamp(_sendAParam.getValue(), 0.0f, 1.0f);
		if (_sendAInput.isConnected()) {
			level *= clamp(_sendAInput.getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		level = 1.0f - level;
		level *= Amplifier::minDecibels;
		_sendAAmp.setLevel(_sendASL.next(level));
		sendA = _sendAAmp.next(_preAParam.getValue() > 0.5f ? preFader : postEQ);
	}
	else {
		sendA = 0.0f;
	}

	if (sendBActive) {
		float level = clamp(_sendBParam.getValue(), 0.0f, 1.0f);
		if (_sendBInput.isConnected()) {
			level *= clamp(_sendBInput.getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		level = 1.0f - level;
		level *= Amplifier::minDecibels;
		_sendBAmp.setLevel(_sendBSL.next(level));
		sendB = _sendBAmp.next(_preBParam.getValue() > 0.5f ? preFader : postEQ);
	}
	else {
		sendB = 0.0f;
	}
}
