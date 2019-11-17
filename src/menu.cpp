
#include "menu.hpp"

using namespace bogaudio;

#define SUBMENUS 1

void OptionsMenuItem::addToMenu(OptionsMenuItem* item, Menu* menu) {
	// one way or another, this will cause item to eventually be deleted; call only one per item.
#ifdef SUBMENUS
	menu->addChild(item);
#else
	std::string label(item->text);
	label += ":";
	menu->addChild(createMenuLabel(label.c_str()));
	for (const OptionMenuItem& omi : item->_items) {
		auto nomi = new OptionMenuItem(omi);
		nomi->text = std::string("\t") + nomi->text;
		menu->addChild(nomi);
	}
	delete item;
#endif
}
