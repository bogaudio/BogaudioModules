#pragma once

#include "bogaudio.hpp"
#include "filters/multimode.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct PEQChannel {
	static const float maxDecibels;
	static const float minDecibels;
	static constexpr float maxFrequency = 20000.0f;
	static constexpr float minFrequency = MultimodeFilter::minFrequency;
	static const float maxFrequencySemitone;
	static const float minFrequencySemitone;

	float _sampleRate;
	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _levelSL;
	MultimodeFilter _filter;
	bogaudio::dsp::SlewLimiter _frequencySL;
	bogaudio::dsp::SlewLimiter _bandwidthSL;
	RootMeanSquare _rms;

	int _c;
	MultimodeFilter::Mode _mode;
	bool _fullFrequencyMode = true;
	int _poles;
	Param& _levelParam;
	Param& _frequencyParam;
	Param& _frequencyCv1Param;
	Param* _frequencyCv2Param;
	Param& _bandwidthParam;
	Input& _levelInput;
	Input& _frequency1Input;
	Input& _frequency2Input;
	Input* _bandwidthInput;

	float out = 0.0f;
	float rms = 0.0f;

	PEQChannel(
		int polyChannel,
		Param& levelParam,
		Param& frequencyParam,
		Param& frequencyCv1Param,
		Param* frequencyCv2Param,
		Param& bandwidthParam,
		Input& levelCvInput,
		Input& frequencyCv1Input,
		Input& frequencyCv2Input,
		Input* bandwidthCvInput,
		float sampleRate = 1000.0f
	)
	: _c(polyChannel)
	, _levelParam(levelParam)
	, _frequencyParam(frequencyParam)
	, _frequencyCv1Param(frequencyCv1Param)
	, _frequencyCv2Param(frequencyCv2Param)
	, _bandwidthParam(bandwidthParam)
	, _levelInput(levelCvInput)
	, _frequency1Input(frequencyCv1Input)
	, _frequency2Input(frequencyCv2Input)
	, _bandwidthInput(bandwidthCvInput)
	{
		setSampleRate(sampleRate);
		setFilterMode(MultimodeFilter::BANDPASS_MODE);
		_rms.setSensitivity(0.05f);
	}

	void setSampleRate(float sampleRate);
	void setFilterMode(MultimodeFilter::Mode mode);
	inline void setFrequencyMode(bool full) { _fullFrequencyMode = full; }
	void modulate();
	void next(float sample); // outputs on members out, rms.
};

struct PEQEngine {
	int _n;
	PEQChannel** _channels;
	Saturator _saturator;

	PEQEngine(int channels) : _n(channels) {
		_channels = new PEQChannel*[_n] {};
	}
	~PEQEngine() {
		for (int i = 0; i < _n; ++i) {
			delete _channels[i];
		}
		delete[] _channels;
	}

	inline void configChannel(
		int i,
		int polyChannel,
		Param& levelParam,
		Param& frequencyParam,
		Param& frequencyCv1Param,
		Param* frequencyCv2Param,
		Param& bandwidthParam,
		Input& levelCvInput,
		Input& frequencyCv1Input,
		Input& frequencyCv2Input,
		Input* bandwidthCvInput
	) {
		_channels[i] = new PEQChannel(
			polyChannel,
			levelParam,
			frequencyParam,
			frequencyCv1Param,
			frequencyCv2Param,
			bandwidthParam,
			levelCvInput,
			frequencyCv1Input,
			frequencyCv2Input,
			bandwidthCvInput
		);
	}
	inline void setLowFilterMode(MultimodeFilter::Mode mode) { _channels[0]->setFilterMode(mode); }
	inline void setHighFilterMode(MultimodeFilter::Mode mode) { _channels[_n - 1]->setFilterMode(mode); }
	void setFrequencyMode(bool full);
	void setSampleRate(float sr);
	void modulate();
	float next(float sample, float* outs, float* rmsSums);
};

} // namespace bogaudio
