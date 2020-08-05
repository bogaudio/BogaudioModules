#pragma once

#include "bogaudio.hpp"

namespace bogaudio {

struct DisableOutputLimitModule : BGModule {
	bool _disableOutputLimit = false;

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
};

struct DisableOutputLimitModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
