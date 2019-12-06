#pragma once

#include "bogaudio.hpp"

using namespace rack;

namespace bogaudio {

struct AddressableSequenceModule : BGModule {
	int _polyInputID;
	int _clockInputID;
	int _selectInputID;
	Trigger _clock[maxChannels];
	Trigger _reset[maxChannels];
	Trigger _selectTrigger[maxChannels];
	bogaudio::dsp::Timer _timer[maxChannels];
	int _step[maxChannels];
	float _select[maxChannels] {};
	bool _selectOnClock = false;
	bool _triggeredSelect = false;

	AddressableSequenceModule(int clockInputID, int selectInputID)
	: _polyInputID(clockInputID)
	, _clockInputID(clockInputID)
	, _selectInputID(selectInputID)
	{
		reset();
		sampleRateChange();
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	int channels() override;
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
		OptionsMenuItem* p = new OptionsMenuItem("Polyphony channels from");
		p->addItem(OptionMenuItem("CLOCK input", [m]() { return m->_polyInputID == m->_clockInputID; }, [m]() { m->_polyInputID = m->_clockInputID; }));
		p->addItem(OptionMenuItem("SELECT input", [m]() { return m->_polyInputID == m->_selectInputID; }, [m]() { m->_polyInputID = m->_selectInputID; }));
		OptionsMenuItem::addToMenu(p, menu);
		menu->addChild(new BoolOptionMenuItem("Select on clock mode", [m]() { return &m->_selectOnClock; }));
		menu->addChild(new BoolOptionMenuItem("Triggered select mode", [m]() { return &m->_triggeredSelect; }));
	}
};

} // namespace bogaudio
