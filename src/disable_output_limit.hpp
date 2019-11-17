#pragma once

#include "bogaudio.hpp"

namespace bogaudio {

struct DisableOutputLimitModule : BGModule {
	bool _disableOutputLimit = false;

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
};

struct DisableOutputLimitModuleWidget : ModuleWidget {
	void appendContextMenu(Menu* menu) override {
		DisableOutputLimitModule* m = dynamic_cast<DisableOutputLimitModule*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new BoolOptionMenuItem("Disable output limit", [m]() { return &m->_disableOutputLimit; }));
	}
};

} // namespace bogaudio
