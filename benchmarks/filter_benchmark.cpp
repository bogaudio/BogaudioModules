
#include <benchmark/benchmark.h>

#include "dsp/noise.hpp"
#include "dsp/filters/multimode.hpp"
#include "dsp/filters/resample.hpp"
#include "dsp/filters/utility.hpp"
#include "dsp/oscillator.hpp"
// #include "dsp/decimator.hpp" // rack

using namespace bogaudio::dsp;

static void BM_Filter_LPFDecimator(benchmark::State& state) {
	WhiteNoiseGenerator r;
	const int n = 8;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = r.next();
	}
	LPFDecimator d(44100.0, n);
	for (auto _ : state) {
		benchmark::DoNotOptimize(d.next(buf));
	}
}
BENCHMARK(BM_Filter_LPFDecimator);

static void BM_Filter_CICDecimator(benchmark::State& state) {
	WhiteNoiseGenerator r;
	const int n = 8;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = r.next();
	}
	CICDecimator d(4, 8);
	for (auto _ : state) {
		benchmark::DoNotOptimize(d.next(buf));
	}
}
BENCHMARK(BM_Filter_CICDecimator);

// static void BM_Filter_RackDecimator(benchmark::State& state) {
//   WhiteNoiseGenerator r;
//   const int n = 8;
//   float buf[n];
//   for (int i = 0; i < n; ++i) {
//     buf[i] = r.next();
//   }
//   rack::Decimator<n, n> d;
//   for (auto _ : state) {
//     benchmark::DoNotOptimize(d.process(buf));
//   }
// }
// BENCHMARK(BM_Filter_RackDecimator);

static void BM_Filter_Biquad(benchmark::State& state) {
	WhiteNoiseGenerator r;
	const int n = 8;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = r.next();
	}

	BiquadFilter<double> f;
	f.setParams(0.012672, 0.025345, 0.012672, 1.102730, -1.974655, 0.922615);
	int i = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(f.next(buf[i]));
		i = (i + 1) % n;
	}
}
BENCHMARK(BM_Filter_Biquad);

static void BM_Filter_AnalogFrequency(benchmark::State& state) {
	WhiteNoiseGenerator r;
	const int n = 128;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = std::abs(r.next()) * 0.5f * M_PI;
	}

	int i = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(std::tan(buf[i]));
		i = (i + 1) % n;
	}
}
BENCHMARK(BM_Filter_AnalogFrequency);

#ifdef RACK_SIMD
static void BM_Filter_Biquad4(benchmark::State& state) {
	WhiteNoiseGenerator r;
	const int n = 128;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = std::abs(r.next()) * 0.5f * M_PI;
	}

	Biquad4 biquads;
	for (int i = 0; i < 4; ++i) {
		biquads.setParams(i, 0.012672, 0.025345, 0.012672, 1.102730, -1.974655, 0.922615);
	}
	int i = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(biquads.next(buf[i]));
		i = (i + 1) % n;
	}
}
BENCHMARK(BM_Filter_Biquad4);
#endif

static void BM_Filter_BiquadBank4(benchmark::State& state) {
	WhiteNoiseGenerator r;
	const int n = 128;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = std::abs(r.next()) * 0.5f * M_PI;
	}

	MultimodeDesigner<4> designer;
	BiquadBank<float, 4> biquads;
	float outGain;
	designer.setParams(
		biquads,
		outGain,
		44100.0f,
		MultimodeTypes::BUTTERWORTH_TYPE,
		4,
		MultimodeTypes::BANDPASS_MODE,
		2000.0f,
		1.0f
	);
	int i = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(biquads.next(buf[i]));
		i = (i + 1) % n;
	}
}
BENCHMARK(BM_Filter_BiquadBank4);

static void BM_Filter_BiquadBank16(benchmark::State& state) {
	WhiteNoiseGenerator r;
	const int n = 128;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = std::abs(r.next()) * 0.5f * M_PI;
	}

	MultimodeDesigner<16> designer;
	BiquadBank<float, 16> biquads;
	float outGain;
	designer.setParams(
		biquads,
		outGain,
		44100.0f,
		MultimodeTypes::BUTTERWORTH_TYPE,
		16,
		MultimodeTypes::BANDPASS_MODE,
		2000.0f,
		1.0f
	);
	int i = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(biquads.next(buf[i]));
		i = (i + 1) % n;
	}
}
BENCHMARK(BM_Filter_BiquadBank16);

static void BM_Filter_BiquadBank16_2Pole(benchmark::State& state) {
	WhiteNoiseGenerator r;
	const int n = 128;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = std::abs(r.next()) * 0.5f * M_PI;
	}

	MultimodeDesigner<16> designer;
	BiquadBank<float, 16> biquads;
	float outGain;
	designer.setParams(
		biquads,
		outGain,
		44100.0f,
		MultimodeTypes::BUTTERWORTH_TYPE,
		2,
		MultimodeTypes::BANDPASS_MODE,
		2000.0f,
		1.0f
	);
	int i = 0;
	for (auto _ : state) {
		benchmark::DoNotOptimize(biquads.next(buf[i]));
		i = (i + 1) % n;
	}
}
BENCHMARK(BM_Filter_BiquadBank16_2Pole);

static void BM_Filter_RMS_Short(benchmark::State& state) {
	SineOscillator o(500.0, 100.0);
	const int n = 256;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = o.next() * 5.0f;
	}
	RootMeanSquare rms(44100.0, 0.05);
	int i = 0;
	for (auto _ : state) {
		i = ++i % n;
		rms.next(buf[i]);
	}
}
BENCHMARK(BM_Filter_RMS_Short);

static void BM_Filter_RMS_Long(benchmark::State& state) {
	SineOscillator o(500.0, 100.0);
	const int n = 256;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = o.next() * 5.0f;
	}
	RootMeanSquare rms(44100.0, 1.0);
	int i = 0;
	for (auto _ : state) {
		i = ++i % n;
		rms.next(buf[i]);
	}
}
BENCHMARK(BM_Filter_RMS_Long);

static void BM_Filter_RMS_Modulating(benchmark::State& state) {
	SineOscillator o(500.0, 100.0);
	const int n = 256;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = o.next() * 5.0f;
	}
	std::minstd_rand g;
	std::uniform_real_distribution<float> r(0.0f, 1.0f);
	RootMeanSquare rms(44100.0, 1.0);
	int i = 0;
	for (auto _ : state) {
		i = ++i % n;
		if (i % 50 == 0) {
			rms.setSensitivity(r(g));
		}
		rms.next(buf[i]);
	}
}
BENCHMARK(BM_Filter_RMS_Modulating);

static void BM_Filter_PureRMS_Short(benchmark::State& state) {
	SineOscillator o(500.0, 100.0);
	const int n = 256;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = o.next() * 5.0f;
	}
	PureRootMeanSquare rms(44100.0, 0.05);
	int i = 0;
	for (auto _ : state) {
		i = ++i % n;
		rms.next(buf[i]);
	}
}
BENCHMARK(BM_Filter_PureRMS_Short);

static void BM_Filter_PucketteEnvelopeFollower(benchmark::State& state) {
	SineOscillator o(500.0, 100.0);
	const int n = 256;
	float buf[n];
	for (int i = 0; i < n; ++i) {
		buf[i] = o.next() * 5.0f;
	}
	PucketteEnvelopeFollower pef;
	int i = 0;
	for (auto _ : state) {
		i = ++i % n;
		pef.next(buf[i]);
	}
}
BENCHMARK(BM_Filter_PucketteEnvelopeFollower);
