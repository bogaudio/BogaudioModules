#pragma once

#include "buffer.hpp"
#include "signal.hpp"

namespace bogaudio {
namespace dsp {

struct Filter {
	virtual ~Filter() {}

	virtual float next(float sample) = 0;
};

struct ResetableFilter : Filter {
	virtual void reset() = 0;
};

template<typename T>
struct BiquadFilter : ResetableFilter {
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

	void reset() override {
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

struct LowPassFilter : ResetableFilter {
	float _sampleRate = 0.0f;
	float _cutoff = 0.0f;
	float _q = 0.0f;

	BiquadFilter<double> _biquad; // double is necessary here to make low cutoffs work at high sample rates.

	LowPassFilter(float sampleRate = 1000.0f, float cutoff = 100.0f, float q = 0.001f) {
		setParams(sampleRate, cutoff, q);
	}

	void setParams(float sampleRate, float cutoff, float q = 0.001f);
	void reset() override { _biquad.reset(); }
	float next(float sample) override {
		return _biquad.next(sample);
	}
};

} // namespace dsp
} // namespace bogaudio
