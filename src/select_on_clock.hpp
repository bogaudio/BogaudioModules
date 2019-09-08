#pragma once

#include "bogaudio.hpp"

using namespace rack;

namespace bogaudio {

struct SelectOnClockModule : BGModule {
	bool _selectOnClock = false;

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
};

struct SelectOnClockModuleWidget : ModuleWidget {
	struct SelectOnClockMenuItem : MenuItem {
		SelectOnClockModule* _module;

		SelectOnClockMenuItem(SelectOnClockModule* module, const char* label)
		: _module(module)
		{
			this->text = label;
		}

		void onAction(const event::Action& e) override {
			_module->_selectOnClock = !_module->_selectOnClock;
		}

		void step() override {
			rightText = _module->_selectOnClock ? "✔" : "";
		}
	};

	void appendContextMenu(Menu* menu) override {
		SelectOnClockModule* m = dynamic_cast<SelectOnClockModule*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new SelectOnClockMenuItem(m, "Select on clock"));
	}
};

} // namespace bogaudio
