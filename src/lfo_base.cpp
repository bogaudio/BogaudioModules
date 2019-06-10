
#include "lfo_base.hpp"
#include "dsp/pitch.hpp"

#include <string.h>

#define PITCH_MODE_KEY "pitch_mode"
#define PITCH_MODE_VALUE_CLASSIC "classic"
#define PITCH_MODE_VALUE_COMPLIANT "compliant"

json_t* LFOBase::dataToJson() {
	json_t* root = json_object();
	switch (_pitchMode) {
		case UNKNOWN_PITCH_MODE: {
			break;
		}
		case CLASSIC_PITCH_MODE: {
			json_object_set_new(root, PITCH_MODE_KEY, json_string(PITCH_MODE_VALUE_CLASSIC));
			break;
		}
		case COMPLIANT_PITCH_MODE: {
			json_object_set_new(root, PITCH_MODE_KEY, json_string(PITCH_MODE_VALUE_COMPLIANT));
			break;
		}
	}
	return root;
}

void LFOBase::dataFromJson(json_t* root) {
	json_t* pm = json_object_get(root, PITCH_MODE_KEY);
	if (pm) {
		if (0 == strncmp(json_string_value(pm), PITCH_MODE_VALUE_CLASSIC, strlen(PITCH_MODE_VALUE_CLASSIC))) {
			setPitchMode(CLASSIC_PITCH_MODE);
		}
		else if (0 == strncmp(json_string_value(pm), PITCH_MODE_VALUE_COMPLIANT, strlen(PITCH_MODE_VALUE_COMPLIANT))) {
			setPitchMode(COMPLIANT_PITCH_MODE);
		}
	}
}

void LFOBase::setPitchMode(PitchMode mode) {
	_pitchMode = mode;

	if (_pitchModeListener) {
		_pitchModeListener->pitchModeChanged();
	}
}

float LFOBase::getPitchOffset() {
	float offset = 0.0f;
	if (_slowMode) {
		offset -= 8.0f;
	}
	else {
		offset -= 4.0f;
	}
	if (isCompliantPitchMode()) {
		offset -= 3.0f;
	}
	return offset;
}

void LFOBase::setFrequency(Param& frequency, Input& pitch, Phasor& phasor) {
	float f = frequency.value;
	if (pitch.isConnected()) {
		f += pitch.getVoltage();
	}
	f += getPitchOffset();

	f = cvToFrequency(f);
	if (f > 2000.0f) {
		f = 2000.0f;
	}
	phasor.setFrequency(f);
}
