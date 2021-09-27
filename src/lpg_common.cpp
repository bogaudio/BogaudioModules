
#include "lpg_common.hpp"

#define RISE_SHAPE_MODE "rise_shape_mode"
#define FALL_SHAPE_MODE "fall_shape_mode"
#define SHAPE_MODE_OFF "off"
#define SHAPE_MODE_ON "on"
#define SHAPE_MODE_INVERTED "inverted"

json_t* LPGEnvBaseModule::saveToJson(json_t* root) {
	switch (_riseShapeMode) {
		case RiseFallShapedSlewLimiter::OFF_SCVM: {
			json_object_set_new(root, RISE_SHAPE_MODE, json_string(SHAPE_MODE_OFF));
			break;
		}
		case RiseFallShapedSlewLimiter::ON_SCVM: {
			json_object_set_new(root, RISE_SHAPE_MODE, json_string(SHAPE_MODE_ON));
			break;
		}
		case RiseFallShapedSlewLimiter::INVERTED_SCVM: {
			json_object_set_new(root, RISE_SHAPE_MODE, json_string(SHAPE_MODE_INVERTED));
			break;
		}
		default:;
	}
	switch (_fallShapeMode) {
		case RiseFallShapedSlewLimiter::OFF_SCVM: {
			json_object_set_new(root, FALL_SHAPE_MODE, json_string(SHAPE_MODE_OFF));
			break;
		}
		case RiseFallShapedSlewLimiter::ON_SCVM: {
			json_object_set_new(root, FALL_SHAPE_MODE, json_string(SHAPE_MODE_ON));
			break;
		}
		case RiseFallShapedSlewLimiter::INVERTED_SCVM: {
			json_object_set_new(root, FALL_SHAPE_MODE, json_string(SHAPE_MODE_INVERTED));
			break;
		}
		default:;
	}
	return root;
}

void LPGEnvBaseModule::loadFromJson(json_t* root) {
	json_t* rsm = json_object_get(root, RISE_SHAPE_MODE);
	if (rsm) {
		std::string s = json_string_value(rsm);
		if (s == SHAPE_MODE_OFF) {
			_riseShapeMode = RiseFallShapedSlewLimiter::OFF_SCVM;
		}
		else if (s == SHAPE_MODE_ON) {
			_riseShapeMode = RiseFallShapedSlewLimiter::ON_SCVM;
		}
		else if (s == SHAPE_MODE_INVERTED) {
			_riseShapeMode = RiseFallShapedSlewLimiter::INVERTED_SCVM;
		}
	}
	json_t* fsm = json_object_get(root, FALL_SHAPE_MODE);
	if (fsm) {
		std::string s = json_string_value(fsm);
		if (s == SHAPE_MODE_OFF) {
			_fallShapeMode = RiseFallShapedSlewLimiter::OFF_SCVM;
		}
		else if (s == SHAPE_MODE_ON) {
			_fallShapeMode = RiseFallShapedSlewLimiter::ON_SCVM;
		}
		else if (s == SHAPE_MODE_INVERTED) {
			_fallShapeMode = RiseFallShapedSlewLimiter::INVERTED_SCVM;
		}
	}
}

void LPGEnvBaseModule::modulate() {
	_gateToTrigger = _gateToTriggerParamID >= 0 && params[_gateToTriggerParamID].getValue() > 0.5f;
	_timeScale = _timeScaleParamID >= 0 && params[_timeScaleParamID].getValue() > 0.5f ? _longTimeScale : 1.0f;
}


void LPGEnvBaseWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<LPGEnvBaseModule*>(module);
	assert(m);

	OptionsMenuItem* rm = new OptionsMenuItem("Rise shape CV");
	rm->addItem(OptionMenuItem(
		"Enable",
		[m]() { return m->_riseShapeMode == RiseFallShapedSlewLimiter::ON_SCVM; },
		[m]() { m->_riseShapeMode = RiseFallShapedSlewLimiter::ON_SCVM; }
	));
	rm->addItem(OptionMenuItem(
		"Enable inverted",
		[m]() { return m->_riseShapeMode == RiseFallShapedSlewLimiter::INVERTED_SCVM; },
		[m]() { m->_riseShapeMode = RiseFallShapedSlewLimiter::INVERTED_SCVM; }
	));
	rm->addItem(OptionMenuItem(
		"Disable",
		[m]() { return m->_riseShapeMode == RiseFallShapedSlewLimiter::OFF_SCVM; },
		[m]() { m->_riseShapeMode = RiseFallShapedSlewLimiter::OFF_SCVM; }
	));
	OptionsMenuItem::addToMenu(rm, menu);

	OptionsMenuItem* fm = new OptionsMenuItem("Fall shape CV");
	fm->addItem(OptionMenuItem(
		"Enable",
		[m]() { return m->_fallShapeMode == RiseFallShapedSlewLimiter::ON_SCVM; },
		[m]() { m->_fallShapeMode = RiseFallShapedSlewLimiter::ON_SCVM; }
	));
	fm->addItem(OptionMenuItem(
		"Enable inverted",
		[m]() { return m->_fallShapeMode == RiseFallShapedSlewLimiter::INVERTED_SCVM; },
		[m]() { m->_fallShapeMode = RiseFallShapedSlewLimiter::INVERTED_SCVM; }
	));
	fm->addItem(OptionMenuItem(
		"Disable",
		[m]() { return m->_fallShapeMode == RiseFallShapedSlewLimiter::OFF_SCVM; },
		[m]() { m->_fallShapeMode = RiseFallShapedSlewLimiter::OFF_SCVM; }
	));
	OptionsMenuItem::addToMenu(fm, menu);
}
