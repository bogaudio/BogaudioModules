
#include "ffft/FFTRealFixLen.h"

#include "buffer.hpp"
#include "analyzer.hpp"

using namespace bogaudio::dsp;

typedef ffft::FFTRealFixLen<10> FFT;

FFT1024::FFT1024() {
  _fft = new FFT();
}

FFT1024::~FFT1024() {
  delete (FFT*)_fft;
}

void FFT1024::do_fft(float* out, float* in) {
  ((FFT*)_fft)->do_fft(out, in);
}
