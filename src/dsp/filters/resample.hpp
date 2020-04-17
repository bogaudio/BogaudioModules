#pragma once

#include "filters/filter.hpp"
#include "filters/experiments.hpp"

namespace bogaudio {
namespace dsp {

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
