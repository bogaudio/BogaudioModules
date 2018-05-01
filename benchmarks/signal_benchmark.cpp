
#include <vector>

#include <benchmark/benchmark.h>

#include "dsp/noise.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

static void BM_DecibelsToAmplitude(benchmark::State& state) {
  std::vector<float> buf = { 10.0f, 6.0f, 3.0f, 0.0f, -3.0f, -6.0f, -10.0f, -30.0f, -60.0f };
  int i = 0;
  for (auto _ : state) {
    i = ++i % buf.size();
    benchmark::DoNotOptimize(decibelsToAmplitude(buf.at(i)));
  }
}
BENCHMARK(BM_DecibelsToAmplitude);

static void BM_AmplitudeToDecibels(benchmark::State& state) {
  std::vector<float> buf = { 0.0001f, 0.0001f, 0.001f, 0.01, 0.1f, 0.3f, 0.5f, 0.8f, 1.0f, 1.5f, 2.0f, 5.0f, 10.0f };
  int i = 0;
  for (auto _ : state) {
    i = ++i % buf.size();
    benchmark::DoNotOptimize(amplitudeToDecibels(buf.at(i)));
  }
}
BENCHMARK(BM_AmplitudeToDecibels);

static void BM_Amplifier(benchmark::State& state) {
  WhiteNoiseGenerator r;
  const int n = 256;
  float samples[n];
  for (int i = 0; i < n; ++i) {
    samples[i] = r.next();
  }
  std::vector<float> decibels = { 10.0f, 6.0f, 3.0f, 0.0f, -3.0f, -6.0f, -10.0f, -30.0f, -60.0f };

  Amplifier a;
  int i = 0, j = 0;
  for (auto _ : state) {
    i = ++i % decibels.size();
    j = ++j % n;
    a.setLevel(decibels.at(i));
    benchmark::DoNotOptimize(a.next(samples[j]));
  }
}
BENCHMARK(BM_Amplifier);

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
