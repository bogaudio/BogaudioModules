
#include "slew_common.hpp"

float RiseFallShapedSlewLimiter::timeMS(Param& param, Input* input, float maxMS, int c) {
	float time = clamp(param.getValue(), 0.0f, 1.0f);
	if (input && input->isConnected()) {
		time *= clamp(input->getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	return time * time * maxMS;
}

float RiseFallShapedSlewLimiter::shape(Param& param, bool invert) {
	float shape = clamp(param.getValue(), -1.0f, 1.0f);
	if (invert) {
		shape *= -1.0f;
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
	bool invertRiseShape
) {
	_rise.setParams(
		sampleRate,
		timeMS(riseParam, riseInput, riseMaxMS, c),
		shape(riseShapeParam, invertRiseShape)
	);
	_fall.setParams(
		sampleRate,
		timeMS(fallParam, fallInput, fallMaxMS, c),
		shape(fallShapeParam)
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
