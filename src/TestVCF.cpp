
#include "TestVCF.hpp"

#include <complex>

void TestVCF::LPFModel::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	resonance = std::max(0.1f, 10.0f * resonance);
	_filter.setParams(APP->engine->getSampleRate(), cutoff, resonance);
}

float TestVCF::LPFModel::next(float sample) {
	return _filter.next(sample);
}

void TestVCF::MultipoleModel::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	MultipoleFilter::Type type = mode == HIGHPASS_MODE ? MultipoleFilter::HP_TYPE : MultipoleFilter::LP_TYPE;
	_filter.setParams(type, 2 * (1 + (int)poles), APP->engine->getSampleRate(), cutoff, topology * 0.29);
}

float TestVCF::MultipoleModel::next(float sample) {
	return _filter.next(sample);
}

void TestVCF::ComplexModel::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_pair1a.setComplexParams(
		1.0f,
		0.0f,
		0.0f,
		0.7f,
		2.0f * M_PI * cutoff / APP->engine->getSampleRate()
	);
	_pair1b.setComplexParams(
		1.0f,
		0.0f,
		0.0f,
		0.7f,
		-2.0f * M_PI * cutoff / APP->engine->getSampleRate()
	);
}

float TestVCF::ComplexModel::next(float sample) {
	return _pair1a.next(sample); // + _pair1b.next(sample);
}

void TestVCF::BookExampleModel::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_poles = poles;

	// _filter.setParams(
	// 	0.00473f,
	// 	0.00946f,
	// 	0.00473f,
	// 	1.0f,
	// 	-1.8485f,
	// 	0.8674f
	// );

	resonance = std::max(0.1f, resonance) * 10.0f;
	float iq = 1.0f / resonance;
	float wa = tanf(0.5f * (2.0f * M_PI * cutoff * APP->engine->getSampleTime()));
	float wa2 = wa * wa;
	if (mode == HIGHPASS_MODE) {
		float a0 = 1.0f;
		float a1 = -2.0f;
		float a2 = 1.0f;
		float b0 = 1.0f * wa2 + wa * iq + 1.0f;
		float b1 = 2.0f * wa2 - 2.0f;
		float b2 = 1.0f * wa2 - wa * iq + 1.0f;
		_filter1.setParams(a0, a1, a2, b0, b1, b2);
		_filter2.setParams(a0, a1, a2, b0, b1, b2);
		_filter3.setParams(a0, a1, a2, b0, b1, b2);
		_filter4.setParams(a0, a1, a2, b0, b1, b2);
	}
	else {
		float a0 = wa2;
		float a1 = 2.0f * wa2;
		float a2 = wa2;
		float b0 = wa2 + wa * iq + 1.0f;
		float b1 = 2.0f * wa2 - 2.0f;
		float b2 = wa2 - wa * iq + 1.0f;
		_filter1.setParams(a0, a1, a2, b0, b1, b2);
		_filter2.setParams(a0, a1, a2, b0, b1, b2);
		_filter3.setParams(a0, a1, a2, b0, b1, b2);
		_filter4.setParams(a0, a1, a2, b0, b1, b2);

		// {
		// 	iq = 0.7654f;
		// 	float a0 = wa2;
		// 	float a1 = 2.0f * wa2;
		// 	float a2 = wa2;
		// 	float b0 = wa2 + wa * iq + 1.0f;
		// 	float b1 = 2.0f * wa2 - 2.0f;
		// 	float b2 = wa2 - wa * iq + 1.0f;
		// 	_filter1.setParams(a0, a1, a2, b0, b1, b2);
		// }
		// {
		// 	iq = 1.847f;
		// 	float a0 = wa2;
		// 	float a1 = 2.0f * wa2;
		// 	float a2 = wa2;
		// 	float b0 = wa2 + wa * iq + 1.0f;
		// 	float b1 = 2.0f * wa2 - 2.0f;
		// 	float b2 = wa2 - wa * iq + 1.0f;
		// 	_filter2.setParams(a0, a1, a2, b0, b1, b2);
		// }
		// // _filter3.setParams(a0, a1, a2, b0, b1, b2);
		// // _filter4.setParams(a0, a1, a2, b0, b1, b2);
	}
}

float TestVCF::BookExampleModel::next(float sample) {
	float out = _filter1.next(sample);
	switch (_poles) {
		case POLES_2: {
			break;
		}
		case POLES_4: {
			out = _filter2.next(out);
			break;
		}
		case POLES_6: {
			out = _filter2.next(out);
			out = _filter3.next(out);
			break;
		}
		default: {
			out = _filter2.next(out);
			out = _filter3.next(out);
			out = _filter4.next(out);
			break;
		}
	}
	return out;
}

