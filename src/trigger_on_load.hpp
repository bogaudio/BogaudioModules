#pragma once

#include "bogaudio.hpp"

using namespace rack;

namespace bogaudio {

struct TriggerOnLoadModule : BGModule {
	bool _triggerOnLoad = true;
	bool _shouldTriggerOnLoad = true;

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;

	virtual bool shouldTriggerOnNextLoad() = 0;
};

struct TriggerOnLoadModuleWidget : ModuleWidget {
	struct TriggerOnLoadMenuItem : MenuItem {
		TriggerOnLoadModule* _module;

		TriggerOnLoadMenuItem(TriggerOnLoadModule* module, const char* label)
		: _module(module)
		{
			this->text = label;
		}

		void onAction(const event::Action& e) override {
			_module->_triggerOnLoad = !_module->_triggerOnLoad;
		}

		void step() override {
			rightText = _module->_triggerOnLoad ? "✔" : "";
		}
	};

	void appendContextMenu(Menu* menu) override {
		TriggerOnLoadModule* m = dynamic_cast<TriggerOnLoadModule*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new TriggerOnLoadMenuItem(m, "Resume loop on load"));
	}
};

} // namespace bogaudio
