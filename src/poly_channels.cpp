
#include "poly_channels.hpp"

using namespace bogaudio;

#define POLY_CHANNELS "poly_channels"

json_t* PolyChannelsModule::toJson(json_t* root) {
	json_object_set_new(root, POLY_CHANNELS, json_integer(_polyChannels));
	return root;
}

void PolyChannelsModule::fromJson(json_t* root) {
	json_t* pc = json_object_get(root, POLY_CHANNELS);
	if (!pc) {
		pc = json_object_get(root, "noise_channels"); // backward compatibility hack.
	}
	if (pc) {
		_polyChannels = json_integer_value(pc);
	}
}


Menu* PolyChannelsMenuItem::createChildMenu() {
	Menu* menu = new Menu;
	PolyChannelsModule* m = _module;
	menu->addChild(new OptionMenuItem("Monophonic", [m]() { return m->_polyChannels == 1; }, [m]() { m->_polyChannels = 1; }));
	for (int i = 2; i <= _maxChannels; i++) {
		menu->addChild(new OptionMenuItem(std::to_string(i).c_str(), [m, i]() { return m->_polyChannels == i; }, [m, i]() { m->_polyChannels = i; }));
	}
	return menu;
}

void PolyChannelsMenuItem::step() {
	MenuItem::step();
	this->rightText = format("%d ▸", _module->_polyChannels);
}