void TestVCF::ButterworthModel::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_nFilters = 1 + (int)poles;
	assert(_nFilters <= maxPoles / 2);

	cutoff = std::max(2.0f, cutoff); // FIXME: another way to avoid DC exploding near 0 hz?
	// resonance = 1.0f + resonance * 20.0f / (2.0f * (float)_nFilters); // FIXME
	// float iq = 1.0f / resonance;
	float iq = 0.707 - 0.65 * resonance / (1.0f + std::pow(0.6f * std::log((float)_nFilters), 2.0));
	float wa = tanf(0.5f * (2.0f * M_PI * cutoff * APP->engine->getSampleTime()));
	float wa2 = wa * wa;
	if (mode == HIGHPASS_MODE) {
		float a0 = 1.0f;
		float a1 = -2.0f;
		float a2 = 1.0f;
		float b1 = 2.0f * wa2 - 2.0f;
		float i2n = 1.0f / (4.0f * _nFilters);
		for (int i = 0; i < _nFilters; ++i) {
			float x = iq * 2.0f * cosf((float)i * M_PI * i2n);
			float b0 = 1.0f * wa2 + wa * x + 1.0f;
			float b2 = 1.0f * wa2 - wa * x + 1.0f;
			_filters[i].setParams(a0, a1, a2, b0, b1, b2);
		}
	}
	else {
		float a0 = wa2;
		float a1 = 2.0f * wa2;
		float a2 = wa2;
		float b1 = 2.0f * wa2 - 2.0f;
		float i2n = 1.0f / (4.0f * _nFilters);
		for (int i = 0; i < _nFilters; ++i) {
			float x = iq * 2.0f * cosf((float)i * M_PI * i2n);
			float b0 = wa2 + wa * x + 1.0f;
			float b2 = wa2 - wa * x + 1.0f;
			_filters[i].setParams(a0, a1, a2, b0, b1, b2);
		}
	}
}

float TestVCF::ButterworthModel::next(float sample) {
	for (int i = 0; i < _nFilters; ++i) {
		sample = _filters[i].next(sample);
	}
	return sample;
}

template<typename T>
void TestVCF::BPButterworthModel1<T>::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_nFilters = 1 + (int)poles;
	assert(_nFilters <= maxPoles / 2);

	bandwidth = 0.1;
	T span = std::max(1.0, bandwidth * 1000.0);
	T wdl = std::max(1.0, cutoff - span);
	T wdh = std::min((T)maxCutoff, cutoff + span);
	T wal = std::tan(0.5 * (2.0 * M_PI * wdl * APP->engine->getSampleTime()));
	T wah = std::tan(0.5 * (2.0 * M_PI * wdh * APP->engine->getSampleTime()));
	T W = wah - wal;
	T W2 = W * W;
	T w02 = wah * wal;

	resonance = 1.0 + resonance * 10.0 / (1.0 * (T)_nFilters); // FIXME
	T iq = 1.0 / resonance;

	T a0 = W2;
	T a1 = W2;
	T a2 = -W2;
	T a3 = -W2;

	T i2n = 1.0 / (2.0 * _nFilters);
	for (int i = 0; i < _nFilters; ++i) {
		T x = iq * 2.0 * std::cos((T)i * M_PI * i2n);
		T w04etc = w02 * w02 + W * x * w02;
		T b0 = 1.0 + W * x + W2 + 2.0 * w02 + w04etc;
		T b1 = -3.0 - W * x + W2 + 2.0 * w02 + 3.0 * w04etc;
		T b2 = 3.0 - W * x - W2 - 2.0 * w02 + 3.0 * w04etc;
		T b3 = -1.0 + W * x - W2 - 2.0 * w02 + w04etc;
		_filters[i].setParams(a0, a1, a2, a3, b0, b1, b2, b3);
	}
}

template<typename T>
float TestVCF::BPButterworthModel1<T>::next(float sample) {
	for (int i = 0; i < _nFilters; ++i) {
		sample = _filters[i].next(sample);
	}
	return sample;
}

