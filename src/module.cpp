
#include "module.hpp"
#include "bogaudio.hpp"
#include "skins.hpp"
#include <cstdio>

using namespace bogaudio;

#define SKIN "skin"

void BGModule::onReset() {
	_steps = _modulationSteps;
	reset();
}

void BGModule::onSampleRateChange() {
	_modulationSteps = APP->engine->getSampleRate() * (2.5f / 1000.0f); // modulate every ~2.5ms regardless of sample rate.
	_steps = _modulationSteps;
	sampleRateChange();
}

json_t* BGModule::dataToJson() {
	json_t* root = json_object();
	if (_skinnable && _skin != "default") {
		json_object_set_new(root, SKIN, json_string(_skin.c_str()));
	}
	return toJson(root);
}

void BGModule::dataFromJson(json_t* root) {
	if (_skinnable) {
		json_t* s = json_object_get(root, SKIN);
		if (s) {
			setSkin(json_string_value(s));
		}
	}

	fromJson(root);
}

void BGModule::process(const ProcessArgs& args) {
	if (_steps < 0) {
		onReset();
		onSampleRateChange();
	}

	processAlways(args);
	if (active()) {
		++_steps;
		if (_steps >= _modulationSteps) {
			_steps = 0;

			int channelsBefore = _channels;
			int channelsNow = std::max(1, channels());
			if (channelsBefore != channelsNow) {
				_channels = channelsNow;
				_inverseChannels = 1.0f / (float)_channels;
				channelsChanged(channelsBefore, channelsNow);
				if (channelsBefore < channelsNow) {
					while (channelsBefore < channelsNow) {
						addChannel(channelsBefore);
						++channelsBefore;
					}
				}
				else {
					while (channelsNow < channelsBefore) {
						removeChannel(channelsBefore - 1);
						--channelsBefore;
					}
				}
			}

			modulate();
			for (int i = 0; i < _channels; ++i) {
				modulateChannel(i);
			}
		}

		processAll(args);
		for (int i = 0; i < _channels; ++i) {
			processChannel(args, i);
		}
		postProcess(args);
	}
	postProcessAlways(args);
}

void BGModule::setSkin(std::string skin) {
	if (skin == "default" || Skins::skins().validKey(skin)) {
		_skin = skin;
		if (_skinChangeListener) {
			_skinChangeListener->skinChanged();
		}
	}
}

void BGModule::setSkinChangeListener(BGModuleWidget* widget) {
	_skinChangeListener = widget;
}


void BGModuleWidget::appendContextMenu(Menu* menu) {
	auto m = dynamic_cast<BGModule*>(module);
	assert(m);
	if (m->_skinnable) {
		auto skins = Skins::skins().available();
		if (skins.size() > 0) {
			// menu->addChild(new MenuLabel());
			OptionsMenuItem* s = new OptionsMenuItem("Skin");
			s->addItem(OptionMenuItem("Default", [m]() { return m->_skin == "default"; }, [m]() { m->setSkin("default"); }));
			for (auto skin : skins) {
				std::string key = skin.key;
				s->addItem(OptionMenuItem(skin.display.c_str(), [m, key]() { return m->_skin == key; }, [m, key]() { m->setSkin(key); }));
			}
			OptionsMenuItem::addToMenu(s, menu);
		}
	}

	contextMenu(menu);
}

void BGModuleWidget::skinChanged() {
	updatePanel();
}

void BGModuleWidget::setPanel(Vec size, std::string slug) {
	_size = size;
	_slug = slug;
	if (module) {
		auto m = dynamic_cast<BGModule*>(module);
		assert(m);
		m->setSkinChangeListener(this);
	}
	updatePanel();
}

void BGModuleWidget::updatePanel() {
	if (_panel) {
		removeChild(_panel);
		delete _panel;
		_panel = NULL;
	}

	std::string skin = Skins::skins().defaultKey();
	if (module) {
		auto m = dynamic_cast<BGModule*>(module);
		assert(m);
		skin = m->_skin;
		if (skin == "default") {
			skin = Skins::skins().defaultKey();
		}
	}

	const int n = 100;
	char svg[n];
	if (skin == "light") {
		snprintf(svg, n, "res/%s.svg", _slug.c_str());
	}
	else {
		snprintf(svg, n, "res/%s-%s.svg", _slug.c_str(), skin.c_str());
	}
	_panel = new SvgPanel();
	_panel->box.size = _size;
	addChildBottom(_panel);
	_panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, svg)));
}
