#pragma once

#include "bogaudio.hpp"

extern Model* modelArp;

namespace bogaudio {

struct Arp : BGModule {
	enum ParamsIds {
		MODE_PARAM,
		GATE_LENGTH_PARAM,
		HOLD_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CLOCK_INPUT,
		RESET_INPUT,
		PITCH_INPUT,
		GATE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		PITCH_OUTPUT,
		GATE_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		UP_LIGHT,
		DOWN_LIGHT,
		UP_DOWN_LIGHT,
		UP_DOWN_REPEAT_LIGHT,
		IN_ORDER_LIGHT,
		RANDOM_LIGHT,
		NUM_LIGHTS
	};

	enum Mode {
		UP_MODE,
		DOWN_MODE,
		UP_DOWN_MODE,
		UP_DOWN_REPEAT_MODE,
		IN_ORDER_MODE,
		RANDOM_MODE
	};

	struct Note {
		float pitch;
		int channel;

		Note() {
			reset();
		}

		void reset();
	};

	Mode _mode = UP_MODE;
	float _gateLength = 0.5f;
	bool _hold = false;
	Trigger _clockTrigger;
	Trigger _resetTrigger;
	Trigger _gateTrigger[maxChannels];
	bool _anyHigh = false;
	bool _gateHigh[maxChannels] {};
	bool _noteOn[maxChannels] {};
	int _noteCount = 0;
	Note _notesAsPlayed[maxChannels];
	Note _notesByPitch[maxChannels];
	int _playIndex = -1;
	float _pitchOut = 0.0f;
	bool _up = true;
	float _sampleTime = 0.001f;
	float _secondsSinceLastClock = -1.0f;
	float _clockSeconds = 0.1f;
	rack::dsp::PulseGenerator _gateGenerator;

	Arp() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(MODE_PARAM, 0.0f, 5.0f, 0.0f, "Playback mode");
		configParam(GATE_LENGTH_PARAM, 0.0f, 1.0f, 0.5f, "Gate length", "%", 0.0f, 100.0f);
		configParam(HOLD_PARAM, 0.0f, 1.0f, 0.0f, "Hold/latch");
	}

	void reset() override;
	void sampleRateChange() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override {}
	void addNote(int c);
	void dropNote(int c);
	void dropAllNotes();
	void shuffleUp(Note* notes, int index);
	void shuffleDown(Note* notes, int index);
};

} // namespace bogaudio
