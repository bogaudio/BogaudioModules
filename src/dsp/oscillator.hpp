
namespace bogaudio {
namespace dsp {

struct OscillatorGenerator : Generator {
	float _sampleRate;
	float _frequency;

	OscillatorGenerator(
		float sampleRate,
		float frequency
	)
	: _sampleRate(sampleRate)
	, _frequency(frequency)
	{
	}

	void setSampleRate(float sampleRate) {
		if (_sampleRate != sampleRate) {
			_sampleRate = sampleRate;
			_sampleRateChanged();
		}
	}

	virtual void _sampleRateChanged() {}

	void setFrequency(float frequency) {
		if (_frequency != frequency) {
			_frequency = frequency;
			_frequencyChanged();
		}
	}

	virtual void _frequencyChanged() {}
};

struct SineOscillator : OscillatorGenerator {
	float _x;
	float _y;
	float _sinDeltaTheta;
	float _cosDeltaTheta;

	SineOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0
	)
	: OscillatorGenerator(sampleRate, frequency)
	, _x(amplitude)
	, _y(0.0)
	{
		updateDeltaTheta();
	}

	virtual void _sampleRateChanged() override {
		updateDeltaTheta();
	}

	virtual void _frequencyChanged() override {
		updateDeltaTheta();
	}

	void updateDeltaTheta();
	virtual float _next() override;
};

} // namespace dsp
} // namespace bogaudio
