
#include <benchmark/benchmark.h>

#include "dsp/noise.hpp"
#include "dsp/filters/multimode.hpp"
#include "dsp/filters/resample.hpp"
// #include "dsp/decimator.hpp" // rack

using namespace bogaudio::dsp;

static void BM_LPFDecimator(benchmark::State& state) {
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
BENCHMARK(BM_LPFDecimator);

static void BM_CICDecimator(benchmark::State& state) {
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
BENCHMARK(BM_CICDecimator);

// static void BM_RackDecimator(benchmark::State& state) {
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
// BENCHMARK(BM_RackDecimator);

static void BM_Biquad(benchmark::State& state) {
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
BENCHMARK(BM_Biquad);

static void BM_AnalogFrequency(benchmark::State& state) {
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
BENCHMARK(BM_AnalogFrequency);

#ifdef RACK_SIMD
static void BM_Biquad4(benchmark::State& state) {
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
BENCHMARK(BM_Biquad4);
#endif

static void BM_BiquadBank4(benchmark::State& state) {
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
BENCHMARK(BM_BiquadBank4);

static void BM_BiquadBank16(benchmark::State& state) {
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
BENCHMARK(BM_BiquadBank16);

static void BM_BiquadBank16_2Pole(benchmark::State& state) {
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
BENCHMARK(BM_BiquadBank16_2Pole);