template<typename T>
void TestVCF::BandButterworthModel<T>::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_nFilterPairs = 1 + (int)poles;
	assert(_nFilterPairs <= maxPoles);

	T span = std::max(1.0, (T)bandwidth / 2.0);
	T wdl = std::max(1.0, cutoff - span);
	T wdh = std::min((T)maxCutoff, cutoff + span);
	T wal = std::tan(0.5 * (2.0 * M_PI * wdl * APP->engine->getSampleTime()));
	T wah = std::tan(0.5 * (2.0 * M_PI * wdh * APP->engine->getSampleTime()));
	T W = wah - wal;
	T w02 = wah * wal;
	const T iq = 1.0 / (0.5 * std::sqrt(2.0));

	if (mode == BAND_REJECT_MODE) {
		T a0 = 1.0 + w02;
		T a1 = 2.0 * w02 - 2.0;
		T a2 = a0;
		T i2n = 1.0 / (4.0 * _nFilterPairs);
		for (int i = 0; i < _nFilterPairs; i += 2) {
			T baseX = iq * 2.0 * std::cos((T)i * M_PI * i2n);
			{
				T x = baseX;
				x = 0.5 * (-x + std::pow(std::max(0.0, x * x - 4.0), 0.5));
				T b0 = -x + W - x * w02;
				T b1 = 2.0 * x - 2.0 * x * w02;
				T b2 = -x - W - x * w02;
				_filters[i].setParams(a0, a1, a2, b0, b1, b2);
			}
			{
				T x = baseX;
				x = 0.5 * (-x - std::pow(std::max(0.0, x * x - 4.0), 0.5));
				T b0 = -x + W - x * w02;
				T b1 = 2.0 * x - 2.0 * x * w02;
				T b2 = -x - W - x * w02;
				_filters[i + 1].setParams(a0, a1, a2, b0, b1, b2);
			}
		}
	}
	else {
		T a0 = W;
		T a1 = 0.0;
		T a2 = -W;
		T i2n = 1.0 / (4.0 * _nFilterPairs);
		for (int i = 0; i < _nFilterPairs; i += 2) {
			T baseX = iq * 2.0 * std::cos((T)i * M_PI * i2n);
			{
				T x = baseX;
				x = 0.5 * (-x + std::pow(std::max(0.0, x * x - 4.0), 0.5));
				T b0 = 1.0 - x * W + w02;
				T b1 = -2.0 + 2.0 * w02;
				T b2 = 1.0 + x * W + w02;
				_filters[i].setParams(a0, a1, a2, b0, b1, b2);
			}
			{
				T x = baseX;
				x = 0.5 * (-x - std::pow(std::max(0.0, x * x - 4.0), 0.5));
				T b0 = 1.0 - x * W + w02;
				T b1 = -2.0 + 2.0 * w02;
				T b2 = 1.0 + x * W + w02;
				_filters[i + 1].setParams(a0, a1, a2, b0, b1, b2);
			}
		}
	}
}

template<typename T>
float TestVCF::BandButterworthModel<T>::next(float sample) {
	for (int i = 0; i < _nFilterPairs; i += 2) {
		sample = _filters[i].next(sample);
		sample = _filters[i + 1].next(sample);
	}
	return sample;
}

template<typename T>
void TestVCF::QuarticBandButterworthModel<T>::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_nFilters = 1 + (int)poles;
	assert(_nFilters <= maxPoles / 2);

	T span = std::max(1.0l, (T)bandwidth / 2.0);
	T wdl = std::max(1.0l, cutoff - span);
	T wdh = std::min((T)maxCutoff, cutoff + span);
	T wal = std::tan(0.5 * (2.0 * M_PI * wdl * APP->engine->getSampleTime()));
	T wah = std::tan(0.5 * (2.0 * M_PI * wdh * APP->engine->getSampleTime()));
	T W = wah - wal;
	T W2 = W * W;
	T w02 = wah * wal;
	T w04 = w02 * w02;
	const T iq = 1.0 / (0.5 * std::sqrt(2.0));

	T a0 = W2;
	T a1 = 0.0;
	T a2 = -2.0 * W2;
	T a3 = 0.0;
	T a4 = W2;

	T i2n = 1.0 / (4.0 * _nFilters);
	for (int i = 0; i < _nFilters; ++i) {
		T x = iq * 2.0 * std::cos((T)i * M_PI * i2n);
		T xW = x * W;
		T W2w02 = W2 + 2.0 * w02;
		T xWw02 = xW * w02;
		T b0 = +1.0  +  +1.0 * xW  +  +1.0 * W2w02  +  +1.0 * xWw02  +  +1.0 * w04;
		T b1 = -4.0  +  -2.0 * xW  +  +0.0 * W2w02  +  +2.0 * xWw02  +  +4.0 * w04;
		T b2 = +6.0  +  +0.0 * xW  +  -2.0 * W2w02  +  +0.0 * xWw02  +  +6.0 * w04;
		T b3 = -4.0  +  +2.0 * xW  +  +0.0 * W2w02  +  -2.0 * xWw02  +  +4.0 * w04;
		T b4 = +1.0  +  +1.0 * xW  +  +1.0 * W2w02  +  -1.0 * xWw02  +  +1.0 * w04;
		_filters[i].setParams(a0, a1, a2, a3, a4, b0, b1, b2, b3, b4);
	}
}

