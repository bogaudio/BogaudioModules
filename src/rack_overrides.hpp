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
		reset();
	}

	bool process(float in) {
		// FIXME.v1
		// switch (state) {
		// 	case LOW:
		// 		if (in >= _highThreshold) {
		// 			state = HIGH;
		// 			return true;
		// 		}
		// 		break;
		// 	case HIGH:
		// 		if (in <= _lowThreshold) {
		// 			state = LOW;
		// 		}
		// 		break;
		// 	default:
		// 		if (in >= _highThreshold) {
		// 			state = HIGH;
		// 		}
		// 		else if (in <= _lowThreshold) {
		// 			state = LOW;
		// 		}
		// 		break;
		// }
		return false;
	}
};

} // namespace bogaudio
