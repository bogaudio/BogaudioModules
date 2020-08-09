#pragma once

#include <functional>
#include "rack.hpp"

using namespace rack;

namespace bogaudio {

struct OptionMenuItem : MenuItem {
	std::function<bool()> _check;
	std::function<void()> _set;

	OptionMenuItem(const char* label, std::function<bool()> check, std::function<void()> set)
	: _check(check)
	, _set(set)
	{
		this->text = label;
	}

	void onAction(const event::Action& e) override {
		_set();
	}

	void step() override {
		MenuItem::step();
		rightText = _check() ? "✔" : "";
	}
};

struct BoolOptionMenuItem : OptionMenuItem {
	BoolOptionMenuItem(const char* label, std::function<bool*()> get)
	: OptionMenuItem(label, [=]() { return *(get()); }, [=]() { bool* b = get(); *b = !*b; })
	{}
};

struct SpacerOptionMenuItem : OptionMenuItem {
	SpacerOptionMenuItem()
	: OptionMenuItem("<spacer>", []() { return false; }, []() {})
	{}
};

struct OptionsMenuItem : MenuItem {
	std::vector<OptionMenuItem> _items;

	OptionsMenuItem(const char* label) {
		this->text = label;
		this->rightText = "▸";
	}

	void addItem(const OptionMenuItem& item);
	void addSpacer();
	Menu* createChildMenu() override;
	static void addToMenu(OptionsMenuItem* item, Menu* menu);
};

} // namespace bogaudio
