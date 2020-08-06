#pragma once

#include "rack.hpp"
#include <string>

using namespace rack;

namespace bogaudio {

struct BGModuleWidget;

struct BGModule : Module {
	int _modulationSteps = 100;
	int _steps = -1;

	static constexpr int maxChannels = PORT_MAX_CHANNELS;
	int _channels = 0;
	float _inverseChannels = 0.0f;

	bool _skinnable = true;
	std::string _skin = "default";
	BGModuleWidget* _skinChangeListener = NULL;

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
	void setSkinChangeListener(BGModuleWidget* widget);
};

struct BGModuleWidget : ModuleWidget {
	SvgPanel* _panel = NULL;
	Vec _size;
	std::string _slug;

	void appendContextMenu(Menu* menu) override;

	virtual void contextMenu(Menu* menu) {}

	void skinChanged();
	void setPanel(Vec size, const std::string slug);
	void updatePanel();
};

} // namespace bogaudio
