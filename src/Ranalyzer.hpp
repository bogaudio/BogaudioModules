#pragma once

#include "bogaudio.hpp"
#include "analyzer_base.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

extern Model* modelRanalyzer;

using namespace bogaudio::dsp;

namespace bogaudio {

struct ChannelDisplayListener {
	virtual void displayChannels(bool c0, bool c1, bool c2) = 0;
};

struct Ranalyzer : AnalyzerBase {
	enum ParamsIds {
		FREQUENCY1_PARAM,
		FREQUENCY2_PARAM,
		TRIGGER_PARAM,
		EXPONENTIAL_PARAM,
		LOOP_PARAM,
		DELAY_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TRIGGER_INPUT,
		RETURN_INPUT,
		TEST_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		TRIGGER_OUTPUT,
		EOC_OUTPUT,
		SEND_OUTPUT,
		NUM_OUTPUTS
	};

	enum Traces {
		ALL_TRACES,
		TEST_RETURN_TRACES,
		ANALYSIS_TRACES
	};

	enum WindowType {
		NONE_WINDOW_TYPE,
		TAPER_WINDOW_TYPE,
		HAMMING_WINDOW_TYPE,
		KAISER_WINDOW_TYPE
	};

	static constexpr float minFrequency = 1.0f;
	static constexpr float maxFrequencyNyquistRatio = 0.49f;
	static constexpr int maxResponseDelay = 66;
	static constexpr float initialDelaySeconds = 0.01f;

	struct FrequencyParamQuantity : ParamQuantity {
		float getDisplayValue() override {
			float v = getValue();
			if (!module) {
				return v;
			}

			float vv = v * v;
			vv *= roundf(APP->engine->getSampleRate() * Ranalyzer::maxFrequencyNyquistRatio) - Ranalyzer::minFrequency;
			vv += Ranalyzer::minFrequency;
			return vv;
		}

		void setDisplayValue(float displayValue) override {
			if (!module) {
				return;
			}
			displayValue -= Ranalyzer::minFrequency;
			displayValue = std::max(0.0f, displayValue);
			float v = displayValue / (roundf(APP->engine->getSampleRate() * Ranalyzer::maxFrequencyNyquistRatio) - Ranalyzer::minFrequency);
			v = powf(v, 0.5f);
			setValue(v);
		}
	};

	PureChirpOscillator _chirp;
	Trigger _trigger;
	rack::dsp::PulseGenerator _triggerPulseGen;
	rack::dsp::PulseGenerator _eocPulseGen;
	float _sampleRate = 0.0f;
	float _sampleTime = 0.0f;
	float _maxFrequency = 0.0f;
	bool _exponential = true;
	bool _loop = false;
	float _frequency1 = 0.0f;
	float _frequency2 = 0.0f;
	bool _run = false;
	bool _flush = false;
	int _returnSampleDelay = 2;
	int _currentReturnSampleDelay = 0;
	int _outBufferCount = 0;
	int _analysisBufferCount = 0;
	HistoryBuffer<float> _inputBuffer;
	int _cycleI = 0;
	int _cycleN = 0;
	bool _useTestInput = false;
	Traces _displayTraces = ALL_TRACES;
	ChannelDisplayListener* _channelDisplayListener = NULL;
	bool _triggerOnLoad = true;
	Timer* _initialDelay = NULL;
	WindowType _windowType = TAPER_WINDOW_TYPE;
	bogaudio::dsp::Window* _window = NULL;

	Ranalyzer()
	: AnalyzerBase(3, NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, 0, SpectrumAnalyzer::OVERLAP_1)
	, _inputBuffer(maxResponseDelay, 0.0f)
	{
		configParam<FrequencyParamQuantity>(FREQUENCY1_PARAM, 0.0f, 1.0f, 0.0f, "Frequency 1", " Hz");
		configParam<FrequencyParamQuantity>(FREQUENCY2_PARAM, 0.0f, 1.0f, 1.0f, "Frequency 2", " Hz");
		configButton(TRIGGER_PARAM, "Trigger");
		configSwitch(EXPONENTIAL_PARAM, 0.0f, 1.0f, 1.0f, "Sweep", {"Linear", "Exponential"});
		configSwitch(LOOP_PARAM, 0.0f, 1.0f, 0.0f, "Loop", {"Disabled", "Enabled"});
		configParam(DELAY_PARAM, 2.0f, (float)maxResponseDelay, 2.0f, "Return sample delay");
		paramQuantities[DELAY_PARAM]->snapEnabled = true;

		_skinnable = false;
	}
	virtual ~Ranalyzer() {
		if (_initialDelay) {
			delete _initialDelay;
		}
		if (_window) {
			delete _window;
		}
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
	void setDisplayTraces(Traces traces);
	void setChannelDisplayListener(ChannelDisplayListener* listener);
	void setWindow(WindowType wt);
};

} // namespace bogaudio
