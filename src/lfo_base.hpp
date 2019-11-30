
#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct PitchModeListener {
	virtual void pitchModeChanged() = 0;
};

struct LFOBase : BGModule {
	bool _slowMode = false;
	PitchModeListener* _pitchModeListener = NULL;

	struct LFOFrequencyParamQuantity : FrequencyParamQuantity {
		float offset() override;
	};

	LFOBase(int np, int ni, int no, int nl = 0) {
		config(np, ni, no, nl);
	}

	float getPitchOffset();
	void setFrequency(Param& frequency, Input& pitch, Phasor& phasor, int c = 0); // FIXME
};


} // namespace bogaudio
