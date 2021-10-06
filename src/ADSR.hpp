#pragma once

#include "bogaudio.hpp"
#include "dsp/envelope.hpp"

extern Model* modelADSR;

namespace bogaudio {

struct ADSR : BGModule {
	enum ParamsIds {
		ATTACK_PARAM,
		DECAY_PARAM,
		SUSTAIN_PARAM,
		RELEASE_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		GATE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		ATTACK_LIGHT,
		DECAY_LIGHT,
		SUSTAIN_LIGHT,
		RELEASE_LIGHT,
		NUM_LIGHTS
	};

	struct Engine {
		Trigger gateTrigger;
		bogaudio::dsp::ADSR envelope;

		Engine() {
			reset();
			sampleRateChange();
			envelope.setSustain(0.0f);
			envelope.setRelease(0.0f);
		}
		void reset();
		void sampleRateChange();
	};
	Engine* _engines[maxChannels] {};
	bool _linearMode = false;
	int _attackLightSum;
	int _decayLightSum;
	int _sustainLightSum;
	int _releaseLightSum;
	float _invert = 1.0f;

	ADSR() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(ATTACK_PARAM, 0.0f, 1.0f, 0.141421f, "Attack", " s");
		configParam<EnvelopeSegmentParamQuantity>(DECAY_PARAM, 0.0f, 1.0f, 0.31623f, "Decay", " s");
		configParam(SUSTAIN_PARAM, 0.0f, 1.0f, 1.0f, "Sustain", "%", 0.0f, 100.0f);
		configParam<EnvelopeSegmentParamQuantity>(RELEASE_PARAM, 0.0f, 1.0f, 0.31623f, "Release", " s");
		configSwitch(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear", {"Disabled (logarithmic)", "Enabled"});
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcessAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
