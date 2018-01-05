
#include <benchmark/benchmark.h>

#include <dsp.hpp>

using namespace bogaudio::dsp;

static void BM_SineOscillator(benchmark::State& state) {
  SineOscillator o(44100.0, 440.0);
  for (auto _ : state) {
    o.next();
  }
}
BENCHMARK(BM_SineOscillator);
