#include "rack_overrides.hpp"

using namespace bogaudio;

bool Trigger::process(float in) {
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


bool NegativeTrigger::process(float in) {
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
