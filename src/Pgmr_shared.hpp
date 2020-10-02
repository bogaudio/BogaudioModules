#pragma once

#include <mutex>
#include <unordered_map>

#include "bogaudio.hpp"
#include "addressable_sequence.hpp"
#include "expanders.hpp"

extern Model* modelPgmr;
extern Model* modelPgmrX;

namespace bogaudio {

struct PgmrExpanderMessage : ExpanderMessage {
	int baseID = -1;
	int position = -1;
	float rangeOffset = 0.0f;
	float rangeScale = 10.0f;
};

struct PgmrStep {
	Param& aParam;
	Param& bParam;
	Param& cParam;
	Param& dParam;
	Light& selectedLight;
	Param& selectParam;
	Input& selectInput;
	Output& selectedOutput;

	Trigger triggers[BGModule::maxChannels];
	rack::dsp::PulseGenerator pulseGens[BGModule::maxChannels];
	float lightSum = 0.0f;

	PgmrStep(
		Param& aParam,
		Param& bParam,
		Param& cParam,
		Param& dParam,
		Light& selectedLight,
		Param& selectParam,
		Input& selectInput,
		Output& selectedOutput
	)
	: aParam(aParam)
	, bParam(bParam)
	, cParam(cParam)
	, dParam(dParam)
	, selectedLight(selectedLight)
	, selectParam(selectParam)
	, selectInput(selectInput)
	, selectedOutput(selectedOutput)
	{
	}

	void reset();
};

typedef ChainableRegistry<PgmrStep, 4> PgmrRegistry;

} // namespace bogaudio
