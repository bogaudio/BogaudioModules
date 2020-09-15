#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

struct RiseFallShapedSlewLimiter {
	enum ShapeCVMode {
		OFF_SCVM,
		ON_SCVM,
		INVERTED_SCVM
	};

	bool _rising = true;
	float _last = 0.0f;
	ShapedSlewLimiter _rise;
	ShapedSlewLimiter _fall;

	float timeMS(int c, Param& param, Input* input, float maxMS);
	float shape(int c, Param& param, bool invert = false, Input* cv = NULL, ShapeCVMode mode = OFF_SCVM);
	void modulate(
		float sampleRate,
		Param& riseParam,
		Input* riseInput,
		float riseMaxMS,
		Param& riseShapeParam,
		Param& fallParam,
		Input* fallInput,
		float fallMaxMS,
		Param& fallShapeParam,
		int c,
		bool invertRiseShape = false,
		Input* shapeCV = NULL,
		ShapeCVMode riseShapeMode = OFF_SCVM,
		ShapeCVMode fallShapeMode = OFF_SCVM
	);
	float next(float sample);
};
