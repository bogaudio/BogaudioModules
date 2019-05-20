
#include "rack0.hpp"

using namespace rack;

extern Plugin *pluginInstance;

namespace bogaudio {

struct TriggerOnLoadModule : Module {
	bool _triggerOnLoad = true;
	bool _shouldTriggerOnLoad = true;

	TriggerOnLoadModule(int numParams, int numInputs, int numOutputs, int numLights)
	{
	    config(numParams, numInputs, numLights);
	}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;

	virtual bool shouldTriggerOnNextLoad() = 0;
};

struct TriggerOnLoadMenuItem : MenuItem {
	TriggerOnLoadModule* _module;

	TriggerOnLoadMenuItem(TriggerOnLoadModule* module, const char* label)
	: _module(module)
	{
		this->text = label;
	}

	void onAction(const event::Action &e) override {
		_module->_triggerOnLoad = !_module->_triggerOnLoad;
	}

	void step() override {
		rightText = _module->_triggerOnLoad ? "✔" : "";
	}
};

} // namespace bogaudio
