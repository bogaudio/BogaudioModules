#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/utility.hpp"
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
	bogaudio::dsp::SlewLimiter _levelCVSL;
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
	void next(float sample, bool solo, int c = 0, bool linearCV = false); // outputs on members out, rms.
};

struct LinearCVMixerModule : BGModule {
	bool _linearCV = false;

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
};

struct LinearCVMixerWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

struct DimmableMixerModule : LinearCVMixerModule {
	float _dimDb = 12.0f;

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
};

struct DimmableMixerWidget : LinearCVMixerWidget {
	void contextMenu(Menu* menu) override;
};

struct MuteButton : ToggleButton {
	bool _randomize = true;

	MuteButton() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1_orange.svg")));
	}

	inline void setRandomize(bool randomize) { _randomize = randomize; }
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
