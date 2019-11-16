#pragma once

#include "bogaudio.hpp"

using namespace rack;

namespace bogaudio {

struct AddressableSequenceModule : BGModule {
	Trigger _clock[maxChannels];
	Trigger _reset[maxChannels];
	Trigger _selectTrigger[maxChannels];
	bogaudio::dsp::Timer _timer[maxChannels];
	int _step[maxChannels];
	float _select[maxChannels] {};
	bool _selectOnClock = false;
	bool _triggeredSelect = false;

	AddressableSequenceModule() {
		reset();
		sampleRateChange();
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	int nextStep(
		int c,
		Input& resetInput,
		Input& clockInput,
		Param& stepsParam,
		Param& directionParam,
		Param& selectParam,
		Input& selectInput
	);
};

struct AddressableSequenceModuleWidget : ModuleWidget {
	struct SelectOnClockMenuItem : MenuItem {
		AddressableSequenceModule* _module;

		SelectOnClockMenuItem(AddressableSequenceModule* module, const char* label)
		: _module(module)
		{
			this->text = label;
		}

		void onAction(const event::Action& e) override {
			_module->_selectOnClock = !_module->_selectOnClock;
		}

		void step() override {
			MenuItem::step();
			rightText = _module->_selectOnClock ? "✔" : "";
		}
	};

	struct TriggeredSelectMenuItem : MenuItem {
		AddressableSequenceModule* _module;

		TriggeredSelectMenuItem(AddressableSequenceModule* module, const char* label)
		: _module(module)
		{
			this->text = label;
		}

		void onAction(const event::Action& e) override {
			_module->_triggeredSelect = !_module->_triggeredSelect;
		}

		void step() override {
			MenuItem::step();
			rightText = _module->_triggeredSelect ? "✔" : "";
		}
	};

	void appendContextMenu(Menu* menu) override {
		AddressableSequenceModule* m = dynamic_cast<AddressableSequenceModule*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new SelectOnClockMenuItem(m, "Select on clock mode"));
		menu->addChild(new TriggeredSelectMenuItem(m, "Triggered select mode"));
	}
};

} // namespace bogaudio
