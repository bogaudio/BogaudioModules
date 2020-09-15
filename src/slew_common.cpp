
#include "slew_common.hpp"

float RiseFallShapedSlewLimiter::timeMS(int c, Param& param, Input* input, float maxMS) {
	float time = clamp(param.getValue(), 0.0f, 1.0f);
	if (input && input->isConnected()) {
		time *= clamp(input->getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	return time * time * maxMS;
}

float RiseFallShapedSlewLimiter::shape(int c, Param& param, bool invert, Input* cv, ShapeCVMode mode) {
	float shape = param.getValue();
	if (invert) {
		shape *= -1.0f;
	}
	if (cv && mode != OFF_SCVM) {
		float v = cv->getPolyVoltage(c) / 5.0f;
		if (mode == INVERTED_SCVM) {
			v = -v;
		}
		shape = clamp(shape + v, -1.0f, 1.0f);
	}
	if (shape < 0.0) {
		shape = 1.0f + shape;
		shape = _rise.minShape + shape * (1.0f - _rise.minShape);
	}
	else {
		shape += 1.0f;
	}
	return shape;
}

void RiseFallShapedSlewLimiter::modulate(
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
	bool invertRiseShape,
	Input* shapeCV,
	ShapeCVMode riseShapeMode,
	ShapeCVMode fallShapeMode
) {
	_rise.setParams(
		sampleRate,
		timeMS(c, riseParam, riseInput, riseMaxMS),
		shape(c, riseShapeParam, invertRiseShape, shapeCV, riseShapeMode)
	);
	_fall.setParams(
		sampleRate,
		timeMS(c, fallParam, fallInput, fallMaxMS),
		shape(c, fallShapeParam, false, shapeCV, fallShapeMode)
	);
}

float RiseFallShapedSlewLimiter::next(float sample) {
	if (sample > _last) {
		if (!_rising) {
			_rising = true;
			_rise._last = _last;
		}
		return _last = _rise.next(sample);
	}

	if (_rising) {
		_rising = false;
		_fall._last = _last;
	}
	return _last = _fall.next(sample);
}
