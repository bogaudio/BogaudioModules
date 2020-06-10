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
	std::string _menuItemLabel;

	TriggerOnLoadModuleWidget(const char* menuItemLabel = "Resume loop on load")
	: _menuItemLabel(menuItemLabel)
	{}

	void appendContextMenu(Menu* menu) override;
};

} // namespace bogaudio
