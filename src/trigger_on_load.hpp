#pragma once

#include "bogaudio.hpp"

using namespace rack;

namespace bogaudio {

struct TriggerOnLoadModule : BGModule {
	bool _triggerOnLoad = true;
	bool _shouldTriggerOnLoad = true;

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;

	virtual bool shouldTriggerOnNextLoad() = 0;
};

struct TriggerOnLoadModuleWidget : BGModuleWidget {
	std::string _menuItemLabel;

	TriggerOnLoadModuleWidget(const char* menuItemLabel = "Resume loop on load")
	: _menuItemLabel(menuItemLabel)
	{}

	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
