#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

struct RiseFallShapedSlewLimiter {
	bool _rising = true;
	float _last = 0.0f;
	ShapedSlewLimiter _rise;
	ShapedSlewLimiter _fall;

	float timeMS(Param& param, Input& input, float maxMS, int c);
	float shape(Param& param);
	void modulate(
		float sampleRate,
		Param& riseParam,
		Input& riseInput,
		float riseMaxMS,
		Param& riseShapeParam,
		Param& fallParam,
		Input& fallInput,
		float fallMaxMS,
		Param& fallShapeParam,
		int c
	);
	float next(float sample);
};
