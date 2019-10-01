#pragma once

#include "bogaudio.hpp"

namespace bogaudio {

struct DisableOutputLimitModule : BGModule {
	bool _disableOutputLimit = false;

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
};

struct DisableOutputLimitModuleWidget : ModuleWidget {
	struct DisableOutputLimitMenuItem : MenuItem {
		DisableOutputLimitModule* _module;

		DisableOutputLimitMenuItem(DisableOutputLimitModule* module, const char* label)
		: _module(module)
		{
			this->text = label;
		}

		void onAction(const event::Action& e) override {
			_module->_disableOutputLimit = !_module->_disableOutputLimit;
		}

		void step() override {
			MenuItem::step();
			rightText = _module->_disableOutputLimit ? "✔" : "";
		}
	};

	void appendContextMenu(Menu* menu) override {
		DisableOutputLimitModule* dolm = dynamic_cast<DisableOutputLimitModule*>(module);
		assert(dolm);
		menu->addChild(new MenuLabel());
		menu->addChild(new DisableOutputLimitMenuItem(dolm, "Disable output limit"));
	}
};

} // namespace bogaudio
