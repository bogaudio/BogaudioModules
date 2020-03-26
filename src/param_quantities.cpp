#include "param_quantities.hpp"
#include "dsp/filter.hpp"
#include "dsp/pitch.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio;
using namespace bogaudio::dsp;

bool AmpliferParamQuantity::isLinear() {
	return false;
}

void AmpliferParamQuantity::setUnits(bool linear) {
	if (linear) {
		unit = "";
	}
	else {
		unit = " dB";
	}
}

float AmpliferParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	if (!isLinear()) {
		v = 1.0f - v;
		v *= dsp::Amplifier::minDecibels;
		setUnits(false);
	}
	else {
		setUnits(true);
	}
	return v;
}

void AmpliferParamQuantity::setDisplayValue(float displayValue) {
	if (!module) {
		return;
	}

	if (!isLinear()) {
		displayValue /= dsp::Amplifier::minDecibels;
		displayValue = 1.0f - displayValue;
	}
	setValue(displayValue);
}


float FrequencyParamQuantity::offset() {
	return 0.0f;
}

float FrequencyParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	v += offset();
	v = powf(2.0f, v);
	v *= dsp::referenceFrequency;
	return v;
}

void FrequencyParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}

	v /= dsp::referenceFrequency;
	v = log2f(v);
	v -= offset();
	setValue(v);
}


float DynamicsRatioParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	v = powf(v, 1.5f);
	v = 1.0f - v;
	v *= M_PI;
	v *= 0.25f;
	v = tanf(v);
	v = 1.0f / v;
	return v;
}

void DynamicsRatioParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}

	v = 1.0f / v;
	v = atanf(v);
	v *= 4.0f;
	v /= M_PI;
	v = 1.0f - v;
	v = powf(v, 1.0f / 1.5f);
	setValue(v);
}


float EQParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	if (v < 0.0f) {
		return -v * Equalizer::cutDb;
	}
	return v * Equalizer::gainDb;
}

void EQParamQuantity::setDisplayValue(float displayValue) {
	if (!module) {
		return;
	}

	if (displayValue < 0.0f) {
		setValue(-(displayValue / Equalizer::cutDb));
	}
	else {
		setValue(displayValue / Equalizer::gainDb);
	}
}
