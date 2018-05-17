#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct MixerChannel {
	const float maxDecibels = 12.0f;
	const float minDecibels = Amplifier::minDecibels;
	const float slewTimeMS = 1.0f;

	Amplifier _amplifier;
	Panner _panner;
	SlewLimiter _slewLimiter;
	RootMeanSquare _rms;

	Param& _levelParam;
	Param& _panParam;
	Param& _muteParam;
	Input& _inInput;
	Input& _cvInput;

	float out = 0.0f;
	float left = 0.0f;
	float right = 0.0f;
	float rms = 0.0f;

	MixerChannel(
		Param& level,
		Param& pan,
		Param& mute,
		Input& in,
		Input& cv,
		float sampleRate = 1000.0f
	)
	: _levelParam(level)
	, _panParam(pan)
	, _muteParam(mute)
	, _inInput(in)
	, _cvInput(cv)
	{
		setSampleRate(sampleRate);
		_rms.setSensitivity(0.05f);
	}

	void setSampleRate(float sampleRate);
	void next(bool stereo); // input from _in; outputs on out, left, right, rms.
};

} // namespace bogaudio
