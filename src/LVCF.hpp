#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/multimode.hpp"

using namespace bogaudio::dsp;

extern Model* modelLVCF;

namespace bogaudio {

struct LVCF : BGModule {
	enum ParamsIds {
		FREQUENCY_PARAM,
		FREQUENCY_CV_PARAM,
		Q_PARAM,
		MODE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN_INPUT,
		FREQUENCY_CV_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LOWPASS_LIGHT,
		BANDPASS_LIGHT,
		HIGHPASS_LIGHT,
		BANDREJECT_LIGHT,
		NUM_LIGHTS
	};

	struct Engine {
		MultimodeFilter16 _filter;
		float _sampleRate;
		bogaudio::dsp::SlewLimiter _frequencySL;
		MultimodeFilter4 _finalHP;

		Engine() {
			sampleRateChange();
		}

		void setParams(
			int poles,
			MultimodeFilter::Mode mode,
			float frequency,
			float qbw,
			MultimodeFilter::BandwidthMode bwm
		);
		void sampleRateChange();
		void reset();
		float next(float sample);
	};

	static constexpr float maxFrequency = 20000.0f;
	static constexpr float minFrequency = MultimodeFilter::minFrequency;
	MultimodeFilter::Mode _mode = MultimodeFilter::UNKNOWN_MODE;
	int _polesSetting = 4;
	int _poles = 0;
	float _q = 0.0f;
	MultimodeFilter::BandwidthMode _bandwidthMode = MultimodeFilter::PITCH_BANDWIDTH_MODE;
	Engine* _engines[maxChannels];

	LVCF() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<ScaledSquaringParamQuantity<(int)maxFrequency>>(FREQUENCY_PARAM, 0.0f, 1.0f, 0.22361f, "Center/cutoff frequency", " HZ");
		configParam(FREQUENCY_CV_PARAM, -1.0f, 1.0f, 0.0f, "Frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(Q_PARAM, 0.0f, 1.0f, 0.0f, "Resonance / bandwidth", "%", 0.0f, 100.0f);
		configSwitch(MODE_PARAM, 0.0f, 3.0f, 0.0f, "Mode", {"Lowpass", "Highpass", "Bandpass", "Band reject"});
		configBypass(IN_INPUT, OUT_OUTPUT);

		configInput(IN_INPUT, "Signal");
		configInput(FREQUENCY_CV_INPUT, "Cutoff CV");

		configOutput(OUT_OUTPUT, "OUT");
	}

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	MultimodeFilter::Mode modeParamValue();
	void modulate() override;
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
