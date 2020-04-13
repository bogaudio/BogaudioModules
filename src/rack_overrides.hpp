#pragma once

#include "rack.hpp"

namespace bogaudio {

struct Trigger : rack::dsp::SchmittTrigger {
	float _highThreshold;
	float _lowThreshold;

	Trigger(float highThreshold = 1.0f, float lowThreshold = 0.1f)
	: _highThreshold(highThreshold)
	, _lowThreshold(lowThreshold)
	{
		assert(_highThreshold > _lowThreshold);
		reset();
	}

	bool process(float in) {
		if (state) {
			if (in <= _lowThreshold) {
				state = false;
			}
		}
		else {
			if (in >= _highThreshold) {
				state = true;
				return true;
			}
		}
		return false;
	}
};

struct NegativeTrigger : rack::dsp::SchmittTrigger {
	float _highThreshold;
	float _lowThreshold;

	NegativeTrigger(float highThreshold = -1.0f, float lowThreshold = -0.1f)
	: _highThreshold(highThreshold)
	, _lowThreshold(lowThreshold)
	{
		assert(_highThreshold < _lowThreshold);
		reset();
	}

	bool process(float in) {
		if (state) {
			if (in >= _lowThreshold) {
				state = false;
			}
		}
		else {
			if (in <= _highThreshold) {
				state = true;
				return true;
			}
		}
		return false;
	}
};

inline float getZoom() {
	return APP->scene->rackScroll->zoomWidget->zoom;
}

} // namespace bogaudio
