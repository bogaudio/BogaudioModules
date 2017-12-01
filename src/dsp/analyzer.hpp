
#include <math.h>

#include "ffft/FFTReal.h"

namespace bogaudio {
namespace dsp {

struct Window {
  int _size;
  float* _window;
  float _sum;

  Window(int size)
  : _size(size)
  , _window(new float[_size] { 0.0 })
  , _sum(0.0)
  {}
  virtual ~Window() {
    delete[] _window;
  }

  virtual float sum() {
    return _sum;
  }

  virtual void apply(float* in, float* out) {
    for (int i = 0; i < _size; ++i) {
      out[i] = in[i] * _window[i];
    }
  }
};

struct HanningWindow : Window {
  HanningWindow(int size, float alpha = 0.5) : Window(size) {
    const float invAlpha = 1.0 - alpha;
    const float twoPIEtc = 2.0 * M_PI / (float)size;
    for (int i = 0; i < _size; ++i) {
      _sum += _window[i] = invAlpha*cos(twoPIEtc*(float)i + M_PI) + alpha;
    }
  }
};

struct HammingWindow : HanningWindow {
  HammingWindow(int size) : HanningWindow(size, 0.54) {}
};


struct FFT1024 {
  void* _fft = NULL;
  FFT1024();
  ~FFT1024();

  void do_fft(float* out, float* in);
};


struct SpectrumAnalyzer : OverlappingBuffer<float> {
  enum Size {
    SIZE_128 = 128,
    SIZE_256 = 256,
    SIZE_512 = 512,
    SIZE_1024 = 1024,
    SIZE_2048 = 2048,
    SIZE_4096 = 4096
  };

  enum Overlap {
    OVERLAP_1 = 1,
    OVERLAP_2 = 2,
    OVERLAP_4 = 4,
    OVERLAP_8 = 8
  };

  enum WindowType {
    WINDOW_NONE,
    WINDOW_HANNING,
    WINDOW_HAMMING
  };

  const float _sampleRate;
  ffft::FFTReal<float>* _fft;
  FFT1024* _fft1024;
  Window* _window;
  float* _windowOut;
  float* _fftOut;

  SpectrumAnalyzer(
    Size size,
    Overlap overlap,
    WindowType windowType,
    float sampleRate
  )
  : OverlappingBuffer(size, overlap)
  , _sampleRate(sampleRate)
  , _fft(NULL)
  , _fft1024(NULL)
  , _window(NULL)
  , _windowOut(NULL)
  , _fftOut(new float[_size])
  {
    assert(_sampleRate > size);

    switch (size) {
      case SIZE_1024: {
        _fft1024 = new FFT1024();
        break;
      }
      default: {
        _fft = new ffft::FFTReal<float>(size);
      }
    }

    switch (windowType) {
      case WINDOW_NONE: {
        break;
      }
      case WINDOW_HANNING: {
        _window = new HanningWindow(size);
        _windowOut = new float[size];
        break;
      }
      case WINDOW_HAMMING: {
        _window = new HammingWindow(size);
        _windowOut = new float[size];
        break;
      }
    }
  }

  virtual ~SpectrumAnalyzer() {
    if (_fft) {
      delete _fft;
    }
    if (_fft1024) {
      delete _fft1024;
    }

    if (_window) {
      delete _window;
      delete[] _windowOut;
    }

    delete[] _fftOut;
  }

  virtual void process(float* samples) {
    float* input = samples;
    if (_window) {
      _window->apply(samples, _windowOut);
      input = _windowOut;
    }
    if (_fft1024) {
      _fft1024->do_fft(_fftOut, input);
    }
    else {
      _fft->do_fft(_fftOut, input);
    }
  }

  void getMagnitudes(float* bins, int nBins) {
    assert(nBins <= _size / 2);
    assert(_size % nBins == 0);

    const int bands = _size / 2;
    const int binWidth = bands / nBins;
    const float normalization = powf(_window ? _window->sum() : _size, 2.0);
    for (int bin = 0; bin < nBins; ++bin) {
      float sum = 0.0;
      int binEnd = bin * binWidth + binWidth;
      for (int i = binEnd - binWidth; i < binEnd; ++i) {
        float power = powf(_fftOut[i], 2.0) + powf(_fftOut[i + bands], 2.0);
        power *= 2.0;
        power /= normalization;
        sum += power;
      }
      bins[bin] = sum / (float)binWidth; // FIXME: averaging is the way to go?
    }
  }

  void getFrequencies(float* bins, int nBins) {
    assert(nBins <= _size / 2);
    assert(_size % nBins == 0);

    const int bands = _size / 2;
    const int binWidth = bands / nBins;
    const float fundamental = _sampleRate / (float)_size;
    for (int bin = 0; bin < nBins; ++bin) {
      bins[bin] = roundf(bin*binWidth*fundamental + binWidth*fundamental/2.0);
    }
  }
};

} // namespace dsp
} // namespace bogaudio
