#pragma once

#include "bogaudio.hpp"
#include "analyzer_base.hpp"

extern Model* modelAnalyzerXL;

namespace bogaudio {

struct AnalyzerXL : AnalyzerBase {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		SIGNALA_INPUT,
		SIGNALB_INPUT,
		SIGNALC_INPUT,
		SIGNALD_INPUT,
		SIGNALE_INPUT,
		SIGNALF_INPUT,
		SIGNALG_INPUT,
		SIGNALH_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	float _sampleRate = 1000.0f;
	float _smooth = 0.25f;
	AnalyzerCore::Quality _quality = AnalyzerCore::QUALITY_GOOD;
	AnalyzerCore::Window _window = AnalyzerCore::WINDOW_KAISER;

	AnalyzerXL() : AnalyzerBase(8, NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {
		_skinnable = false;
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
