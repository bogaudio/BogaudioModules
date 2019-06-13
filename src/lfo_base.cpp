
#include "lfo_base.hpp"
#include "dsp/pitch.hpp"

float LFOBase::getPitchOffset() {
	float offset = -3.0f;
	if (_slowMode) {
		offset -= 8.0f;
	}
	else {
		offset -= 4.0f;
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
