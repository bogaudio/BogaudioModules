
namespace bogaudio {
namespace dsp {

struct NoiseGenerator : Generator {};

struct WhiteNoiseGenerator : NoiseGenerator {
	const uint32_t _mask = -1;
	const float _randMax = powf(2.0, 31) - 1;
	int _last = rack::randomu32();

	virtual float _next() {
		// don't use this for cryptography.
		_last = ((_last * 1103515245) + 12345) & _mask;
		return _last / _randMax;
	}
};

template<typename G>
struct BasePinkNoiseGenerator : NoiseGenerator {
	static const int _n = 7;
	G _g;
	G _gs[_n];
	uint32_t _count = rack::randomu32();

	virtual float _next() {
		// See: http://www.firstpr.com.au/dsp/pink-noise/
		float sum = _g.next();
		for (int i = 0, bit = 1; i < _n; ++i, bit <<= 1) {
			if (_count & bit) {
				sum += _gs[i].next();
			}
			else {
				sum += _gs[i].current();
			}
		}
		++_count;
		return sum / (float)(_n + 1);
	}
};

struct PinkNoiseGenerator : BasePinkNoiseGenerator<WhiteNoiseGenerator> {};

struct RedNoiseGenerator : BasePinkNoiseGenerator<PinkNoiseGenerator> {};

struct GaussianNoiseGenerator : NoiseGenerator {
	GaussianNoiseGenerator() {}

	virtual float _next() {
		return rack::randomNormal();
	}
};

} // namespace dsp
} // namespace bogaudio
