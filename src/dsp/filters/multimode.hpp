#pragma once

#include <complex>

#include "filters/filter.hpp"

namespace bogaudio {
namespace dsp {

// hacky workaround for certain linkers here; see https://github.com/bogaudio/BogaudioModules/issues/104
#define BOGAUDIO_DSP_MULTIMODEFILTER_MINFREQUENCY 1.0f
#define BOGAUDIO_DSP_MULTIMODEFILTER_MAXFREQUENCY 21000.0f
#define BOGAUDIO_DSP_MULTIMODEFILTER_MINQBW 0.0f
#define BOGAUDIO_DSP_MULTIMODEFILTER_MAXQBW 1.0f
#define BOGAUDIO_DSP_MULTIMODEFILTER_MINBWLINEAR 10.0f
#define BOGAUDIO_DSP_MULTIMODEFILTER_MAXBWLINEAR 5000.0f
#define BOGAUDIO_DSP_MULTIMODEFILTER_MINBWPITCH (1.0f / (1.0f * 12.0f * 100.0f / 25.0f))
#define BOGAUDIO_DSP_MULTIMODEFILTER_MAXBWPITCH 2.0f

struct MultimodeFilter : Filter {
	typedef double T;
	typedef std::complex<T> TC;

	struct Pole {
		TC p;
		T x = 0.0;
		T y = 0.0;
		TC pc;
		TC p2;
		TC i2p;
		TC i2pc;
		T r = 0.0;

		Pole() {}
		Pole(T re, T im, T x, T y) : p(TC(re, im)), x(x), y(y) {
			pc = std::conj(p);
			p2 = p * p;
			i2p = (T)1.0 / ((T)2.0 * p);
			i2pc = (T)1.0 / ((T)2.0 * pc);
			r = std::abs(p);
		}
	};

	enum Type {
		UNKNOWN_TYPE,
		BUTTERWORTH_TYPE,
		CHEBYSHEV_TYPE
	};

	enum Mode {
		UNKNOWN_MODE,
		LOWPASS_MODE,
		HIGHPASS_MODE,
		BANDPASS_MODE,
		BANDREJECT_MODE
	};

	enum BandwidthMode {
		UNKNOWN_BANDWIDTH_MODE,
		LINEAR_BANDWIDTH_MODE,
		PITCH_BANDWIDTH_MODE
	};

	static constexpr int minPoles = 1;
	static constexpr int maxPoles = 16;
	static constexpr int modPoles = 1;
	const float minFrequency = BOGAUDIO_DSP_MULTIMODEFILTER_MINFREQUENCY;
	const float maxFrequency = BOGAUDIO_DSP_MULTIMODEFILTER_MAXFREQUENCY;
	const float minQbw = BOGAUDIO_DSP_MULTIMODEFILTER_MINQBW;
	const float maxQbw = BOGAUDIO_DSP_MULTIMODEFILTER_MAXQBW;
	const float minBWLinear = BOGAUDIO_DSP_MULTIMODEFILTER_MINBWLINEAR;
	const float maxBWLinear = BOGAUDIO_DSP_MULTIMODEFILTER_MAXBWLINEAR;
	const float minBWPitch = BOGAUDIO_DSP_MULTIMODEFILTER_MINBWPITCH;
	const float maxBWPitch = BOGAUDIO_DSP_MULTIMODEFILTER_MAXBWPITCH;

	float _sampleRate = 44100.0f;
	float _half2PiST = 0.0f;
	Type _type = UNKNOWN_TYPE;
	Mode _mode = UNKNOWN_MODE;
	int _nPoles = 0;
	float _frequency = -1.0f;
	float _qbw = -1.0f;
	BandwidthMode _bandwidthMode = UNKNOWN_BANDWIDTH_MODE;

	Pole _poles[maxPoles / 2];
	BiquadFilter<T> _filters[maxPoles] {};
	int _nFilters = 1;
	float _outGain = 1.0f;

	void setParams(
		float sampleRate,
		Type type,
		int poles,
		Mode mode,
		float frequency,
		float qbw,
		BandwidthMode bwm = PITCH_BANDWIDTH_MODE
	);
	float next(float sample) override;
	void reset();
};

} // namespace dsp
} // namespace bogaudio
