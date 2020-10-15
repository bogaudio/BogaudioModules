#pragma once

#include "rack.hpp"
#include "skins.hpp"
#include <string>
#include <vector>

using namespace rack;

namespace bogaudio {

struct SkinChangeListener {
	virtual void skinChanged(const std::string& skin) = 0;
};

struct BGModule : Module {
	int _modulationSteps = 100;
	int _steps = -1;
	bool _initialized = false;

	static constexpr int maxChannels = PORT_MAX_CHANNELS;
	int _channels = 0;
	float _inverseChannels = 0.0f;

	bool _skinnable = true;
	std::string _skin = "default";
	std::vector<SkinChangeListener*> _skinChangeListeners;

	BGModule() {
	}
	virtual ~BGModule() {
		while (_channels >= 1) {
			removeChannel(_channels - 1);
			--_channels;
		}
	}

	void onReset() override;
	void onSampleRateChange() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void process(const ProcessArgs& args) override;

	virtual void reset() {}
	virtual void sampleRateChange() {}
	virtual json_t* toJson(json_t* root) { return root; }
	virtual void fromJson(json_t* root) {}
	virtual bool active() { return true; }
	virtual int channels() { return 1; }
	virtual void channelsChanged(int before, int after) {}
	virtual void addChannel(int c) {}
	virtual void removeChannel(int c) {}
	virtual void processAlways(const ProcessArgs& args) {} // called before modulate()!
	virtual void modulate() {}
	virtual void modulateChannel(int c) {}
	virtual void processAll(const ProcessArgs& args) {}
	virtual void processChannel(const ProcessArgs& args, int c) {}
	virtual void postProcess(const ProcessArgs& args) {}
	virtual void postProcessAlways(const ProcessArgs& args) {} // modulate() may not have been called.

	void setSkin(std::string skin);
	void addSkinChangeListener(SkinChangeListener* listener);
};

struct BGModuleWidget : ModuleWidget, SkinChangeListener, DefaultSkinChangeListener {
	bool _skinnable = true;
	SvgPanel* _panel = NULL;
	Vec _size;
	std::string _slug;
	std::string _loadedSkin;

	BGModuleWidget();
	virtual ~BGModuleWidget();

	void appendContextMenu(Menu* menu) override;
	void addParam(ParamWidget* param);
	void addInput(PortWidget* input);
	void addOutput(PortWidget* output);

	virtual void contextMenu(Menu* menu) {}

	void skinChanged(const std::string& skin) override;
	void defaultSkinChanged(const std::string& skin) override;
	void setPanel(Vec size, const std::string slug, bool skinnable = true);
	void updatePanel();
	void createScrews();
};

} // namespace bogaudio
