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

	struct NoteSet {
		struct Note {
			float pitch;
			int channel;

			Note() {
				reset();
			}

			void reset();
		};


		bool _noteOn[maxChannels] {};
		int _noteCount = 0;
		Note _notesAsPlayed[maxChannels];
		Note _notesByPitch[maxChannels];
		bool _notesDirty = false;
		int _playIndex = -1;
		bool _up = true;
		NoteSet* _syncTo;
		bool _syncNext = true;

		NoteSet(NoteSet* syncTo = NULL) : _syncTo(syncTo) {}

		inline int noteCount() { return _noteCount; }
		bool nextPitch(Mode mode, float& pitchOut);
		void reset();
		void resetSequence();
		void addNote(int c, float pitch);
		void dropNote(int c);
		void shuffleUp(Note* notes, int index);
		void shuffleDown(Note* notes, int index);
		void sync();
	};

	Mode _mode = UP_MODE;
	float _gateLength = 0.5f;
	bool _hold = false;
	bool _notesImmediate = false;
	Trigger _clockTrigger;
	Trigger _resetTrigger;
	Trigger _gateTrigger[maxChannels];
	bool _anyHigh = false;
	bool _gateHigh[maxChannels] {};
	NoteSet* _currentNotes;
	NoteSet* _playbackNotes;
	float _pitchOut = 0.0f;
	float _sampleTime = 0.001f;
	float _secondsSinceLastClock = -1.0f;
	float _clockSeconds = 0.1f;
	rack::dsp::PulseGenerator _gateGenerator;

	Arp() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(MODE_PARAM, 0.0f, 5.0f, 0.0f, "Playback mode");
		configParam(GATE_LENGTH_PARAM, 0.0f, 1.0f, 0.5f, "Gate length", "%", 0.0f, 100.0f);
		configParam(HOLD_PARAM, 0.0f, 1.0f, 0.0f, "Hold/latch");

		_currentNotes = new NoteSet();
		_playbackNotes = new NoteSet(_currentNotes);
	}
	virtual ~Arp() {
		delete _currentNotes;
		delete _playbackNotes;
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override {}
	void dropAllNotes();
};

} // namespace bogaudio
