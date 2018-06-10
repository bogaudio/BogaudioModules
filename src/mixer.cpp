
#include "mixer.hpp"

const float MixerChannel::maxDecibels = 6.0f;
const float MixerChannel::minDecibels = Amplifier::minDecibels;
const float MixerChannel::slewTimeMS = 10.0f;

void MixerChannel::setSampleRate(float sampleRate) {
	_slewLimiter.setParams(sampleRate, slewTimeMS);
	_rms.setSampleRate(sampleRate);
}

void MixerChannel::next(bool stereo) {
	if (!_inInput.active) {
		rms = out = left = right = 0.0f;
		return;
	}

	if (_muteParam.value > 0.5f) {
		_amplifier.setLevel(_slewLimiter.next(minDecibels));
	}
	else {
		float level = clamp(_levelParam.value, 0.0f, 1.0f);
		if (_levelInput.active) {
			level *= clamp(_levelInput.value / 10.0f, 0.0f, 1.0f);
		}
		level *= maxDecibels - minDecibels;
		level += minDecibels;
		_amplifier.setLevel(_slewLimiter.next(level));
	}

	out = _amplifier.next(_inInput.value);
	rms = _rms.next(out) / 5.0f;
	if (stereo) {
		float pan = clamp(_panParam.value, -1.0f, 1.0f);
		if (_panInput.active) {
			pan *= clamp(_panInput.value / 5.0f, -1.0f, 1.0f);
		}
		_panner.setPan(pan);
		_panner.next(out, left, right);
	}
}