template<typename T>
float TestVCF::QuarticBandButterworthModel<T>::next(float sample) {
	for (int i = 0; i < _nFilters; ++i) {
		sample = _filters[i].next(sample);
	}
	return sample;
}

void TestVCF::AllPassModel::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	float tq = std::tan(resonance * M_PI * APP->engine->getSampleTime());
	float alpha = (tq - 1.0f) / (tq + 1.0f);
	float beta = -std::cos(2.0f * M_PI * cutoff * APP->engine->getSampleTime());
	float a1 = beta * (1.0f - alpha);
	_filter.setParams(
		-alpha,
		a1,
		1.0f,
		1.0f,
		a1,
		-alpha
	);
}

float TestVCF::AllPassModel::next(float sample) {
	return _filter.next(sample);
}


template<typename T>
void TestVCF::ChebyshevModel<T>::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	switch (mode) {
		case BANDPASS_MODE:
		case BAND_REJECT_MODE: {
			setParamsBPBR(cutoff, bandwidth, resonance, mode, poles, topology);
			break;
		}
		default: {
			setParamsLPHP(cutoff, bandwidth, resonance, mode, poles, topology);
		}
	}
}

template<typename T>
void TestVCF::ChebyshevModel<T>::setParamsLPHP(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	int nf = 1 + (int)poles;
	assert(nf <= maxPoles / 2);
	for (int i = _nFilters; i < nf; ++i) {
		_filters[i].reset();
	}
	_nFilters = nf;
	int nPoles = 2 * _nFilters;

	cutoff = std::max(2.0f, cutoff); // FIXME: another way to avoid DC exploding near 0 hz?
	T iq = (1.0 / std::sqrt(2.0)) - 0.65 * resonance; // / (1.0 + std::pow(0.6 * std::log((float)_nFilters), 2.0)); // FIXME
	T wa = std::tan(0.5 * (2.0 * M_PI * cutoff * APP->engine->getSampleTime()));

	T ripple = 3.0 + std::max(0.0, 12.0 * resonance);
	if (topology >= 0.5) {
		T e = ripple / 10.0;
		e = std::pow(10.0, e);
		e -= 1.0f;
		e = std::sqrt(e);
		T ef = std::asinh(1.0 / e) / (float)nPoles;

		// _outGain = 1.0f / (std::pow(2.0, (T)(nPoles - 1)) * e);
		_outGain = 1.0f / std::pow(2.0, (T)(nPoles - 1));

		for (int i = 0, fi = 0; i < _nFilters; ++i) {
			int k = i + 1;
			T a = ((T)(2 * k + nPoles - 1)) * M_PI / (T)(2 * nPoles);
			T re = -std::sinh(ef) * std::sin(a);
			T im = std::cosh(ef) * std::cos(a);
			T x = 2.0 * re;
			T y = re * re + im * im;
			// fi += polesToFilters(mode, _filters + fi, x, y, wa, 0.0, 0.0);
			polesToFilterLPHP(mode, _filters[fi], x, y, wa);
			++fi;
		}
	}
	else {
		_outGain = 1.0f;

		for (int i = 0, fi = 0; i < _nFilters; ++i) {
			int k = i + 1;
			T a = ((T)(2 * k + nPoles - 1)) * M_PI / (T)(2 * nPoles);
			T re = std::cos(a);
			T im = std::sin(a);
			T x = (i == _nFilters / 2 ? iq : 1.0) * 2.0 * re;
			T y = re * re + im * im;
			// fi += polesToFilters(mode, _filters + fi, x, y, wa, 0.0, 0.0);
			polesToFilterLPHP(mode, _filters[fi], x, y, wa);
			++fi;
		}
	}
}

