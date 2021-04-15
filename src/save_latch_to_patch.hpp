#pragma once

#include "bogaudio.hpp"

using namespace rack;

namespace bogaudio {

struct SaveLatchToPatchModule : BGModule {
	bool _saveLatchedToPatch = false;
	bool _latchedHigh[maxChannels] {};
	bool _latch = false;

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
};

struct SaveLatchToPatchModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
