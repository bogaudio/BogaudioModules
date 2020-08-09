
#include "skins.hpp"
#include "bogaudio.hpp"
#include <unistd.h>
#include <fstream>
#include <cstdio>

Skins& Skins::skins() {
	static Skins instance;
	std::lock_guard<std::mutex> lock(instance._instanceLock);
	if (!instance._loaded) {
		instance.loadSkins();
		instance.loadCssValues();
		instance._loaded = true;
	}
	return instance;
}

bool Skins::validKey(const std::string& key) const {
	for (auto s : _available) {
		if (s.key == key) {
			return true;
		}
	}
	return false;
}

const char* Skins::skinCssValue(const std::string& skinKey, const std::string& valueKey) const {
	std::string sk = skinKey;
	if (sk == "default") {
		sk = defaultKey();
	}
	if (!validKey(sk)) {
		return NULL;
	}

	auto values = _skinCssValues.find(sk);
	if (values == _skinCssValues.end()) {
		return NULL;
	}
	auto value = values->second.find(valueKey);
	if (value == values->second.end()) {
		return NULL;
	}
	return value->second.c_str();
}

NVGcolor Skins::cssColorToNVGColor(const char* color, const NVGcolor& ifError) {
	auto h2i = [](char h) {
		switch (h) {
			case '0': return 0;
			case '1': return 1;
			case '2': return 2;
			case '3': return 3;
			case '4': return 4;
			case '5': return 5;
			case '6': return 6;
			case '7': return 7;
			case '8': return 8;
			case '9': return 9;
			case 'A': return 10;
			case 'B': return 11;
			case 'C': return 12;
			case 'D': return 13;
			case 'E': return 14;
			case 'F': return 15;
			case 'a': return 10;
			case 'b': return 11;
			case 'c': return 12;
			case 'd': return 13;
			case 'e': return 14;
			case 'f': return 15;
			default: return -1;
		}
	};
	if (color[0] == '#') {
		if (strlen(color) == 4) {
			int i1 = h2i(color[1]);
			int i2 = h2i(color[2]);
			int i3 = h2i(color[3]);
			if (i1 >= 0 && i2 >= 0 && i3 >= 0) {
				return nvgRGBA(16*i1 + i1, 16*i2 + i2, 16*i3 + i3, 0xff);
			}
		}
		else if (strlen(color) == 7) {
			int i11 = h2i(color[1]);
			int i12 = h2i(color[1]);
			int i21 = h2i(color[3]);
			int i22 = h2i(color[4]);
			int i31 = h2i(color[5]);
			int i32 = h2i(color[6]);
			if (i11 >= 0 && i12 >= 0 && i21 >= 0 && i22 >= 0 && i31 >= 0 && i32 >= 0) {
				return nvgRGBA(16*i11 + i12, 16*i21 + i22, 16*i31 + i32, 0xff);
			}
		}
	}
	return ifError;
}

void Skins::setDefaultSkin(std::string skinKey) {
	if (skinKey == "default") {
		skinKey = "light";
	}
	std::string path = rack::asset::user("Bogaudio.json");
	std::string error;
	if (!validKey(skinKey)) {
		error = "invalid key: " + skinKey;
	}
	else {
		std::ofstream f(path);
		f << "{\n  \"skins\": {\n    \"default\": \"";
		f << skinKey;
		f << "\"\n  }\n}\n";
		if (!f) {
			error = "error writing \"" + path + "\": " + strerror(errno);
		}
	}

	if (error.size() > 0) {
		WARN("Bogaudio: setting default skin: %s\n", error.c_str());
	}
	else {
		_default = skinKey;
		INFO("Bogaudio: skin information written to %s\n", path.c_str());

		std::lock_guard<std::mutex> lock(_defaultSkinListenersLock);
		for (auto listener : _defaultSkinListeners) {
			listener->defaultSkinChanged(_default);
		}
	}
}

void Skins::registerDefaultSkinChangeListener(DefaultSkinChangeListener* listener) {
	std::lock_guard<std::mutex> lock(_defaultSkinListenersLock);
	_defaultSkinListeners.insert(listener);
}

void Skins::deregisterDefaultSkinChangeListener(DefaultSkinChangeListener* listener) {
	std::lock_guard<std::mutex> lock(_defaultSkinListenersLock);
	_defaultSkinListeners.erase(listener);
}

void Skins::loadSkins() {
	_available.push_back(Skin("light", "Light"));
	_available.push_back(Skin("dark", "Dark"));
	_available.push_back(Skin("lowcontrast", "Dark (low-contrast)"));
	_default = "light";

	std::string path = rack::asset::user("Bogaudio.json");
	if (access(path.c_str(), R_OK) != 0) {
		return;
	}

	json_error_t error;
	json_t* root = json_load_file(path.c_str(), 0, &error);
	if (!root) {
		WARN("Bogaudio: JSON error loading skins data from %s: %s\n", path.c_str(), error.text);
		return;
	}

	json_t* skins = json_object_get(root, "skins");
	if (!skins) {
		WARN("Bogaudio: no \"skins\" section found in %s\n", path.c_str());
	}
	else {
		json_t* d = json_object_get(skins, "default");
		if (!d) {
			WARN("Bogaudio: \"skins\" section has no key \"default\" in %s\n", path.c_str());
		}
		else {
			std::string dk = json_string_value(d);
			if (!validKey(dk)) {
				WARN("Bogaudio: \"skins\" \"default\" value \"%s\" is invalid in %s\n", dk.c_str(), path.c_str());
				WARN("Bogaudio: available skins are:\n");
				for (auto s : _available) {
					WARN("Bogaudio:   %s\n", s.key.c_str());
				}
			}
			else {
				_default = dk;
				INFO("Bogaudio: skin information loaded successfully from %s\n", path.c_str());
			}
		}
	}

	json_decref(root);
}

void Skins::loadCssValues() {
	std::string path = rack::asset::plugin(pluginInstance, "res/skin_css_values.json");
	if (access(path.c_str(), R_OK) != 0) {
		return;
	}

	json_error_t error;
	json_t* root = json_load_file(path.c_str(), 0, &error);
	if (!root) {
		WARN("Bogaudio: JSON error loading CSS values data from %s: %s\n", path.c_str(), error.text);
		return;
	}

	void* i = json_object_iter(root);
	while (i) {
		const char* skin = json_object_iter_key(i);
		json_t* values = json_object_iter_value(i);
		void* j = json_object_iter(values);
		css_values_map valuesMap;
		while (j) {
			const char* k = json_object_iter_key(j);
			const char* v = json_string_value(json_object_iter_value(j));
			valuesMap.insert(css_values_map::value_type(k, v));
			j = json_object_iter_next(values, j);
		}
		_skinCssValues.insert(skin_css_values_map::value_type(skin, valuesMap));
		i = json_object_iter_next(root, i);
	}

	json_decref(root);
}
