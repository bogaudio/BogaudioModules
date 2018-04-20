#pragma once

namespace bogaudio {
namespace dsp {

struct PositiveZeroCrossing {
	const float positiveThreshold = 0.01f;
	const float negativeThreshold = -positiveThreshold;
	const int zeroesForReset = 20;

	enum State {
		NEGATIVE_STATE,
		POSITIVE_STATE,
		COUNT_ZEROES_STATE
	};

	State _state;
	bool _triggerable;
	int _zeroCount = 0;

	PositiveZeroCrossing(bool triggerable = true)
	: _triggerable(triggerable)
	{
		reset();
	}

	bool next(float sample);
	void reset();
};

} // namespace dsp
} // namespace bogaudio
