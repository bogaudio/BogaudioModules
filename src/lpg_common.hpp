#pragma once

#include "slew_common.hpp"

struct LPGEnvBaseModule : BGModule {
	int _gateToTriggerParamID;
	int _timeScaleParamID;
	bool _gateToTrigger = false;
	float _timeScale = 1.0f;
	const float _maxVelocityDb = 0.0f;
	float _minVelocityDb = -6.0f;

	LPGEnvBaseModule(int gateToTriggerParamID, int timeScaleParamID)
	: _gateToTriggerParamID(gateToTriggerParamID)
	, _timeScaleParamID(timeScaleParamID)
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