template<typename T>
void TestVCF::ChebyshevModel<T>::setParamsBPBR(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	int nf = 2 * (1 + (int)poles);
	assert(nf <= maxPoles);
	for (int i = _nFilters; i < nf; ++i) {
		_filters[i].reset();
	}
	_nFilters = nf;
	int nPoles = _nFilters;

	T span = std::max(1.0, (T)bandwidth / 2.0);
	T wdl = std::max(1.0, cutoff - span);
	T wdh = std::min((T)maxCutoff, cutoff + span);
	T wal = std::tan(0.5 * (2.0 * M_PI * wdl * APP->engine->getSampleTime()));
	T wah = std::tan(0.5 * (2.0 * M_PI * wdh * APP->engine->getSampleTime()));
	T W = wah - wal;
	T w02 = wah * wal;
	// const T iq = 1.0 / (0.5 * std::sqrt(2.0));
	// const T iq = 0.707;

	T ripple = 3.0; // + std::max(0.0, 12.0 * resonance);
	if (topology >= 0.5) {
		T e = ripple / 10.0;
		e = std::pow(10.0, e);
		e -= 1.0f;
		e = std::sqrt(e);
		T ef = std::asinh(1.0 / e) / (float)nPoles;

		// _outGain = 1.0f / (std::pow(2.0, (T)(nPoles - 1)) * e);
		_outGain = 1.0f / std::pow(2.0, (T)(nPoles - 1));

		for (int i = 0, fi = 0, n = _nFilters / 2; i < n; ++i) {
			int k = i + 1;
			T a = ((T)(2 * k + nPoles - 1)) * M_PI / (T)(2 * nPoles);
			T re = -std::sinh(ef) * std::sin(a);
			T im = std::cosh(ef) * std::cos(a);
			// T x = 2.0 * re;
			// T y = re * re + im * im;
			// fi += polesToFilters(mode, _filters + fi, x, y, 0.0, W, w02);
			polesToFiltersBPBR(mode, _filters[fi], _filters[fi + 1], re, im, W, w02);
			fi += 2;
		}
	}
	else {
		_outGain = 1.0f;

		for (int i = 0, fi = 0, n = _nFilters / 2; i < n; ++i) {
			int k = i + 1;
			T a = ((T)(2 * k + nPoles - 1)) * M_PI / (T)(2 * nPoles);
			T re = std::cos(a);
			T im = std::sin(a);
			// T x = iq * 2.0 * re;
			// T y = re * re + im * im;
			// fi += polesToFilters(mode, _filters + fi, x, y, 0.0, W, w02);
			polesToFiltersBPBR(mode, _filters[fi], _filters[fi + 1], re, im, W, w02);
			fi += 2;
		}
	}
}

template<typename T>
int TestVCF::ChebyshevModel<T>::polesToFilters(Mode mode, BiquadFilter<T>* fs, T x, T y, T wa, T W, T w02) {
	switch (mode) {
		case BANDPASS_MODE: {
			// y = 1.0;
			x = -x;
			T a0 = W;
			T a1 = 0.0;
			T a2 = -W;
			// x = 0.5 * (-x + std::pow(std::max(0.0, x * x - 4.0), 0.5));
			T rt = std::sqrt(std::max(0.0, x * x - 4.0 * y));
			{
				T xx = 0.5 * (-x + rt);
				T b0 = 1.0 - xx * W + w02;
				T b1 = -2.0 + 2.0 * w02;
				T b2 = 1.0 + xx * W + w02;
				fs[0].setParams(a0, a1, a2, b0, b1, b2);
			}
			{
				T xx = 0.5 * (-x - rt);
				T b0 = 1.0 - xx * W + w02;
				T b1 = -2.0 + 2.0 * w02;
				T b2 = 1.0 + xx * W + w02;
				fs[1].setParams(a0, a1, a2, b0, b1, b2);
			}
			return 2;
		}

		case BAND_REJECT_MODE: {
			x = -x;
			T a0 = 1.0 + w02;
			T a1 = 2.0 * w02 - 2.0;
			T a2 = a0;
			T rt = std::sqrt(std::max(0.0, x * x - 4.0 * y));
			{
				T xx = 0.5 * (-x + rt);
				T b0 = -xx + W - xx * w02;
				T b1 = 2.0 * xx - 2.0 * xx * w02;
				T b2 = -xx - W - xx * w02;
				fs[0].setParams(a0, a1, a2, b0, b1, b2);
			}
			{
				T xx = 0.5 * (-x + rt);
				T b0 = -xx + W - xx * w02;
				T b1 = 2.0 * xx - 2.0 * xx * w02;
				T b2 = -xx - W - xx * w02;
				fs[1].setParams(a0, a1, a2, b0, b1, b2);
			}
			return 2;
		}

		case HIGHPASS_MODE: {
			T wa2 = wa * wa;
			T a0 = 1.0;
			T a1 = -2.0f;
			T a2 = 1.0;
			T b0 = wa2 - x * wa + y;
			T b1 = 2.0 * wa2 - 2.0 * y;
			T b2 = wa2 + x * wa + y;
			fs[0].setParams(a0, a1, a2, b0, b1, b2);
			return 1;
		}

		default: {
			T wa2 = wa * wa;
			T a0 = wa2;
			T a1 = 2.0f * wa2;
			T a2 = wa2;
			T b0 = 1.0 - x * wa + y * wa2;
			T b1 = -2.0 + 2.0 * y * wa2;
			T b2 = 1.0 + x * wa + y * wa2;
			fs[0].setParams(a0, a1, a2, b0, b1, b2);
			return 1;
		}
	}
}

