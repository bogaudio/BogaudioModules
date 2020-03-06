
#include <assert.h>
#include <algorithm>

#include "filter.hpp"

using namespace bogaudio::dsp;

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


// See: http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
void LowPassFilter::setParams(float sampleRate, float cutoff, float q) {
	if (_sampleRate == sampleRate && _cutoff == cutoff && _q == q) {
		return;
	}
	_sampleRate = sampleRate;
	_cutoff = cutoff;
	_q = q;
	// printf("\nLPF set param: sr=%f c=%f q=%f\n", _sampleRate, _cutoff, _q);

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


float DCBlocker::next(float sample) {
	const float r = 0.999f;
	_lastOut = sample - _lastIn + r * _lastOut;
	_lastIn = sample;
	return _lastOut;
}


// Adapted from Smith 1997, "The Scientist and Engineer's Guide to DSP"
void MultipoleFilter::setParams(
	Type type,
	int poles,
	float sampleRate,
	float cutoff,
	float ripple
) {
	if (
		_type == type &&
		_poles == poles &&
		_sampleRate == sampleRate &&
		_cutoff == cutoff &&
		_ripple == ripple
	) {
		return;
	}
	assert(poles >= 1 && poles <= maxPoles);
	assert(poles % 2 == 0); // relax this later?
	assert(sampleRate >= 0.0f);
	assert(cutoff >= 0.0f && cutoff <= sampleRate / 2.0f);
	assert(ripple >= 0.0f && ripple <= maxRipple);
	_type = type;
	_poles = poles;
	_sampleRate = sampleRate;
	_cutoff = cutoff;
	_ripple = ripple;

	for (int p = 0, pn = _poles / 2; p < pn; ++p) {
		double a0 = 0.0;
		double a1 = 0.0;
		double a2 = 0.0;
		double b1 = 0.0;
		double b2 = 0.0;
		{
			double angle = M_PI/(_poles*2.0) + p*M_PI/_poles;
			double rp = -cos(angle);
			double ip = sin(angle);

			if (_ripple > 0.01f) {
				double es = sqrt(pow(1.0 / (1.0 - _ripple), 2.0) - 1.0);
				double esi = 1.0 / es;
				double esis = esi * esi;
				double polesi = 1.0 / (float)_poles;
				double vx = polesi * log(esi + sqrt(esis + 1.0));
				double kx = polesi * log(esi + sqrt(esis - 1.0));
				kx = (exp(kx) + exp(-kx)) / 2.0;
				rp *= ((exp(vx) - exp(-vx)) / 2.0) / kx;
				ip *= ((exp(vx) + exp(-vx)) / 2.0) / kx;
				// printf("\n\n\ncheb p=%d pn=%d rip=%f rp=%f ip=%f es=%f vx=%f kx=%f\n", p, pn, _ripple, rp, ip, es, vx, kx);
			}

			const double t = 2.0 * tan(0.5);
			const double ts = t * t;
			// printf("\n\nco=%f sr=%f fc=%f\n", _cutoff, _sampleRate, _cutoff / _sampleRate);
			double m = rp*rp + ip*ip;
			double mts = m * ts;
			double d = 4.0 - 4.0*rp*t + mts;
			double x0 = ts / d;
			double x1 = 2.0 * x0;
			double x2 = x0;
			double y1 = (8.0 - 2.0*mts) / d;
			double y2 = (-4.0 - 4.0*rp*t - mts) / d;
			// printf("vs p=%d t=%f w=%f m=%f d=%f x0=%f x1=%f x2=%f y1=%f y2=%f\n", p, t, w, m, d, x0, x1, x2, y1, y2);

			// FIXME: optimization: everything above here only depends on type, poles and ripple.

			double w = 2.0 * M_PI * (_cutoff / _sampleRate);
			double w2 = w / 2.0;
			double k = 0.0;
			switch (_type) {
				case LP_TYPE: {
					k = sin(0.5 - w2) / sin(0.5 + w2);
					break;
				}
				case HP_TYPE: {
					k = -cos(w2 + 0.5) / cos(w2 - 0.5);
					break;
				}
			}
			double ks = k * k;
			d = 1.0 + y1*k - y2*ks;
			a0 = (x0 - x1*k + x2*ks) / d;
			a1 = (-2.0*x0*k + x1 + x1*ks - 2.0*x2*k) / d;
			a2 = (x0*ks - x1*k + x2) / d;
			b1 = (2.0*k + y1 + y1*ks - 2.0*y2*k) / d;
			b2 = (-ks - y1*k + y2) / d;
			if (_type == HP_TYPE) {
				a1 = -a1;
				b1 = -b1;
			}

			// printf("pole %d: rp=%f ip=%f a0=%f a1=%f a2=%f b1=%f b2=%f\n\n\n", p, rp, ip, a0, a1, a2, b1, b2);
			_biquads[p].setParams(a0, a1, a2, 1.0, -b1, -b2);
		}
	}
}

float MultipoleFilter::next(float sample) {
	for (int p = 0, pn = _poles / 2; p < pn; ++p) {
		sample = _biquads[p].next(sample);
	}
	return sample;
}


void MultimodeFilter::setParams(
	float sampleRate,
	Type type,
	int poles,
	Mode mode,
	float frequency,
	float qbw,
	BandwidthMode bwm
) {
	assert(poles >= minPoles && poles <= maxPoles);
	assert(poles % modPoles == 0);
	assert(frequency >= minFrequency && frequency <= maxFrequency);
	assert(qbw >= minQbw && qbw <= maxQbw);

	bool repole = _type != type || _mode != mode || _nPoles != poles || (type == CHEBYSHEV_TYPE && (mode == LOWPASS_MODE || mode == HIGHPASS_MODE) && _qbw != qbw);
	bool redesign = repole || _frequency != frequency || _qbw != qbw || _sampleRate != sampleRate || _bandwidthMode != bwm;
	_sampleRate = sampleRate;
	_half2PiST = M_PI * (1.0f / sampleRate);
	_type = type;
	_nPoles = poles;
	_mode = mode;
	_frequency = frequency;
	_qbw = qbw;
	_bandwidthMode = bwm;

	if (repole) {
		switch (_type) {
			case BUTTERWORTH_TYPE: {
				int np = _nPoles / 2 + (_nPoles % 2 == 1);
				for (int k = 1, j = np - 1; k <= np; ++k, --j) {
					T a = (T)(2 * k + _nPoles - 1) * M_PI / (T)(2 * _nPoles);
					T re = std::cos(a);
					T im = std::sin(a);
					_poles[j] = Pole(-re, im, re + re, re * re + im * im);
				}

				_outGain = 1.0f;
				break;
			}

			case CHEBYSHEV_TYPE: {
				T ripple = 3.0;
				if (mode == LOWPASS_MODE || mode == HIGHPASS_MODE) {
					ripple += std::max(0.0f, 6.0f * qbw);
				}
				T e = ripple / (T)10.0;
				e = std::pow((T)10.0, e);
				e -= (T)1.0;
				e = std::sqrt(e);
				T ef = std::asinh((T)1.0 / e) / (float)_nPoles;
				T efr = -std::sinh(ef);
				T efi = std::cosh(ef);

				int np = _nPoles / 2 + (_nPoles % 2 == 1);
				for (int k = 1, j = np - 1; k <= np; ++k, --j) {
					T a = (T)(2 * k - 1) * M_PI / (T)(2 * _nPoles);
					T re = efr * std::sin(a);
					T im = efi * std::cos(a);
					_poles[j] = Pole(-re, im, re + re, re * re + im * im);
				}

				_outGain = 1.0 / (e * std::pow(2.0, (T)(_nPoles - 1)));
				// _outGain = 1.0f / std::pow(2.0f, (T)(_nPoles - 1));
				break;
			}

			default: {
				assert(false);
			}
		}
	}

	if (redesign) {
		switch (_mode) {
			case LOWPASS_MODE:
			case HIGHPASS_MODE: {
				int nf = _nPoles / 2 + _nPoles % 2;
				for (int i = _nFilters; i < nf; ++i) {
					_filters[i].reset();
				}
				_nFilters = nf;

				// T iq = (1.0 / std::sqrt(2.0)) - 0.65 * _qbw;
				T iq = (T)0.8 - (T)0.6 * _qbw;
				T wa = std::tan(_frequency * _half2PiST);
				T wa2 = wa * wa;

				if (_mode == LOWPASS_MODE) {
					int ni = 0;
					int nf = _nFilters;
					if (_nPoles % 2 == 1) {
						++ni;
						--nf;
						T wap = wa * std::real(_poles[0].p);
						_filters[0].setParams(wa, wa, 0.0, wap + (T)1.0, wap - (T)1.0, (T)0.0);
					}
					T a0 = wa2;
					T a1 = wa2 + wa2;
					T a2 = wa2;
					for (int i = 0; i < nf; ++i) {
						Pole& pole = _poles[ni + i];
						T ywa2 = pole.y * wa2;
						T ywa21 = ywa2 + (T)1.0;
						T x = (((T)(i == nf / 2) * (iq - (T)1.0)) + (T)1.0) * pole.x;
						T xwa = x * wa;
						T b0 = ywa21 - xwa;
						T b1 = (T)-2.0 + (ywa2 + ywa2);
						T b2 = ywa21 + xwa;
						_filters[ni + i].setParams(a0, a1, a2, b0, b1, b2);
					}
				}
				else {
					int ni = 0;
					int nf = _nFilters;
					if (_nPoles % 2 == 1) {
						++ni;
						--nf;
						T rp = std::real(_poles[0].p);
						_filters[0].setParams(1.0, -1.0, 0.0, wa + rp, wa - rp, 0.0);
					}
					T a0 = 1.0;
					T a1 = -2.0f;
					T a2 = 1.0;
					for (int i = 0; i < nf; ++i) {
						Pole& pole = _poles[ni + i];
						T wa2y = wa2 + pole.y;
						T x = (((T)(i == nf / 2) * (iq - (T)1.0)) + (T)1.0) * pole.x;
						T xwa = x * wa;
						T b0 = wa2y - xwa;
						T b1 = (wa2 + wa2) - (pole.y + pole.y);
						T b2 = wa2y + xwa;
						_filters[ni + i].setParams(a0, a1, a2, b0, b1, b2);
					}
				}
				break;
			}

			case BANDPASS_MODE:
			case BANDREJECT_MODE: {
				int nf = ((_nPoles / 2) * 2) + (_nPoles % 2);
				for (int i = _nFilters; i < nf; ++i) {
					_filters[i].reset();
				}
				_nFilters = nf;

				T wdl = 0.0;
				T wdh = 0.0;
				switch (_bandwidthMode) {
					case LINEAR_BANDWIDTH_MODE: {
						float bandwidth = std::max(minBWLinear, maxBWLinear * _qbw);
						wdl = std::max(minFrequency, _frequency - 0.5f * bandwidth);
						wdh = std::min(maxFrequency, std::max((float)wdl + 10.0f, _frequency + 0.5f * bandwidth));
						break;
					}
					case PITCH_BANDWIDTH_MODE: {
						float bandwidth = std::max(minBWPitch, maxBWPitch * _qbw);
						wdl = std::max(minFrequency, powf(2.0f, -bandwidth) * _frequency);
						wdh = std::min(maxFrequency, std::max((float)wdl + 10.0f, powf(2.0f, bandwidth) * _frequency));
						break;
					}
					default: {
						assert(false);
					}
				}
				T wal = std::tan(wdl * _half2PiST);
				T wah = std::tan(wdh * _half2PiST);
				T w = wah - wal;
				T w2 = w * w;
				T w02 = wah * wal;

				if (_mode == BANDPASS_MODE) {
					T a0 = w;
					T a1 = 0.0;
					T a2 = -w;

					int ni = 0;
					int nf = _nFilters;
					if (_nPoles % 2 == 1) {
						++ni;
						--nf;
						T wp = w * std::real(_poles[0].p);
						_filters[0].setParams(
							a0,
							a1,
							a2,
							(T)1.0 + wp + w02,
							(T)-2.0 + (w02 + w02),
							(T)1.0 - wp + w02
						);
					}
					for (int i = 0; i < nf; i += 2) {
						Pole& pole = _poles[ni + i / 2];
						TC x = pole.p2;
						x *= w2;
						x -= (T)4.0 * w02;
						x = std::sqrt(x);
						TC xc = std::conj(x);
						TC wp = w * pole.p;
						TC wpc = w * pole.pc;
						TC y1 = (x - wp) * (T)0.5;
						TC y1c = (xc - wpc) * (T)0.5;
						TC y2 = (-x - wp) * (T)0.5;
						TC y2c = (-xc - wpc) * (T)0.5;
						TC cf1a = -(y1 + y1c);
						TC cf2a = y1 * y1c;
						TC cf1b = -(y2 + y2c);
						TC cf2b = y2 * y2c;
						T f1a = std::real(cf1a);
						T f1b = std::real(cf1b);
						T f2a = std::real(cf2a);
						T f2b = std::real(cf2b);

						{
							T b0 = (T)1.0 + f1a + f2a;
							T b1 = (T)-2.0 + (f2a + f2a);
							T b2 = (T)1.0 - f1a + f2a;
							_filters[ni + i].setParams(a0, a1, a2, b0, b1, b2);
						}
						{
							T b0 = (T)1.0 + f1b + f2b;
							T b1 = (T)-2.0 + (f2b + f2b);
							T b2 = (T)1.0 - f1b + f2b;
							_filters[ni + i + 1].setParams(a0, a1, a2, b0, b1, b2);
						}
					}
				}
				else {
					T a0 = (T)1.0 + w02;
					T a1 = (T)-2.0 + (w02 + w02);
					T a2 = a0;

					int ni = 0;
					int nf = _nFilters;
					if (_nPoles % 2 == 1) {
						++ni;
						--nf;
						T rp = std::real(_poles[0].p);
						T rpw02 = rp * w02;
						_filters[0].setParams(
							a0,
							a1,
							a2,
							rp + w + rpw02,
							(T)-2.0 * rp + (rpw02 + rpw02),
							rp - w + rpw02
						);
					}
					for (int i = 0; i < nf; i += 2) {
						Pole& pole = _poles[ni + i / 2];
						TC x = pole.p2;
						x *= (T)-4.0 * w02;
						x += w2;
						x = std::sqrt(x);
						TC xc = std::conj(x);
						TC y1 = (x - w) * pole.i2p;
						TC y1c = (xc - w) * pole.i2pc;
						TC y2 = (-x - w) * pole.i2p;
						TC y2c = (-xc - w) * pole.i2pc;
						TC cf1a = -pole.r * (y1 + y1c);
						TC cf2a = pole.r * y1 * y1c;
						TC cf1b = -pole.r * (y2 + y2c);
						TC cf2b = pole.r * y2 * y2c;
						T f1a = std::real(cf1a);
						T f1b = std::real(cf1b);
						T f2a = std::real(cf2a);
						T f2b = std::real(cf2b);

						{
							T b0 = pole.r + f1a + f2a;
							T b1 = (T)-2.0 * pole.r + (f2a + f2a);
							T b2 = pole.r - f1a + f2a;
							_filters[ni + i].setParams(a0, a1, a2, b0, b1, b2);
						}
						{
							T b0 = pole.r + f1b + f2b;
							T b1 = (T)-2.0 * pole.r + (f2b + f2b);
							T b2 = pole.r - f1b + f2b;
							_filters[ni + i + 1].setParams(a0, a1, a2, b0, b1, b2);
						}
					}
				}
				break;
			}

			default: {
				assert(false);
			}
		}
	}
}

float MultimodeFilter::next(float sample) {
	for (int i = 0; i < _nFilters; ++i) {
		sample = _filters[i].next(sample);
	}
	return _outGain * sample;
}

void MultimodeFilter::reset() {
	for (int i = 0; i < _nFilters; ++i) {
		_filters[i].reset();
	}
}


void Equalizer::setParams(
	float sampleRate,
	float lowDb,
	float midDb,
	float highDb
) {
	assert(lowDb >= cutDb && lowDb <= gainDb);
	assert(midDb >= cutDb && midDb <= gainDb);
	assert(highDb >= cutDb && highDb <= gainDb);

	_lowAmp.setLevel(lowDb);
	_lowFilter.setParams(
		sampleRate,
		MultimodeFilter::BUTTERWORTH_TYPE,
		4,
		MultimodeFilter::LOWPASS_MODE,
		100.0f,
		0.0f
	);

	_midAmp.setLevel(midDb);
	_midFilter.setParams(
		sampleRate,
		MultimodeFilter::BUTTERWORTH_TYPE,
		2,
		MultimodeFilter::BANDPASS_MODE,
		350.0f,
		0.55f,
		MultimodeFilter::PITCH_BANDWIDTH_MODE
	);

	_highAmp.setLevel(highDb);
	_highFilter.setParams(
		sampleRate,
		MultimodeFilter::BUTTERWORTH_TYPE,
		4,
		MultimodeFilter::HIGHPASS_MODE,
		1000.0f,
		0.0f
	);
}

float Equalizer::next(float sample) {
	float low = _lowAmp.next(_lowFilter.next(sample));
	float mid = _midAmp.next(_midFilter.next(sample));
	float high = _highAmp.next(_highFilter.next(sample));
	return low + mid + high;
}


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
