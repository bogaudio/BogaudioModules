
#include <benchmark/benchmark.h>

#include "dsp/noise.hpp"
#include "dsp/filter.hpp"
#include "dsp/decimator.hpp" // rack

using namespace bogaudio::dsp;

static void BM_Decimator(benchmark::State& state) {
  WhiteNoiseGenerator r;
  const int n = 8;
  float buf[n];
  for (int i = 0; i < n; ++i) {
    buf[i] = r.next();
  }
  Decimator d(44100.0, n);
  for (auto _ : state) {
    benchmark::DoNotOptimize(d.next(n, buf));
  }
}
BENCHMARK(BM_Decimator);

static void BM_RackDecimator(benchmark::State& state) {
  WhiteNoiseGenerator r;
  const int n = 8;
  float buf[n];
  for (int i = 0; i < n; ++i) {
    buf[i] = r.next();
  }
  rack::Decimator<n, n> d;
  for (auto _ : state) {
    benchmark::DoNotOptimize(d.process(buf));
  }
}
BENCHMARK(BM_RackDecimator);
