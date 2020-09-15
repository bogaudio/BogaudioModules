#pragma once

#include "slew_common.hpp"

struct LPGEnvBaseModule : BGModule {
	int _gateToTriggerParamID;
	int _timeScaleParamID;
	float _longTimeScale;
	bool _gateToTrigger = true;
	float _timeScale = 1.0f;
	RiseFallShapedSlewLimiter::ShapeCVMode _riseShapeMode = RiseFallShapedSlewLimiter::INVERTED_SCVM;
	RiseFallShapedSlewLimiter::ShapeCVMode _fallShapeMode = RiseFallShapedSlewLimiter::ON_SCVM;

	LPGEnvBaseModule(int gateToTriggerParamID = -1, int timeScaleParamID = -1, float longTimeScale = 10.0f)
	: _gateToTriggerParamID(gateToTriggerParamID)
	, _timeScaleParamID(timeScaleParamID)
	, _longTimeScale(longTimeScale)
	{}

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	void modulate() override;
};

struct LPGEnvBaseWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

template <int SCALE>
struct TimeParamQuantity : ScaledSquaringParamQuantity<SCALE> {
	typedef ScaledSquaringParamQuantity<SCALE> Base;

	float getDisplayValue() override {
		auto m = dynamic_cast<LPGEnvBaseModule*>(Base::module);
		assert(m);
		return m->_timeScale * Base::getDisplayValue();
	}

	void setDisplayValue(float displayValue) override {
		auto m = dynamic_cast<LPGEnvBaseModule*>(Base::module);
		assert(m);
		return Base::setDisplayValue(displayValue / m->_timeScale);
	}
};