template<typename T>
void TestVCF::ChebyshevModel<T>::polesToFilterLPHP(Mode mode, BiquadFilter<T>& f, T x, T y, T wa) {
	switch (mode) {
		case LOWPASS_MODE: {
			T wa2 = wa * wa;
			T a0 = wa2;
			T a1 = 2.0f * wa2;
			T a2 = wa2;
			T b0 = 1.0 - x * wa + y * wa2;
			T b1 = -2.0 + 2.0 * y * wa2;
			T b2 = 1.0 + x * wa + y * wa2;
			f.setParams(a0, a1, a2, b0, b1, b2);
			break;
		}

		case HIGHPASS_MODE: {
			T wa2 = wa * wa;
			T a0 = 1.0;
			T a1 = -2.0f;
			T a2 = 1.0;
			T b0 = wa2 - x * wa + y;
			T b1 = 2.0 * wa2 - 2.0 * y;
			T b2 = wa2 + x * wa + y;
			f.setParams(a0, a1, a2, b0, b1, b2);
			break;
		}

		default: {
			assert(false);
		}
	}
}

template<typename T>
void TestVCF::ChebyshevModel<T>::polesToFiltersBPBR(Mode mode, BiquadFilter<T>& f0, BiquadFilter<T>& f1, T re, T im, T W, T w02) {
	typedef std::complex<T> TC;
	switch (mode) {
		case BANDPASS_MODE: {
			// TC pole(-re, im);
			// TC poleW2 = pole * W * 0.5;
			// T R = std::abs(poleW2);
			// T r = std::arg(poleW2);
			// TC X = pole * pole;
			// X *= W * W;
			// X -= 4.0 * w02;
			// X = std::sqrt(X);
			// X *= 0.5;
			// T Q = std::abs(X);
			// T q = std::arg(X);
			// T f = 2.0 * Q * std::cos(q);
			// T F1a = 2.0 * R * std::cos(r);
			// T F1b = F1a + f;
			// F1a -= f;
			// f = 2.0 * R * Q * std::cos(r - q);
			// T F2a = R * R + Q * Q;
			// T F2b = F2a + f;
			// F2a -= f;
			TC P(-re, im);
			TC PC = std::conj(P);
			TC X = P * P;
			X *= W * W;
			X -= 4.0 * w02;
			X = std::sqrt(X);
			TC XC = std::conj(X);
			TC Y1 = (X - W * P) * 0.5;
			TC Y1C = (XC - W * PC) * 0.5;
			TC Y2 = (-X - W * P) * 0.5;
			TC Y2C = (-XC - W * PC) * 0.5;
			TC cF1a = -(Y1 + Y1C);
			TC cF2a = Y1 * Y1C;
			TC cF1b = -(Y2 + Y2C);
			TC cF2b = Y2 * Y2C;
			T F1a = std::real(cF1a);
			T F1b = std::real(cF1b);
			T F2a = std::real(cF2a);
			T F2b = std::real(cF2b);

			T a0 = W;
			T a1 = 0.0;
			T a2 = -W;
			{
				T b0 = 1.0 + F1a + F2a;
				T b1 = -2.0 + 2.0 * F2a;
				T b2 = 1.0 - F1a + F2a;
				f0.setParams(a0, a1, a2, b0, b1, b2);
			}
			{
				T b0 = 1.0 + F1b + F2b;
				T b1 = -2.0 + 2.0 * F2b;
				T b2 = 1.0 - F1b + F2b;
				f1.setParams(a0, a1, a2, b0, b1, b2);
			}
			break;
		}

		case BAND_REJECT_MODE: {
			TC P(-re, im);
			TC PC = std::conj(P);
			T R = std::abs(P);
			TC X = P * P;
			X *= -4.0 * w02;
			X += W * W;
			X = std::sqrt(X);
			TC XC = std::conj(X);
			TC Y1 = (X - W) / (2.0 * P);
			TC Y1C = (XC - W) / (2.0 * PC);
			TC Y2 = (-X - W) / (2.0 * P);
			TC Y2C = (-XC - W) / (2.0 * PC);
			TC cF1a = -R * (Y1 + Y1C);
			TC cF2a = R * Y1 * Y1C;
			TC cF1b = -R * (Y2 + Y2C);
			TC cF2b = R * Y2 * Y2C;
			T F1a = std::real(cF1a);
			T F1b = std::real(cF1b);
			T F2a = std::real(cF2a);
			T F2b = std::real(cF2b);

			T a0 = 1.0 + w02;
			T a1 = -2.0 + 2.0 * w02;
			T a2 = 1.0 + w02;
			{
				T b0 = R + F1a + F2a;
				T b1 = -2.0 * R + 2.0 * F2a;
				T b2 = R - F1a + F2a;
				f0.setParams(a0, a1, a2, b0, b1, b2);
			}
			{
				T b0 = R + F1b + F2b;
				T b1 = -2.0 * R + 2.0 * F2b;
				T b2 = R - F1b + F2b;
				f1.setParams(a0, a1, a2, b0, b1, b2);
			}
			break;
		}

		default: {
			assert(false);
		}
	}
}

