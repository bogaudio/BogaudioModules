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
	static constexpr float minFrequency = 100.0f; // MultimodeFilter::minFrequency;

	float _sampleRate;
	Amplifier _amplifier;
	bogaudio::dsp::SlewLimiter _levelSL;
	FourPoleMultimodeFilter _filter;
	bogaudio::dsp::SlewLimiter _frequencySL;
	bogaudio::dsp::SlewLimiter _bandwidthSL;
	RootMeanSquare _rms;

	int _c;
	MultimodeFilter::Mode _mode;
	int _poles;
	Param& _levelParam;
	Param& _frequencyParam;
	Param& _frequencyCvParam;
	Param& _bandwidthParam;
	Input& _levelInput;
	Input& _frequency1Input;
	Input& _frequency2Input;
	Input* _bandwidthInput;

	float out = 0.0f;
	float rms = 0.0f;

	PEQChannel(
		int polyChannel,
		Param& level,
		Param& frequency,
		Param& frequencyCv,
		Param& bandwidth,
		Input& levelCv,
		Input& frequencyCv1,
		Input& frequencyCv2,
		Input* bandwidthCv = NULL,
		float sampleRate = 1000.0f
	)
	: _c(polyChannel)
	, _levelParam(level)
	, _frequencyParam(frequency)
	, _frequencyCvParam(frequencyCv)
	, _bandwidthParam(bandwidth)
	, _levelInput(levelCv)
	, _frequency1Input(frequencyCv2)
	, _frequency2Input(frequencyCv1)
	, _bandwidthInput(bandwidthCv)
	{
		setSampleRate(sampleRate);
		setFilterMode(MultimodeFilter::BANDPASS_MODE);
		_rms.setSensitivity(0.05f);
	}

	void setSampleRate(float sampleRate);
	void setFilterMode(MultimodeFilter::Mode mode);
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
		Param& level,
		Param& frequency,
		Param& frequencyCv,
		Param& bandwidth,
		Input& levelCv,
		Input& frequencyCv1,
		Input& frequencyCv2,
		Input* bandwidthCv = NULL
	) {
		_channels[i] = new PEQChannel(
			polyChannel,
			level,
			frequency,
			frequencyCv,
			bandwidth,
			levelCv,
			frequencyCv1,
			frequencyCv2,
			bandwidthCv
		);
	}
	inline void setLowFilterMode(MultimodeFilter::Mode mode) { _channels[0]->setFilterMode(mode); }
	inline void setHighFilterMode(MultimodeFilter::Mode mode) { _channels[_n - 1]->setFilterMode(mode); }
	void setSampleRate(float sr);
	void modulate();
	float next(float sample, float* rmsSums);
};

} // namespace bogaudio
