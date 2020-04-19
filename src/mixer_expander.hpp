#pragma once

#include "mixer.hpp"
#include "expanders.hpp"
#include "dsp/filters/equalizer.hpp"

namespace bogaudio {

template<int N>
struct MixerExpanderMessage : ExpanderMessage {
	bool active[N] {};
	float preFader[N] {};
	float postFader[N] {};
	float postEQ[N] {};
	float returnA[2] {};
	float returnB[2] {};
};

struct MixerExpanderChannel {
	Equalizer _eq;
	Amplifier _sendAAmp;
	Amplifier _sendBAmp;
	bogaudio::dsp::SlewLimiter _sendASL;
	bogaudio::dsp::SlewLimiter _sendBSL;

	Param& _lowParam;
	Param& _midParam;
	Param& _highParam;
	Param& _sendAParam;
	Param& _sendBParam;
	Param& _preAParam;
	Param& _preBParam;
	Input& _sendAInput;
	Input& _sendBInput;

	float postEQ = 0.0f;
	float sendA = 0.0f;
	float sendB = 0.0f;

	MixerExpanderChannel(
		Param& low,
		Param& mid,
		Param& high,
		Param& sendA,
		Param& sendB,
		Param& preA,
		Param& preB,
		Input& cvA,
		Input& cvB,
		float sampleRate = 1000.0f
	)
	: _lowParam(low)
	, _midParam(mid)
	, _highParam(high)
	, _sendAParam(sendA)
	, _sendBParam(sendB)
	, _preAParam(preA)
	, _preBParam(preB)
	, _sendAInput(cvA)
	, _sendBInput(cvB)
	{
		setSampleRate(sampleRate);
	}

	void setSampleRate(float sampleRate);
	float knobToDb(Param& p);
	void modulate();
	void next(float preFader, float postFader, bool sendAActive, bool sendBActive); // outputs on members postEQ, sendA, sendB
};

} // namespace bogaudio
