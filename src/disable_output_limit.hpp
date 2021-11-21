#pragma once

#include "bogaudio.hpp"

namespace bogaudio {

struct DisableOutputLimitModule : BGModule {
	bool _disableOutputLimit = false;

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
};

struct DisableOutputLimitModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
