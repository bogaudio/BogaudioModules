#pragma once

#include "bogaudio.hpp"
#include "expanders.hpp"
#include "dsp/filters/equalizer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

#define MIXER_PAN_SLEW_MS 10.0f

template<int N>
struct MixerExpanderMessage : ExpanderMessage {
	float preFader[N] {};
	float postFader[N] {};
	float postEQ[N] {};
	float returnA[2] {};
	float returnB[2] {};
};

struct MixerChannel {
	static const float maxDecibels;
	static const float minDecibels;
	static const float levelSlewTimeMS;

	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _levelSL;
	RootMeanSquare _rms;

	Param& _levelParam;
	Param& _muteParam;
	Input& _levelInput;
	Input* _muteInput;

	float out = 0.0f;
	float rms = 0.0f;

	MixerChannel(
		Param& level,
		Param& mute,
		Input& levelCv,
		float sampleRate = 1000.0f,
		Input* muteCv = NULL
	)
	: _levelParam(level)
	, _muteParam(mute)
	, _levelInput(levelCv)
	, _muteInput(muteCv)
	{
		setSampleRate(sampleRate);
		_rms.setSensitivity(0.05f);
	}

	void setSampleRate(float sampleRate);
	void next(float sample, bool solo, int c = 0); // outputs on members out, rms.
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
	void next(float preFader, float postFader); // outputs on members postEQ, sendA, sendB
};

struct MuteButton : ToggleButton {
	MuteButton() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1_orange.svg")));
	}

	void onButton(const event::Button& e) override;
};

struct SoloMuteButton : ParamWidget {
	std::vector<std::shared_ptr<Svg>> _frames;
	SvgWidget* _svgWidget; // deleted elsewhere.
	CircularShadow* shadow = NULL;

	SoloMuteButton();
	void onButton(const event::Button& e) override;
	void onChange(const event::Change& e) override;
};

} // namespace bogaudio