template<typename T>
float TestVCF::ChebyshevModel<T>::next(float sample) {
	for (int i = 0; i < _nFilters; ++i) {
		sample = _filters[i].next(sample);
	}
	return _outGain * sample;
}

template<typename T>
void TestVCF::TwoPoleResonatorModel<T>::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	// T wa = std::tan(0.5 * (2.0 * M_PI * cutoff * APP->engine->getSampleTime()));
	T wa = 2.0 * M_PI * cutoff * APP->engine->getSampleTime();
	T r = std::max(0.1f, 1.0f * resonance);
	_filter.setParams(1.0, 0.0, -1.0 * r, 1.0, -2.0 * r * std::cos(wa), r * r);
	_outGain = 1.0; // * (1.0 - r * r);
}

template<typename T>
float TestVCF::TwoPoleResonatorModel<T>::next(float sample) {
	return _filter.next(sample) * _outGain;
}

template<typename M, int FACTOR>
void TestVCF::OversamplingModel<M, FACTOR>::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_model.setParams(cutoff / (float)FACTOR, bandwidth / (float)FACTOR, resonance, mode, poles, topology);
	_interpolator.setParams(APP->engine->getSampleRate(), FACTOR);
	_decimator.setParams(APP->engine->getSampleRate(), FACTOR);
}

template<typename M, int FACTOR>
float TestVCF::OversamplingModel<M, FACTOR>::next(float sample) {
	float buf[FACTOR];
	_interpolator.next(sample, buf);
	for (int i = 0; i < FACTOR; ++i) {
		buf[i] = _model.next(buf[i]);
	}
	return _decimator.next(buf);
}

template<typename M>
void TestVCF::FeedbackModel<M>::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_q = resonance;
	_model.setParams(cutoff, bandwidth, 0.0f, mode, poles, topology);
}

template<typename M>
float TestVCF::FeedbackModel<M>::next(float sample) {
	return _last = _model.next(sample - _q * _last);
}

template<typename M, typename T>
void TestVCF::AddResonanceModel<M, T>::setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) {
	_model.setParams(cutoff, bandwidth, 0.0f, mode, poles, topology);
	_resonator.setParams(cutoff, bandwidth, resonance * (1.0 / std::sqrt(2.0)), mode, poles, topology);
}

template<typename M, typename T>
float TestVCF::AddResonanceModel<M, T>::next(float sample) {
	return _model.next(_resonator.next(sample));
}

void TestVCF::modulate() {
	float cutoff = maxCutoff * clamp(params[CUTOFF_PARAM].getValue(), 0.0f, 1.0f);
	float q = clamp(params[Q_PARAM].getValue(), 0.0f, 1.0f);
	float bandwidth = q * 10000.0f;
	_mode = (Mode)clamp((int)params[MODE_PARAM].getValue(), 0, 3);
	_poles = (Poles)clamp((int)params[POLES_PARAM].getValue(), 0, 5);
	float topology = clamp(params[TOPOLOGY_PARAM].getValue(), 0.0f, 1.0f);

	if (_model) {
		_model->setParams(
			cutoff,
			bandwidth,
			q,
			_mode,
			_poles,
			topology
		);
	}
	if (_model2) {
		_model2->setParams(
			cutoff,
			bandwidth,
			q,
			_mode,
			_poles,
			topology
		);
	}

	_amplifier.setLevel(Amplifier::maxDecibels * clamp(params[DRIVE_PARAM].getValue(), 0.0f, 1.0f));
}

