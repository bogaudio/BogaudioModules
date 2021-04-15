#include "save_latch_to_patch.hpp"

#define SAVE_LATCHED_TO_PATCH "save_latched_to_patch"
#define LATCHED_STATE "latched_state"

json_t* SaveLatchToPatchModule::toJson(json_t* root) {
	json_object_set_new(root, SAVE_LATCHED_TO_PATCH, json_boolean(_saveLatchedToPatch));
	if (_saveLatchedToPatch && _latch) {
		json_t* a = json_array();
		for (int c = 0; c < maxChannels; ++c) {
			json_array_append_new(a, json_boolean(_latchedHigh[c]));
		}
		json_object_set_new(root, LATCHED_STATE, a);
	}
	return root;
}

void SaveLatchToPatchModule::fromJson(json_t* root) {
	json_t* sl = json_object_get(root, SAVE_LATCHED_TO_PATCH);
	if (sl) {
		_saveLatchedToPatch = json_is_true(sl);
		json_t* a = json_object_get(root, LATCHED_STATE);
		if (_saveLatchedToPatch && a && json_array_size(a) == maxChannels) {
			for (int c = 0; c < maxChannels; ++c) {
				json_t* ls = json_array_get(a, c);
				if (ls && json_is_true(ls)) {
					_latchedHigh[c] = true;
				}
			}
		}
	}
}


void SaveLatchToPatchModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<SaveLatchToPatchModule*>(module);
	assert(m);
	menu->addChild(new BoolOptionMenuItem("Save latched state to patch", [m]() { return &m->_saveLatchedToPatch; }));
}
