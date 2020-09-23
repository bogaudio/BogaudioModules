#pragma once

#include "assert.h"
#include <math.h>

#include "ffft/FFTReal.h"

#include "buffer.hpp"

namespace bogaudio {
namespace dsp {

struct Window {
	int _size;
	float* _window;
	float _sum;

	Window(int size)
	: _size(size)
	, _window(new float[_size] {})
	, _sum(0.0)
	{}
	virtual ~Window() {
		delete[] _window;
	}

	inline int size() { return _size; }
	inline float at(int i) { assert(i >= 0 && i < _size); return _window[i]; }
	inline float sum() { return _sum; }
	void apply(float* in, float* out);
};

struct HanningWindow : Window {
	HanningWindow(int size, float alpha = 0.5);
};

struct HammingWindow : HanningWindow {
	HammingWindow(int size) : HanningWindow(size, 0.54) {}
};

struct KaiserWindow : Window {
	KaiserWindow(int size, float alpha = 7.865f);

	float i0(float x);
};

struct PlanckTaperWindow : Window {
	PlanckTaperWindow(int size, int taperSamples);
};

struct FFT1024 {
	void* _fft = NULL;
	FFT1024();
	~FFT1024();

	void do_fft(float* out, float* in);
};

struct FFT4096 {
	void* _fft = NULL;
	FFT4096();
	~FFT4096();

	void do_fft(float* out, float* in);
};

struct FFT8192 {
	void* _fft = NULL;
	FFT8192();
	~FFT8192();

	void do_fft(float* out, float* in);
};

struct FFT16384 {
	void* _fft = NULL;
	FFT16384();
	~FFT16384();

	void do_fft(float* out, float* in);
};

struct FFT32768 {
	void* _fft = NULL;
	FFT32768();
	~FFT32768();

	void do_fft(float* out, float* in);
};

struct SpectrumAnalyzer : OverlappingBuffer<float> {
	enum Size {
		SIZE_128 = 128,
		SIZE_256 = 256,
		SIZE_512 = 512,
		SIZE_1024 = 1024,
		SIZE_2048 = 2048,
		SIZE_4096 = 4096,
		SIZE_8192 = 8192,
		SIZE_16384 = 16384,
		SIZE_32768 = 32768
	};
	static constexpr Size maxSize = SIZE_32768;

	enum Overlap {
		OVERLAP_1 = 1,
		OVERLAP_2 = 2,
		OVERLAP_4 = 4,
		OVERLAP_8 = 8
	};

	enum WindowType {
		WINDOW_NONE,
		WINDOW_HANNING,
		WINDOW_HAMMING,
		WINDOW_KAISER
	};

	const float _sampleRate;
	ffft::FFTReal<float>* _fft = NULL;
	FFT1024* _fft1024 = NULL;
	FFT4096* _fft4096 = NULL;
	FFT8192* _fft8192 = NULL;
	FFT16384* _fft16384 = NULL;
	FFT32768* _fft32768 = NULL;
	Window* _window = NULL;
	float* _windowOut = NULL;
	float* _fftOut = NULL;

	SpectrumAnalyzer(
		Size size,
		Overlap overlap,
		WindowType windowType,
		float sampleRate,
		bool autoProcess = true
	);
	virtual ~SpectrumAnalyzer();

	void processBuffer(float* samples) override;
	void getMagnitudes(float* bins, int nBins);
};

} // namespace dsp
} // namespace bogaudio
