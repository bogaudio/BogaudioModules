#pragma once

#include "bogaudio.hpp"
#include "analyzer_base.hpp"

extern Model* modelAnalyzer;

namespace bogaudio {

struct Analyzer : AnalyzerBase {
	enum ParamsIds {
		RANGE_PARAM, // no longer used
		SMOOTH_PARAM,
		QUALITY_PARAM,
		POWER_PARAM,  // no longer used
		WINDOW_PARAM,
		RANGE2_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		SIGNALA_INPUT,
		SIGNALB_INPUT,
		SIGNALC_INPUT,
		SIGNALD_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		SIGNALA_OUTPUT,
		SIGNALB_OUTPUT,
		SIGNALC_OUTPUT,
		SIGNALD_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		QUALITY_HIGH_LIGHT,
		QUALITY_GOOD_LIGHT,
		POWER_ON_LIGHT,
		QUALITY_ULTRA_LIGHT,
		WINDOW_NONE_LIGHT,
		WINDOW_HAMMING_LIGHT,
		WINDOW_KAISER_LIGHT,
		NUM_LIGHTS
	};

	float _sampleRate = 1000.0f;

	Analyzer() : AnalyzerBase(4, NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		configParam(RANGE2_PARAM, -1.0f, 1.0f, 0.0f, "Range");
		configParam(SMOOTH_PARAM, 0.0f, 1.0f, 0.5f, "Smoothing", " ms", 0.0f, 500.0f);
		configParam(QUALITY_PARAM, 1.0f, 3.0f, 1.0f, "Analysis quality");
		configParam(WINDOW_PARAM, 1.0f, 3.0f, 1.0f, "Analysis window type");
	}
	virtual ~Analyzer() {
		reset();
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
