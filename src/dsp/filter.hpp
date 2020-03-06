#pragma once

#include <stdint.h>
#include <math.h>
#include <complex>

#include "buffer.hpp"
#include "signal.hpp"

namespace bogaudio {
namespace dsp {

struct Filter {
	Filter() {}
	virtual ~Filter() {}

	virtual float next(float sample) = 0;
};

template<typename T>
struct BiquadFilter : Filter {
	T _a0 = 0.0;
	T _a1 = 0.0;
	T _a2 = 0.0;
	T _b1 = 0.0;
	T _b2 = 0.0 ;

	T _x[3] {};
	T _y[3] {};

	void setParams(T a0, T a1, T a2, T b0, T b1, T b2) {
		T ib0 = 1.0 / b0;
		_a0 = a0 * ib0;
		_a1 = a1 * ib0;
		_a2 = a2 * ib0;
		_b1 = b1 * ib0;
		_b2 = b2 * ib0;
	}

	void reset() {
		_x[0] = _x[1] = _x[2] = 0.0;
		_y[0] = _y[1] = _y[2] = 0.0;
	}

	float next(float sample) override {
		_x[2] = _x[1];
		_x[1] = _x[0];
		_x[0] = sample;

		_y[2] = _y[1];
		_y[1] = _y[0];
		_y[0] = _a0 * _x[0];
		_y[0] += _a1 * _x[1];
		_y[0] += _a2 * _x[2];
		_y[0] -= _b1 * _y[1];
		_y[0] -= _b2 * _y[2];

		return _y[0];
	}
};

struct ComplexBiquadFilter : BiquadFilter<float> {
	float _gain = 1.0f;
	float _zeroRadius = 1.0f;
	float _zeroTheta = M_PI;
	float _poleRadius = 0.9f;
	float _poleTheta = 0.0f;

	ComplexBiquadFilter() {
		updateParams();
	}

	void setComplexParams(
		float gain,
		float zeroRadius,
		float zeroTheta,
		float poleRadius,
		float poleTheta
	);
	void updateParams();
};

struct LowPassFilter : Filter {
	float _sampleRate = 0.0f;
	float _cutoff = 0.0f;
	float _q = 0.0f;

	BiquadFilter<float> _biquad;

	LowPassFilter(float sampleRate = 1000.0f, float cutoff = 100.0f, float q = 0.001f) {
		setParams(sampleRate, cutoff, q);
	}

	void setParams(float sampleRate, float cutoff, float q = 0.001f);
	void reset() { _biquad.reset(); }
	float next(float sample) override {
		return _biquad.next(sample);
	}
};

struct DCBlocker : Filter {
	float _lastIn = 0.0f;
	float _lastOut = 0.0f;

	float next(float sample) override;
};

struct MultipoleFilter : Filter {
	enum Type {
		LP_TYPE,
		HP_TYPE
	};

	static constexpr int maxPoles = 20;
	static constexpr float maxRipple = 0.29f;
	Type _type = LP_TYPE;
	int _poles = 1;
	float _sampleRate = 0.0f;
	float _cutoff = 0.0f;
	float _ripple = 0.0f;
	BiquadFilter<double> _biquads[maxPoles / 2];

	MultipoleFilter() {}

	void setParams(
		Type type,
		int poles,
		float sampleRate,
		float cutoff,
		float ripple // FIXME: using this with more than two poles creates large gain, need compensation.
	);
	float next(float sample) override;
};

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
	static constexpr float minFrequency = 1.0f;
	static constexpr float maxFrequency = 21000.0f;
	static constexpr float minQbw = 0.0f;
	static constexpr float maxQbw = 1.0f;
	static constexpr float minBWLinear = 10.0f;
	static constexpr float maxBWLinear = 5000.0f;
	static constexpr float minBWPitch = 1.0f / (1.0f * 12.0f * 100.0f / 25.0f);
	static constexpr float maxBWPitch = 2.0f;

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

struct Equalizer : Filter {
	static constexpr float gainDb = 12.0f;
	static constexpr float cutDb = -36.0f;

	Amplifier _lowAmp;
	Amplifier _midAmp;
	Amplifier _highAmp;
	MultimodeFilter _lowFilter;
	MultimodeFilter _midFilter;
	MultimodeFilter _highFilter;

	void setParams(
		float sampleRate,
		float lowDb,
		float midDb,
		float highDb
	);
	float next(float sample) override;
};

struct Decimator {
	Decimator() {}
	virtual ~Decimator() {}

	virtual void setParams(float sampleRate, int factor) = 0;
	virtual float next(const float* buf) = 0;
};

struct LPFDecimator : Decimator {
	int _factor;
	MultipoleFilter _filter;

	LPFDecimator(float sampleRate = 1000.0f, int factor = 8) {
		setParams(sampleRate, factor);
	}
	void setParams(float sampleRate, int factor) override;
	float next(const float* buf) override;
};

struct CICDecimator : Decimator {
	typedef int64_t T;
	static constexpr T scale = ((T)1) << 32;
	int _stages;
	T* _integrators;
	T* _combs;
	int _factor = 0;
	float _gainCorrection;

	CICDecimator(int stages = 4, int factor = 8);
	virtual ~CICDecimator();

	void setParams(float sampleRate, int factor) override;
	float next(const float* buf) override;
};

struct Interpolator {
	Interpolator() {}
	virtual ~Interpolator() {}

	virtual void setParams(float sampleRate, int factor) = 0;
	virtual void next(float sample, float* buf) = 0;
};

struct CICInterpolator : Interpolator {
	typedef int64_t T;
	static constexpr T scale = ((T)1) << 32;
	int _stages;
	T* _integrators;
	T* _combs;
	T* _buffer;
	int _factor = 0;
	float _gainCorrection;

	CICInterpolator(int stages = 4, int factor = 8);
	virtual ~CICInterpolator();

	void setParams(float sampleRate, int factor) override;
	void next(float sample, float* buf) override;
};

} // namespace dsp
} // namespace bogaudio
