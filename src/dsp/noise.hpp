
namespace bogaudio {
namespace dsp {

struct NoiseGenerator : Generator {
	NoiseGenerator() {}
};

struct WhiteNoiseGenerator : NoiseGenerator {
	WhiteNoiseGenerator() {}

	virtual float next() {
		return rack::randomf();
	}
};

struct PinkNoiseGenerator : NoiseGenerator {
	static const int _n = 7;
	uint64_t _g;
	uint64_t _gs[_n];
	uint32_t _count;
	uint64_t _sum;

	PinkNoiseGenerator() {
		_sum = _g = rack::randomu64();
		for (int i = 0; i < _n; ++i) {
			_sum += _gs[i] = rack::randomu64();
		}
		_count = rack::randomu32();
	}

	virtual float next() {
		// See: http://www.firstpr.com.au/dsp/pink-noise/
		_sum -= _g;
		_sum += _g = rack::randomu64();
		for (int i = 0, bit = 1; i < _n; ++i, bit <<= 1) {
			if (_count & bit) {
				_sum -= _gs[i];
				_sum += _gs[i] = rack::randomu64();
				break;
			}
		}
		++_count;
		return (_sum >> (64 - 24)) / powf(2, 24);
	}
};

struct GaussianNoiseGenerator : NoiseGenerator {
	GaussianNoiseGenerator() {}

	virtual float next() {
		return rack::randomNormal();
	}
};

} // namespace dsp
} // namespace bogaudio
