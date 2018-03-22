#pragma once

#include <stdlib.h>
#include <math.h>
#include <vector>

#include "base.hpp"
#include "table.hpp"

namespace bogaudio {
namespace dsp {

struct OscillatorGenerator : Generator {
	float _sampleRate;
	float _frequency;

	OscillatorGenerator(
		float sampleRate,
		float frequency
	)
	: _sampleRate(sampleRate > 1.0 ? sampleRate : 1.0)
	, _frequency(frequency)
	{
	}

	void setSampleRate(float sampleRate) {
		if (_sampleRate != sampleRate && sampleRate >= 1.0) {
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

struct Phasor : OscillatorGenerator {
	static constexpr float maxPhase = 2.0f;

	float _delta;
	double _phase = 0.0;

	Phasor(
		float sampleRate,
		float frequency,
		float initialPhase = 0.0
	)
	: OscillatorGenerator(sampleRate, frequency)
	{
		setPhase(initialPhase);
		_update();
	}

	virtual void _sampleRateChanged() override {
		_update();
	}

	virtual void _frequencyChanged() override {
		_update();
	}

	void setPhase(float radians);
	float nextFromPhasor(const Phasor& phasor, float offset = 0.0f); // offset is not radians, but local phase.
	virtual void _update();
	void advancePhase();
	virtual float _next() override final;
	virtual float _nextForPhase(float phase);

	static float radiansToPhase(float radians) { return radians / M_PI; }
	static float phaseToRadians(float phase) { return phase * M_PI; }
};

struct TablePhasor : Phasor {
	const Table& _table;
	float _amplitude;

	TablePhasor(
		const Table& table,
		double sampleRate,
		double frequency,
		float amplitude = 1.0f
	)
	: Phasor(sampleRate, frequency)
	, _table(table)
	, _amplitude(amplitude)
	{
	}

	virtual float _nextForPhase(float phase) override;
};

struct SineOscillator : OscillatorGenerator {
	double _k1, _k2;
	double _x;
	double _y;
	double _amplitude;

	SineOscillator(
		double sampleRate,
		double frequency,
		double amplitude = 1.0,
		double initialPhase = 0.0
	)
	: OscillatorGenerator(sampleRate, frequency)
	, _amplitude(amplitude)
	{
		setPhase(initialPhase);
		update();
	}

	virtual void _sampleRateChanged() override {
		update();
	}

	virtual void _frequencyChanged() override {
		update();
	}

	void setPhase(double phase);
	void update();
	virtual float _next() override;
};

struct SineTableOscillator : TablePhasor {
	SineTableOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0
	)
	: TablePhasor(StaticSineTable::table(), sampleRate, frequency, amplitude)
	{
	}
};

struct SawOscillator : Phasor {
	const float halfMaxPhase = 0.5f * maxPhase;
	float _amplitude;

	SawOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0f
	)
	: Phasor(sampleRate, frequency)
	, _amplitude(amplitude)
	{
	}

	virtual float _nextForPhase(float phase) override;
};

struct SaturatingSawOscillator : SawOscillator {
	float _saturation;
	float _saturationNormalization;
	float _amplitude2;

	SaturatingSawOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0f
	)
	: SawOscillator(sampleRate, frequency)
	, _saturation(0.0f)
	, _saturationNormalization(1.0f)
	, _amplitude2(amplitude)
	{
	}

	void setSaturation(float saturation);

	virtual float _nextForPhase(float phase) override;
};

struct BandLimitedSawOscillator : SaturatingSawOscillator {
	int _quality;
	const Table& _table;
	float _qd = 0.0f;
	float _halfTableLen;

	BandLimitedSawOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0f,
		int quality = 5,
		const Table& table = StaticBlepTable::table()
	)
	: SaturatingSawOscillator(sampleRate, frequency, amplitude)
	, _quality(quality)
	, _table(table)
	, _halfTableLen(_table.length() / 2)
	{
		setQuality(quality);
	}

	void setQuality(int quality);

	virtual void _update() override;
	virtual float _nextForPhase(float phase) override;
};

struct SquareOscillator : Phasor {
	const float minPulseWidth = 0.03f;
	const float maxPulseWidth = 1.0f - minPulseWidth;
	float _amplitude;
	float _negativeAmplitude;
	float _pulseWidthInput;
	float _pulseWidth = 0.5;
	bool positive = true;

	SquareOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0
	)
	: Phasor(sampleRate, frequency)
	, _amplitude(amplitude)
	, _negativeAmplitude(-amplitude)
	{
	}

	void setPulseWidth(float pw);

	virtual float _nextForPhase(float phase) override;
};

struct BandLimitedSquareOscillator : BandLimitedSawOscillator {
	const float minPulseWidth = 0.03f;
	const float maxPulseWidth = 1.0f - minPulseWidth;
	float _pulseWidthInput;
	float _pulseWidth;
	float _offset;

	BandLimitedSquareOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0f,
		int quality = 5,
		const Table& table = StaticBlepTable::table()
	)
	: BandLimitedSawOscillator(sampleRate, frequency, amplitude, quality, table)
	{
		setPulseWidth(0.05f);
	}

	void setPulseWidth(float pw);

	virtual float _nextForPhase(float phase) override;
};

struct TriangleOscillator : Phasor {
	const float quarterMaxPhase = 0.25f * maxPhase;
	const float threeQuartersMaxPhase = 0.75f * maxPhase;
	const float twiceMaxPhase = 2.0f * maxPhase;
	float _amplitude;

	TriangleOscillator(
		float sampleRate,
		float frequency,
		float amplitude = 1.0
	)
	: Phasor(sampleRate, frequency)
	, _amplitude(amplitude)
	{
	}

	virtual float _nextForPhase(float phase) override;
};

struct SineBankOscillator : OscillatorGenerator {
	struct Partial {
		float frequency;
		float frequencyRatio;
		float amplitude;
		float amplitudeTarget;
		float amplitudeStepDelta;
		int amplitudeSteps;
		SineOscillator sine;

		Partial()
		: frequency(0.0)
		, frequencyRatio(0.0)
		, amplitude(0.0)
		, amplitudeTarget(0.0)
		, amplitudeStepDelta(0.0)
		, amplitudeSteps(0)
		, sine(0.0, 0.0, 1.0)
		{}
	};

	const float _maxPartialFrequencySRRatio = 0.48;
	float _maxPartialFrequency = 0.0;
	const int _amplitudeEnvelopeMS = 10;
	int _amplitudeEnvelopeSamples = 0;
	std::vector<Partial> _partials;

	SineBankOscillator(
		float sampleRate,
		float frequency,
		int partialCount
	)
	: OscillatorGenerator(sampleRate, frequency)
	, _partials(partialCount)
	{
		_sampleRateChanged();
		_frequencyChanged();
	}

	int partialCount() {
		return _partials.size();
	}

	// one-based indexes.
	void setPartial(int i, float frequencyRatio, float amplitude);
	void setPartialFrequencyRatio(int i, float frequencyRatio);
	void setPartialAmplitude(int i, float amplitude, bool envelope = false);

	void syncToPhase(float phase);

	virtual void _sampleRateChanged() override;
	virtual void _frequencyChanged() override;
	virtual float _next() override;
};

} // namespace dsp
} // namespace bogaudio
