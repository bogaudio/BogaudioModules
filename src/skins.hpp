#pragma once

#include "rack.hpp"
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct DefaultSkinChangeListener {
	virtual void defaultSkinChanged(const std::string& skin) = 0;
};

struct Skin {
	std::string key;
	std::string display;

	Skin(const char* key, const char* display) :  key(key), display(display) {}
};

class Skins {
private:
	typedef std::unordered_map<std::string, std::string> css_values_map;
	typedef std::unordered_map<std::string, css_values_map> skin_css_values_map;
	typedef std::unordered_set<DefaultSkinChangeListener*> default_skin_listeners_set;
	std::vector<Skin> _available;
	std::string _default;
	skin_css_values_map _skinCssValues;
	default_skin_listeners_set _defaultSkinListeners;
	std::mutex _defaultSkinListenersLock;
	bool _loaded = false;
	std::mutex _instanceLock;

public:
	Skins() {}
	Skins(const Skins&) = delete;
	void operator=(const Skins&) = delete;

	static Skins& skins();
	inline const std::vector<Skin>& available() const { return _available; }
	inline const std::string& defaultKey() const { return _default; }
	bool validKey(const std::string& key) const;
	const char* skinCssValue(const std::string& skinKey, const std::string& valueKey) const;
	static NVGcolor cssColorToNVGColor(const char* color, const NVGcolor& ifError);

	void setDefaultSkin(std::string skinKey);
	void registerDefaultSkinChangeListener(DefaultSkinChangeListener* listener);
	void deregisterDefaultSkinChangeListener(DefaultSkinChangeListener* listener);

private:
	void loadSkins();
	void loadCssValues();
};
