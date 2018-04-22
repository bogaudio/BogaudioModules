
#include <benchmark/benchmark.h>

#include "dsp/noise.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

static void BM_SlewLimiter_Fast(benchmark::State& state) {
  WhiteNoiseGenerator r;
  const int n = 256;
  float buf[n];
  for (int i = 0; i < n; ++i) {
    buf[i] = r.next();
  }
  SlewLimiter sl(44100.0, 1.0f);
  int i = 0;
  for (auto _ : state) {
    i = ++i % n;
    benchmark::DoNotOptimize(sl.next(buf[i]));
  }
}
BENCHMARK(BM_SlewLimiter_Fast);

static void BM_SlewLimiter_Slow(benchmark::State& state) {
  WhiteNoiseGenerator r;
  const int n = 256;
  float buf[n];
  for (int i = 0; i < n; ++i) {
    buf[i] = r.next();
  }
  SlewLimiter sl(44100.0, 1.0f);
  int i = 0, j = 0;
  for (auto _ : state) {
    i = ++i % n;
    if (i == 0) {
      j = ++j % n;
    }
    benchmark::DoNotOptimize(sl.next(buf[j]));
  }
}
BENCHMARK(BM_SlewLimiter_Slow);
