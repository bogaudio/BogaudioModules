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

	float _range = 0.0f;
	float _smooth = 0.25f;
	AnalyzerCore::Quality _quality = AnalyzerCore::QUALITY_GOOD;
	AnalyzerCore::Window _window = AnalyzerCore::WINDOW_KAISER;

	AnalyzerXL() : AnalyzerBase(8, NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {
		_skinnable = false;
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	void modulate() override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
