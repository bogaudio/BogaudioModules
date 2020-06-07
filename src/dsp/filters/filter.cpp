
#include <math.h>

#include "filters/filter.hpp"

using namespace bogaudio::dsp;

// See: http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
void LowPassFilter::setParams(float sampleRate, float cutoff, float q) {
	if (_sampleRate == sampleRate && _cutoff == cutoff && _q == q) {
		return;
	}
	_sampleRate = sampleRate;
	_cutoff = cutoff;
	_q = q;
	// printf("\nLPF set param: sr=%f c=%f q=%f\n", _sampleRate, _cutoff, _q);

	double w0 = 2.0 * M_PI * (double)(_cutoff / _sampleRate);
	double cosw0 = cos(w0);
	double alpha = sin(w0) / (2.0 * _q);

	_biquad.setParams(
		(1.0 - cosw0) / 2.0,
		1.0 - cosw0,
		(1.0 - cosw0) / 2.0,
		1.0 + alpha,
		-2.0 * cosw0,
		1.0 - alpha
	);
}
