
#include "sum_average.hpp"

using namespace bogaudio;

#define SUM "sum"

json_t* SumAverageModule::toJson(json_t* root) {
	root = MatrixBaseModule::toJson(root);
	json_object_set_new(root, SUM, json_boolean(_sum));
	return root;
}

void SumAverageModule::fromJson(json_t* root) {
	MatrixBaseModule::fromJson(root);
	json_t* s = json_object_get(root, SUM);
	if (s) {
		_sum = json_is_true(s);
	}
}


void SumAverageModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<SumAverageModule*>(module);
	assert(m);
	MatrixBaseModuleWidget::contextMenu(menu);
	menu->addChild(new OptionMenuItem("Average", [m]() { return !m->_sum; }, [m]() { m->_sum = !m->_sum; }));
}
