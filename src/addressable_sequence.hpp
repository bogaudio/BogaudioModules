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
	void appendContextMenu(Menu* menu) override {
		AddressableSequenceModule* m = dynamic_cast<AddressableSequenceModule*>(module);
		assert(m);
		menu->addChild(new MenuLabel());
		menu->addChild(new BoolOptionMenuItem("Select on clock mode", [m]() { return &m->_selectOnClock; }));
		menu->addChild(new BoolOptionMenuItem("Triggered select mode", [m]() { return &m->_triggeredSelect; }));
	}
};

} // namespace bogaudio
