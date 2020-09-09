
#include "lpg_common.hpp"

#define VELOCITY_MINIMUM_DECIBELS "velocity_minimum_decibels"

json_t* LPGEnvBaseModule::toJson(json_t* root) {
	json_object_set_new(root, VELOCITY_MINIMUM_DECIBELS, json_real(_minVelocityDb));
	return root;
}

void LPGEnvBaseModule::fromJson(json_t* root) {
	json_t* mdb = json_object_get(root, VELOCITY_MINIMUM_DECIBELS);
	if (mdb) {
		_minVelocityDb = json_real_value(mdb);
	}
}

void LPGEnvBaseModule::modulate() {
	_gateToTrigger = _gateToTriggerParamID >= 0 && params[_gateToTriggerParamID].getValue() > 0.5f;
	_timeScale = _timeScaleParamID >= 0 && params[_timeScaleParamID].getValue() > 0.5f ? _longTimeScale : 1.0f;
}


void LPGEnvBaseWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<LPGEnvBaseModule*>(module);
	assert(m);
	OptionsMenuItem* mi = new OptionsMenuItem("Minimum velocity output gain");
	mi->addItem(OptionMenuItem("-3db", [m]() { return m->_minVelocityDb == -3.0f; }, [m]() { m->_minVelocityDb = -3.0f; }));
	mi->addItem(OptionMenuItem("-6db", [m]() { return m->_minVelocityDb == -6.0f; }, [m]() { m->_minVelocityDb = -6.0f; }));
	mi->addItem(OptionMenuItem("-12db", [m]() { return m->_minVelocityDb == -12.0f; }, [m]() { m->_minVelocityDb = -12.0f; }));
	mi->addItem(OptionMenuItem("-24db", [m]() { return m->_minVelocityDb == -24.0f; }, [m]() { m->_minVelocityDb = -24.0f; }));
	mi->addItem(OptionMenuItem("-60db", [m]() { return m->_minVelocityDb == -60.0f; }, [m]() { m->_minVelocityDb = -60.0f; }));
	OptionsMenuItem::addToMenu(mi, menu);
}
