#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/multimode.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVCF;

namespace bogaudio {

struct VCF : BGModule {
	enum ParamsIds {
		FREQUENCY_PARAM,
		FREQUENCY_CV_PARAM,
		FM_PARAM,
		Q_PARAM,
		MODE_PARAM,
		SLOPE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		FREQUENCY_CV_INPUT,
		FM_INPUT,
		PITCH_INPUT,
		IN_INPUT,
		Q_INPUT,
		SLOPE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		static constexpr int maxPoles = 12;
		static constexpr int minPoles = 1;
		static constexpr int nFilters = maxPoles;
		MultimodeFilter _filters[nFilters];
		float _gains[nFilters] {};
		bogaudio::dsp::SlewLimiter _gainSLs[nFilters];
		float _sampleRate;
		bogaudio::dsp::SlewLimiter _frequencySL;
		MultimodeFilter _finalHP;

		Engine() {
			sampleRateChange();
		}

		void setParams(
			float slope,
			MultimodeFilter::Mode mode,
			float frequency,
			float qbw,
			MultimodeFilter::BandwidthMode bwm
		);
		void reset();
		void sampleRateChange();
		float next(float sample);
	};

	static constexpr float maxFrequency = 20000.0f;
	static constexpr float minFrequency = MultimodeFilter::minFrequency;
	MultimodeFilter::Mode _mode = MultimodeFilter::UNKNOWN_MODE;
	MultimodeFilter::BandwidthMode _bandwidthMode = MultimodeFilter::PITCH_BANDWIDTH_MODE;
	Engine* _engines[maxChannels] {};

	VCF() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<ScaledSquaringParamQuantity<(int)maxFrequency>>(FREQUENCY_PARAM, 0.0f, 1.0f, 0.22361f, "Center/cutoff frequency", " HZ");
		configParam(FREQUENCY_CV_PARAM, -1.0f, 1.0f, 0.0f, "Frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(FM_PARAM, 0.0f, 1.0f, 0.0f, "FM", "%", 0.0f, 100.0f);
		configParam(Q_PARAM, 0.0f, 1.0f, 0.0f, "Resonance / bandwidth", "%", 0.0f, 100.0f);
		configParam(MODE_PARAM, 0.0f, 3.0f, 0.0f, "Mode");
		configParam<ScaledSquaringParamQuantity<Engine::maxPoles - Engine::minPoles>>(SLOPE_PARAM, 0.0f, 1.0f, 0.522233f, "Slope", " poles", 0.0f, 1.0f, Engine::minPoles);
	}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
