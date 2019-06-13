
#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct PitchModeListener {
	virtual void pitchModeChanged() = 0;
};

struct LFOBase : Module {
	bool _slowMode = false;
	PitchModeListener* _pitchModeListener = NULL;

	LFOBase(int np, int ni, int no, int nl) {
		config(np, ni, no, nl);
	}

	float getPitchOffset();
	void setFrequency(Param& frequency, Input& pitch, Phasor& phasor);
};

struct LFOFrequencyParamQuantity : FrequencyParamQuantity {
	float offset() override {
		LFOBase* lfo = static_cast<LFOBase*>(module);
		return lfo->getPitchOffset();
	}
};

} // namespace bogaudio
