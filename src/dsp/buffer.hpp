
#include <algorithm>

template<typename T>
struct OverlappingBuffer {
  const int _size;
  const int _overlap;
  const int _overlapN;
  const int _samplesN;
  T* _samples;
  int _sample;

  OverlappingBuffer(int size, int o)
  : _size(size)
  , _overlap(o)
  , _overlapN(_size / _overlap)
  , _samplesN(2*_size - _overlapN)
  , _samples(new T[_samplesN])
  , _sample(0)
  {
    assert(_size > 0);
    assert(_overlap > 0 && _overlap <= _size && _size % _overlap == 0);
  }
  virtual ~OverlappingBuffer() {
    delete[] _samples;
  }

  virtual void process(T* samples) = 0;

  virtual bool step(T sample) {
    _samples[_sample++] = sample;
    assert(_sample <= _samplesN);

    if (_sample >= _size && _sample % _overlapN == 0) {
      process(_samples + _sample - _size);

      if (_overlap == 1) {
        _sample = 0;
      }
      else if (_sample == _samplesN) {
        std::copy(_samples + _size, _samples + _samplesN, _samples);
        _sample = _samplesN - _size;
      }

      return true;
    }
    return false;
  }
};
