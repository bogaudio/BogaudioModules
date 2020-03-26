#pragma once

#include "rack.hpp"

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
	virtual bool isLinear();
	virtual void setUnits(bool linear);
	float getDisplayValue() override;
	void setDisplayValue(float displayValue) override;
};

struct FrequencyParamQuantity : ParamQuantity {
	virtual float offset();
	float getDisplayValue() override;
	void setDisplayValue(float v) override;
};

struct DynamicsRatioParamQuantity : ParamQuantity {
	float getDisplayValue() override;
	void setDisplayValue(float v) override;
};

struct EQParamQuantity : ParamQuantity {
	float getDisplayValue() override;
	void setDisplayValue(float displayValue) override;
};

} // namespace bogaudio
