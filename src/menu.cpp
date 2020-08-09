
#include "menu.hpp"

using namespace bogaudio;

#define SUBMENUS 1

void OptionsMenuItem::addItem(const OptionMenuItem& item) {
	_items.push_back(item);
}

void OptionsMenuItem::addSpacer() {
	_items.push_back(SpacerOptionMenuItem());
}

Menu* OptionsMenuItem::createChildMenu() {
	Menu* menu = new Menu;
	for (const OptionMenuItem& item : _items) {
		if (item.text == "<spacer>") {
			menu->addChild(new MenuLabel());
		}
		else {
			menu->addChild(new OptionMenuItem(item));
		}
	}
	return menu;
}

void OptionsMenuItem::addToMenu(OptionsMenuItem* item, Menu* menu) {
	// one way or another, this will cause item to eventually be deleted; call only one per item.
#ifdef SUBMENUS
	menu->addChild(item);
#else
	std::string label(item->text);
	label += ":";
	menu->addChild(createMenuLabel(label.c_str()));
	for (const OptionMenuItem& omi : item->_items) {
		if (omi.text == "<spacer>") {
			menu->addChild(new MenuLabel());
		}
		else {
			auto nomi = new OptionMenuItem(omi);
			nomi->text = std::string("\t") + nomi->text;
			menu->addChild(nomi);
		}
	}
	delete item;
#endif
}
