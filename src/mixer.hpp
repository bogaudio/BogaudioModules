#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

#define MIXER_PAN_SLEW_MS 10.0f

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
	void reset();
	void next(float sample, bool solo, int c = 0); // outputs on members out, rms.
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
