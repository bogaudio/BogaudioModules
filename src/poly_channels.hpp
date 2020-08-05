#pragma once

#include "bogaudio.hpp"

using namespace rack;

namespace bogaudio {

struct PolyChannelsModule : BGModule {
	int _polyChannels = 1;

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
};

struct PolyChannelsMenuItem : MenuItem {
	PolyChannelsModule* _module;
	int _maxChannels;

	PolyChannelsMenuItem(PolyChannelsModule* module, int maxChannels = 16, const char* label = "Polyphony channels")
	: _module(module)
	{
		_maxChannels = clamp(maxChannels, 1, BGModule::maxChannels);
		this->text = label;
	}

	Menu* createChildMenu() override;
	void step() override;
};

} // namespace bogaudio
