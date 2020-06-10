#pragma once

#include "bogaudio.hpp"

namespace bogaudio {

struct DisableOutputLimitModule : BGModule {
	bool _disableOutputLimit = false;

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
};

struct DisableOutputLimitModuleWidget : ModuleWidget {
	void appendContextMenu(Menu* menu) override;
};

} // namespace bogaudio
