
#include <assert.h>
#include <math.h>
#include <algorithm>

#include "filters/resample.hpp"

using namespace bogaudio::dsp;

void LPFDecimator::setParams(float sampleRate, int factor) {
	_factor = factor;
	_filter.setParams(
		MultipoleFilter::LP_TYPE,
		4,
		factor * sampleRate,
		0.45f * sampleRate,
		0
	);
}

float LPFDecimator::next(const float* buf) {
	float s = 0.0f;
	for (int i = 0; i < _factor; ++i) {
		s = _filter.next(buf[i]);
	}
	return s;
}


// cascaded integrator–comb decimator: https://en.wikipedia.org/wiki/Cascaded_integrator%E2%80%93comb_filter
CICDecimator::CICDecimator(int stages, int factor) {
	assert(stages > 0);
	_stages = stages;
	_integrators = new T[_stages + 1] {};
	_combs = new T[_stages] {};
	setParams(0.0f, factor);
}

CICDecimator::~CICDecimator() {
	delete[] _integrators;
	delete[] _combs;
}

void CICDecimator::setParams(float _sampleRate, int factor) {
	assert(factor > 0);
	if (_factor != factor) {
		_factor = factor;
		_gainCorrection = 1.0f / (float)(pow(_factor, _stages));
	}
}

float CICDecimator::next(const float* buf) {
	for (int i = 0; i < _factor; ++i) {
		_integrators[0] = buf[i] * scale;
		for (int j = 1; j <= _stages; ++j) {
			_integrators[j] += _integrators[j - 1];
		}
	}
	T s = _integrators[_stages];
	for (int i = 0; i < _stages; ++i) {
		T t = s;
		s -= _combs[i];
		_combs[i] = t;
	}
	return _gainCorrection * (s / (float)scale);
}


CICInterpolator::CICInterpolator(int stages, int factor) {
	assert(stages > 0);
	_stages = stages;
	_integrators = new T[_stages + 1] {};
	_combs = new T[_stages] {};
	_buffer = NULL;
	setParams(0.0f, factor);
}

CICInterpolator::~CICInterpolator() {
	delete[] _integrators;
	delete[] _combs;
	delete[] _buffer;
}

void CICInterpolator::setParams(float _sampleRate, int factor) {
	assert(factor > 0);
	if (_factor != factor) {
		_factor = factor;
		_gainCorrection = 1.0f / 512.0f; // (float)(pow(_factor, _stages / 2));
		if (_buffer) {
			delete[] _buffer;
		}
		_buffer = new T[_factor] {};
	}
}

void CICInterpolator::next(float sample, float* buf) {
	T s = sample * scale;
	for (int i = 0; i < _stages; ++i) {
		T t = s;
		s -= _combs[i];
		_combs[i] = t;
	}

	std::fill(_buffer, _buffer + _factor, (T)0);
	_buffer[0] = s;
	for (int i = 0; i < _factor; ++i) {
		_integrators[0] = _buffer[i];
		for (int j = 1; j <= _stages; ++j) {
			_integrators[j] += _integrators[j - 1];
		}
		buf[i] = _gainCorrection * (_integrators[_stages] / (float)scale);
	}
}
