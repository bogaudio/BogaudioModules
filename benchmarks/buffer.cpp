
#include <benchmark/benchmark.h>

#include <dsp.hpp>

using namespace bogaudio::dsp;

struct BMOverlappingBuffer : OverlappingBuffer<float> {
  BMOverlappingBuffer(int size, int o) : OverlappingBuffer(size, o) {}
  virtual void process(float* samples) {}
};

static void BM_OverlappingBuffer(benchmark::State& state) {
  BMOverlappingBuffer b(1024, 2);
  int i = 0;
  for (auto _ : state) {
    b.step(i++);
  }
}
BENCHMARK(BM_OverlappingBuffer);
