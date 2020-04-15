#pragma once

#include "bogaudio.hpp"

using namespace rack;

namespace bogaudio {

struct OutputRange {
	float _rangeOffset = 0.0f;
	float _rangeScale = 10.0f;

	struct OutputRangeParamQuantity : ParamQuantity {
		float getDisplayValue() override;
		void setDisplayValue(float v) override;
	};
};

template<class BASE>
struct OutputRangeModule : BASE, OutputRange {
	json_t* dataToJson() override {
		json_t* root = BASE::dataToJson();
		if (!root) {
			root = json_object();
		}
		json_object_set_new(root, "range_offset", json_real(_rangeOffset));
		json_object_set_new(root, "range_scale", json_real(_rangeScale));
		return root;
	}

	void dataFromJson(json_t* root) override {
		BASE::dataFromJson(root);

		json_t* ro = json_object_get(root, "range_offset");
		if (ro) {
			_rangeOffset = json_real_value(ro);
		}

		json_t* rs = json_object_get(root, "range_scale");
		if (rs) {
			_rangeScale = json_real_value(rs);
		}
	}
};

struct OutputRangeOptionMenuItem : OptionMenuItem {
	OutputRangeOptionMenuItem(OutputRange* module, const char* label, float offset, float scale)
	: OptionMenuItem(
		label,
		[=]() { return module->_rangeOffset == offset && module->_rangeScale == scale; },
		[=]() {
			module->_rangeOffset = offset;
			module->_rangeScale = scale;
		}
	)
	{}

	static void addOutputRangeOptionsToMenu(Module* module, Menu* menu) {
		auto m = dynamic_cast<OutputRange*>(module);
		assert(m);
		OptionsMenuItem* mi = new OptionsMenuItem("Range");
		mi->addItem(OutputRangeOptionMenuItem(m, "+/-10V", 0.0f, 10.0f));
		mi->addItem(OutputRangeOptionMenuItem(m, "+/-5V", 0.0f, 5.0f));
		mi->addItem(OutputRangeOptionMenuItem(m, "+/-3V", 0.0f, 3.0f));
		mi->addItem(OutputRangeOptionMenuItem(m, "+/-1V", 0.0f, 1.0f));
		mi->addItem(OutputRangeOptionMenuItem(m, "0V-10V", 1.0f, 5.0f));
		mi->addItem(OutputRangeOptionMenuItem(m, "0V-5V", 1.0f, 2.5f));
		mi->addItem(OutputRangeOptionMenuItem(m, "0V-3V", 1.0f, 1.5f));
		mi->addItem(OutputRangeOptionMenuItem(m, "0V-1V", 1.0f, 0.5f));
		OptionsMenuItem::addToMenu(mi, menu);
	}
};

} // namespace bogaudio
