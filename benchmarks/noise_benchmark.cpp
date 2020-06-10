
#include <benchmark/benchmark.h>

#include "dsp/noise.hpp"

using namespace bogaudio::dsp;

static void BM_Noise_WhiteNoise(benchmark::State& state) {
	WhiteNoiseGenerator g;
	for (auto _ : state) {
		g.next();
	}
}
BENCHMARK(BM_Noise_WhiteNoise);

static void BM_Noise_PinkNoise(benchmark::State& state) {
	PinkNoiseGenerator g;
	for (auto _ : state) {
		g.next();
	}
}
BENCHMARK(BM_Noise_PinkNoise);

static void BM_Noise_RedNoise(benchmark::State& state) {
	RedNoiseGenerator g;
	for (auto _ : state) {
		g.next();
	}
}
BENCHMARK(BM_Noise_RedNoise);

static void BM_Noise_GaussianNoise(benchmark::State& state) {
	GaussianNoiseGenerator g;
	for (auto _ : state) {
		g.next();
	}
}
BENCHMARK(BM_Noise_GaussianNoise);
