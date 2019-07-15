
#include "select_on_clock.hpp"

#define SELECT_ON_CLOCK "select_on_clock"

json_t* SelectOnClockModule::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, SELECT_ON_CLOCK, json_boolean(_selectOnClock));
	return root;
}

void SelectOnClockModule::dataFromJson(json_t* root) {
	json_t* s = json_object_get(root, SELECT_ON_CLOCK);
	if (s) {
		_selectOnClock = json_is_true(s);
	}
}