void TestVCF::processAll(const ProcessArgs& args) {
	lights[LOWPASS_LIGHT].value = _mode == LOWPASS_MODE;
	lights[HIGHPASS_LIGHT].value = _mode == HIGHPASS_MODE;
	lights[BANDPASS_LIGHT].value = _mode == BANDPASS_MODE;
	lights[BAND_REJECT_LIGHT].value = _mode == BAND_REJECT_MODE;

	lights[POLES_2_LIGHT].value = _poles == POLES_2;
	lights[POLES_4_LIGHT].value = _poles == POLES_4;
	lights[POLES_6_LIGHT].value = _poles == POLES_6;
	lights[POLES_8_LIGHT].value = _poles == POLES_8;
	lights[POLES_10_LIGHT].value = _poles == POLES_10;
	lights[POLES_12_LIGHT].value = _poles == POLES_12;

	float in = inputs[IN_INPUT].getVoltage();
	in = _amplifier.next(in);
	if (_model) {
		float out = _model->next(in);
		out = _saturator.next(out);
		outputs[OUT_OUTPUT].setVoltage(out);
	}
	if (_model2) {
		float out = _model2->next(in);
		out = _saturator2.next(out);
		outputs[OUT_B_OUTPUT].setVoltage(out);
	}
}

struct TestVCFWidget : BGModuleWidget {
	static constexpr int hp = 12;

	TestVCFWidget(TestVCF* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "TestVCF");
		createScrews();

		// generated by svg_widgets.rb
		auto cutoffParamPosition = Vec(40.0, 50.0);
		auto qParamPosition = Vec(40.0, 120.0);
		auto driveParamPosition = Vec(40.0, 190.0);
		auto topologyParamPosition = Vec(40.0, 260.0);
		auto modeParamPosition = Vec(145.0, 77.0);
		auto polesParamPosition = Vec(145.0, 156.0);

		auto inInputPosition = Vec(47.5, 318.0);

		auto outOutputPosition = Vec(76.5, 318.0);
		auto outBOutputPosition = Vec(105.5, 318.0);

		auto lowpassLightPosition = Vec(130.0, 58.0);
		auto highpassLightPosition = Vec(151.0, 58.0);
		auto bandpassLightPosition = Vec(130.0, 68.0);
		auto bandRejectLightPosition = Vec(151.0, 68.0);
		auto poles2LightPosition = Vec(130.0, 127.0);
		auto poles4LightPosition = Vec(151.0, 127.0);
		auto poles6LightPosition = Vec(130.0, 137.0);
		auto poles8LightPosition = Vec(151.0, 137.0);
		auto poles10LightPosition = Vec(130.0, 147.0);
		auto poles12LightPosition = Vec(151.0, 147.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(cutoffParamPosition, module, TestVCF::CUTOFF_PARAM));
		addParam(createParam<Knob38>(qParamPosition, module, TestVCF::Q_PARAM));
		addParam(createParam<Knob38>(driveParamPosition, module, TestVCF::DRIVE_PARAM));
		addParam(createParam<Knob38>(topologyParamPosition, module, TestVCF::TOPOLOGY_PARAM));
		addParam(createParam<StatefulButton9>(modeParamPosition, module, TestVCF::MODE_PARAM));
		addParam(createParam<StatefulButton9>(polesParamPosition, module, TestVCF::POLES_PARAM));

		addInput(createInput<Port24>(inInputPosition, module, TestVCF::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, TestVCF::OUT_OUTPUT));
		addOutput(createOutput<Port24>(outBOutputPosition, module, TestVCF::OUT_B_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(lowpassLightPosition, module, TestVCF::LOWPASS_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(highpassLightPosition, module, TestVCF::HIGHPASS_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(bandpassLightPosition, module, TestVCF::BANDPASS_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(bandRejectLightPosition, module, TestVCF::BAND_REJECT_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(poles2LightPosition, module, TestVCF::POLES_2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(poles4LightPosition, module, TestVCF::POLES_4_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(poles6LightPosition, module, TestVCF::POLES_6_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(poles8LightPosition, module, TestVCF::POLES_8_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(poles10LightPosition, module, TestVCF::POLES_10_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(poles12LightPosition, module, TestVCF::POLES_12_LIGHT));
	}
};

Model* modelTestVCF = createModel<TestVCF, TestVCFWidget>("Bogaudio-TestVCF", "TestVCF", "Test VCF");
