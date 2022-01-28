#pragma once

#include "bogaudio.hpp"
#include "output_range.hpp"
#include "dsp/signal.hpp"

using namespace rack;

namespace bogaudio {

struct AddressableSequenceModule : BGModule {
	int _polyInputID = -1;
	int _clockInputID = -1;
	int _selectInputID = -1;
	Trigger _clock[maxChannels];
	NegativeTrigger _negativeClock[maxChannels];
	Trigger _reset[maxChannels];
	Trigger _selectTrigger[maxChannels];
	bogaudio::dsp::Timer _timer[maxChannels];
	int _step[maxChannels] {};
	float _select[maxChannels] {};
	bool _selectOnClock = false;
	bool _triggeredSelect = false;
	bool _reverseOnNegativeClock = false;
	bool _wrapSelectAtSteps = false;

	void setInputIDs(int clockInputID, int selectInputID);
	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	int channels() override;
	int nextStep(
		int c,
		Input* resetInput,
		Input& clockInput,
		Param* stepsParam,
		Param& directionParam,
		Param* selectParam,
		Input& selectInput,
		int n = 8
	);
	int setStep(int c, int i, int n = 8);
};

struct AddressableSequenceBaseModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

struct AddressableSequenceModuleWidget : AddressableSequenceBaseModuleWidget {
	void contextMenu(Menu* menu) override;
};

typedef OutputRangeModule<AddressableSequenceModule> OutputRangeAddressableSequenceModule;

} // namespace bogaudio
