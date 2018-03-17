
#include <math.h>
#include <assert.h>

#include "filter.hpp"

using namespace bogaudio::dsp;

// See: http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

void BiquadFilter::setParams(float b0, float b1, float b2, float a0, float a1, float a2) {
	_b0 = b0 / a0;
	_b1 = b1 / a0;
	_b2 = b2 / a0;
	_a1 = a1 / a0;
	_a2 = a2 / a0;
	// printf("Biquad set params: b0=%f b1=%f b2=%f a1=%f a2=%f\n", _b0, _b1, _b2, _a1, _a2);
}

float BiquadFilter::next(float sample) {
	_x[2] = _x[1];
	_x[1] = _x[0];
	_x[0] = sample;

	_y[2] = _y[1];
	_y[1] = _y[0];
	_y[0] = _b0 * _x[0];
	_y[0] += _b1 * _x[1];
	_y[0] += _b2 * _x[2];
	_y[0] -= _a1 * _y[1];
	_y[0] -= _a2 * _y[2];

	return _y[0];
}


void ComplexBiquadFilter::setComplexParams(
	float gain,
	float zeroRadius,
	float zeroTheta,
	float poleRadius,
	float poleTheta
) {
	if (
		_gain != gain ||
		_zeroRadius != zeroRadius ||
		_zeroTheta != zeroTheta ||
		_poleRadius != poleRadius ||
		_poleTheta != poleTheta
	) {
		assert(gain >= 0.0f && gain <= 1.0f);
		assert(zeroRadius >= 0.0f && zeroRadius <= 1.0f);
		assert(zeroTheta >= 0.0f && zeroTheta <= 2.0f*M_PI);
		assert(poleRadius >= 0.0f && poleRadius <= 1.0f);
		assert(poleTheta >= 0.0f && poleTheta <= 2.0f*M_PI);
		_gain = gain;
		_zeroRadius = zeroRadius;
		_zeroTheta = zeroTheta;
		_poleRadius = poleRadius;
		_poleTheta = poleTheta;
		updateParams();
	}
}

void ComplexBiquadFilter::updateParams() {
	setParams(
		_gain,
		-2.0f * _zeroRadius * cosf(_zeroTheta) * _gain,
		_zeroRadius * _zeroRadius * _gain,
		1.0f,
		-2.0f * _poleRadius * cosf(_poleTheta),
		_poleRadius * _poleRadius
	);
}


void LowPassFilter::setParams(float sampleRate, float cutoff, float q) {
	if (_sampleRate == sampleRate && _cutoff == cutoff && _q == q) {
		return;
	}
	// printf("\nLPF set param: sr=%f c=%f q=%f\n", _sampleRate, _cutoff, _q);
	_sampleRate = sampleRate;
	_cutoff = cutoff;
	_q = q;

	float w0 = 2.0 * M_PI * _cutoff / _sampleRate;
	float cosw0 = cosf(w0);
	float alpha = sinf(w0) / (2.0 * _q);

	_biquad.setParams(
		(1.0 - cosw0) / 2.0,
		1.0 - cosw0,
		(1.0 - cosw0) / 2.0,
		1.0 + alpha,
		-2.0 * cosw0,
		1.0 - alpha
	);
}
