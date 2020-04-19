
#include <assert.h>
#include <math.h>
#include <algorithm>

#include "filters/multimode.hpp"

using namespace bogaudio::dsp;

#ifdef RACK_SIMD

void Biquad4::setParams(int i, float a0, float a1, float a2, float b0, float b1, float b2) {
	assert(i >= 0 && i < 4);
	float ib0 = 1.0 / b0;
	_a0[i] = a0 * ib0;
	_a1[i] = a1 * ib0;
	_a2[i] = a2 * ib0;
	_b1[i] = b1 * ib0;
	_b2[i] = b2 * ib0;
}

void Biquad4::reset() {
	_x[0] = _x[1] = _x[2] = 0.0;
	_y[0] = _y[1] = _y[2] = 0.0;
}

void Biquad4::setN(int n) {
	assert(n <= 4);
	for (; n < 4; ++n) {
		setParams(n, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
	}
}

float Biquad4::next(float sample) {
	if (_disable) {
		return sample;
	}

	_x[2] = _x[1];
	_x[1] = _x[0];

	// slower: _x[0] = _mm_shuffle_ps(_y[0].v, _y[0].v, _MM_SHUFFLE(2, 1, 0, 0)); _x[0][0] = sample;
	_x[0] = float_4(sample, _y[0][0], _y[0][1], _y[0][2]);

	_y[2] = _y[1];
	_y[1] = _y[0];
	_y[0] = ((_a0 * _x[0]) + (_a1 * _x[1]) + (_a2 * _x[2])) - ((_b1 * _y[1]) + (_b2 * _y[2]));
	return _y[0][3];
}

template<> void BiquadBank<MultimodeTypes::T, 4>::setParams(int i, MultimodeTypes::T a0, MultimodeTypes::T a1, MultimodeTypes::T a2, MultimodeTypes::T b0, MultimodeTypes::T b1, MultimodeTypes::T b2) {
	assert(i >= 0 && i < 4);
	_biquads->setParams(i, a0, a1, a2, b0, b1, b2);
}

template<> void BiquadBank<MultimodeTypes::T, 4>::reset() {
	_biquads->reset();
}

template<> void BiquadBank<MultimodeTypes::T, 4>::setN(int n) {
	for (; n < 4; ++n) {
		_biquads->setParams(n, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
	}
}

template<> float BiquadBank<MultimodeTypes::T, 4>::next(float sample) {
	return _biquads->next(sample);
}

template<> void BiquadBank<MultimodeTypes::T, 16>::setParams(int i, MultimodeTypes::T a0, MultimodeTypes::T a1, MultimodeTypes::T a2, MultimodeTypes::T b0, MultimodeTypes::T b1, MultimodeTypes::T b2) {
	assert(i >= 0 && i < 16);
	_biquads[i / 4].setParams(i % 4, a0, a1, a2, b0, b1, b2);
}

template<> void BiquadBank<MultimodeTypes::T, 16>::reset() {
	_biquads[0].reset();
	_biquads[1].reset();
	_biquads[2].reset();
	_biquads[3].reset();
}

template<> void BiquadBank<MultimodeTypes::T, 16>::setN(int n) {
	for (int i = n, nn = n + (4 - n % 4); i < nn; ++i) {
		_biquads[i / 4].setParams(i % 4, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
	}
	for (int i = 0; i < 4; ++i) {
		_biquads[i].disable(4 * i > n);
	}
}

template<> float BiquadBank<MultimodeTypes::T, 16>::next(float sample) {
	sample = _biquads[0].next(sample);
	sample = _biquads[1].next(sample);
	sample = _biquads[2].next(sample);
	return _biquads[3].next(sample);
}

#else

template<typename T, int N> void BiquadBank<T, N>::setParams(int i, T a0, T a1, T a2, T b0, T b1, T b2) {
	assert(i >= 0 && i < N);
	_biquads[i].setParams(a0, a1, a2, b0, b1, b2);
}

template<typename T, int N> void BiquadBank<T, N>::reset() {
	for (int i = 0; i < N; ++i) {
		_biquads[i].reset();
	}
}

template<typename T, int N> void BiquadBank<T, N>::setN(int n) {
	assert(n <= N);
	_n = n;
	for (; n < N; ++n) {
		_biquads[n].reset();
	}
}

template<typename T, int N> float BiquadBank<T, N>::next(float sample) {
	for (int i = 0; i < _n; ++i) {
		sample = _biquads[i].next(sample);
	}
	return sample;
}

#endif

template struct bogaudio::dsp::BiquadBank<MultimodeTypes::T, 4>;
template struct bogaudio::dsp::BiquadBank<MultimodeTypes::T, 16>;


template<int N> void MultimodeDesigner<N>::setParams(
	BiquadBank<T, N>& biquads,
	float& outGain,
	float sampleRate,
	Type type,
	int poles,
	Mode mode,
	float frequency,
	float qbw,
	BandwidthMode bwm
) {
	assert(N >= minPoles && N <= maxPoles);
	assert(poles >= minPoles && poles <= N);
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

				outGain = 1.0f;
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

				outGain = 1.0 / (e * std::pow(2.0, (T)(_nPoles - 1)));
				// outGain = 1.0f / std::pow(2.0f, (T)(_nPoles - 1));
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
				_nBiquads = _nPoles / 2 + _nPoles % 2;
				biquads.setN(_nBiquads);

				// T iq = (1.0 / std::sqrt(2.0)) - 0.65 * _qbw;
				T iq = (T)0.8 - (T)0.6 * _qbw;
				T wa = std::tan(_frequency * _half2PiST);
				T wa2 = wa * wa;

				if (_mode == LOWPASS_MODE) {
					int ni = 0;
					int nb = _nBiquads;
					if (_nPoles % 2 == 1) {
						++ni;
						--nb;
						T wap = wa * std::real(_poles[0].p);
						biquads.setParams(0, wa, wa, 0.0, wap + (T)1.0, wap - (T)1.0, (T)0.0);
					}
					T a0 = wa2;
					T a1 = wa2 + wa2;
					T a2 = wa2;
					for (int i = 0; i < nb; ++i) {
						Pole& pole = _poles[ni + i];
						T ywa2 = pole.y * wa2;
						T ywa21 = ywa2 + (T)1.0;
						T x = (((T)(i == nb / 2) * (iq - (T)1.0)) + (T)1.0) * pole.x;
						T xwa = x * wa;
						T b0 = ywa21 - xwa;
						T b1 = (T)-2.0 + (ywa2 + ywa2);
						T b2 = ywa21 + xwa;
						biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
					}
				}
				else {
					int ni = 0;
					int nb = _nBiquads;
					if (_nPoles % 2 == 1) {
						++ni;
						--nb;
						T rp = std::real(_poles[0].p);
						biquads.setParams(0, 1.0, -1.0, 0.0, wa + rp, wa - rp, 0.0);
					}
					T a0 = 1.0;
					T a1 = -2.0f;
					T a2 = 1.0;
					for (int i = 0; i < nb; ++i) {
						Pole& pole = _poles[ni + i];
						T wa2y = wa2 + pole.y;
						T x = (((T)(i == nb / 2) * (iq - (T)1.0)) + (T)1.0) * pole.x;
						T xwa = x * wa;
						T b0 = wa2y - xwa;
						T b1 = (wa2 + wa2) - (pole.y + pole.y);
						T b2 = wa2y + xwa;
						biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
					}
				}
				break;
			}

			case BANDPASS_MODE:
			case BANDREJECT_MODE: {
				_nBiquads = ((_nPoles / 2) * 2) + (_nPoles % 2);
				biquads.setN(_nBiquads);

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
					int nb = _nBiquads;
					if (_nPoles % 2 == 1) {
						++ni;
						--nb;
						T wp = w * std::real(_poles[0].p);
						biquads.setParams(
							0,
							a0,
							a1,
							a2,
							(T)1.0 + wp + w02,
							(T)-2.0 + (w02 + w02),
							(T)1.0 - wp + w02
						);
					}
					for (int i = 0; i < nb; i += 2) {
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
							biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
						}
						{
							T b0 = (T)1.0 + f1b + f2b;
							T b1 = (T)-2.0 + (f2b + f2b);
							T b2 = (T)1.0 - f1b + f2b;
							biquads.setParams(ni + i + 1, a0, a1, a2, b0, b1, b2);
						}
					}
				}
				else {
					T a0 = (T)1.0 + w02;
					T a1 = (T)-2.0 + (w02 + w02);
					T a2 = a0;

					int ni = 0;
					int nb = _nBiquads;
					if (_nPoles % 2 == 1) {
						++ni;
						--nb;
						T rp = std::real(_poles[0].p);
						T rpw02 = rp * w02;
						biquads.setParams(
							0,
							a0,
							a1,
							a2,
							rp + w + rpw02,
							(T)-2.0 * rp + (rpw02 + rpw02),
							rp - w + rpw02
						);
					}
					for (int i = 0; i < nb; i += 2) {
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
							biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
						}
						{
							T b0 = pole.r + f1b + f2b;
							T b1 = (T)-2.0 * pole.r + (f2b + f2b);
							T b2 = pole.r - f1b + f2b;
							biquads.setParams(ni + i + 1, a0, a1, a2, b0, b1, b2);
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

template struct bogaudio::dsp::MultimodeDesigner<4>;
template struct bogaudio::dsp::MultimodeDesigner<16>;


template<int N> void MultimodeBase<N>::design(
	float sampleRate,
	Type type,
	int poles,
	Mode mode,
	float frequency,
	float qbw,
	BandwidthMode bwm
) {
	_designer.setParams(
		_biquads,
		_outGain,
		sampleRate,
		type,
		poles,
		mode,
		frequency,
		qbw,
		bwm
	);
}

template<int N> float MultimodeBase<N>::next(float sample) {
	return _outGain * _biquads.next(sample);
}

template<int N> void MultimodeBase<N>::reset() {
	_biquads.reset();
}

template struct bogaudio::dsp::MultimodeBase<4>;
template struct bogaudio::dsp::MultimodeBase<16>;
