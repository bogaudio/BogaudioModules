#pragma once

#include "rack.hpp"

#include "dsp/pitch.hpp"
#include "signal.hpp"

using namespace rack;

namespace bogaudio {

template <int scale>
struct ScaledSquaringParamQuantity : ParamQuantity {
	float getDisplayValue() override {
		float v = getValue();
		if (!module) {
			return v;
		}

		float vv = v * v;
		vv *= (float)scale;
		vv += displayOffset;
		if (v < 0.0f) {
			return -vv;
		}
		return vv;
	}

	void setDisplayValue(float displayValue) override {
		if (!module) {
			return;
		}
		displayValue -= displayOffset;
		float v = fabsf(displayValue) / (float)scale;
		v = powf(v, 0.5f);
		if (displayValue < 0.0f) {
			setValue(-v);
		}
		else {
			setValue(v);
		}
	}
};

typedef ScaledSquaringParamQuantity<1> OneXSquaringParamQuantity;

typedef ScaledSquaringParamQuantity<10> TenXSquaringParamQuantity;

typedef TenXSquaringParamQuantity EnvelopeSegmentParamQuantity;

struct AmpliferParamQuantity : ParamQuantity {
	virtual bool isLinear() {
		return false;
	}

	virtual void setUnits(bool linear) {
		if (linear) {
			unit = "";
		}
		else {
			unit = " dB";
		}
	}

	float getDisplayValue() override {
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

	void setDisplayValue(float displayValue) override {
		if (!module) {
			return;
		}

		if (!isLinear()) {
			displayValue /= dsp::Amplifier::minDecibels;
			displayValue = 1.0f - displayValue;
		}
		setValue(displayValue);
	}
};

struct FrequencyParamQuantity : ParamQuantity {
	virtual float offset() {
		return 0.0f;
	}

	float getDisplayValue() override {
		float v = getValue();
		if (!module) {
			return v;
		}

		v += offset();
		v = powf(2.0f, v);
		v *= dsp::referenceFrequency;
		return v;
	}

	void setDisplayValue(float v) override {
		if (!module) {
			return;
		}

		v /= dsp::referenceFrequency;
		v = log2f(v);
		v -= offset();
		setValue(v);
	}
};

struct DynamicsRatioParamQuantity : ParamQuantity {
	float getDisplayValue() override {
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

	void setDisplayValue(float v) override {
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
};

} // namespace bogaudio
