#pragma once

#include "bogaudio.hpp"
#include "expanders.hpp"
#include "parametric_eq.hpp"

extern Model* modelPEQ14;
extern Model* modelPEQ14XO;
extern Model* modelPEQ14XR;
extern Model* modelPEQ14XV;

namespace bogaudio {

struct PEQ14ExpanderMessage : ExpanderMessage {
	bool valid = false;
	float outs[BGModule::maxChannels][14];
	float frequencies[BGModule::maxChannels][14];
	float levels[BGModule::maxChannels][14];
	float bandwidths[BGModule::maxChannels];
	bool lowLP = false;
	bool highHP = false;

	PEQ14ExpanderMessage() {
		reset();
	}

	void reset() {
		valid = false;
		const int n = BGModule::maxChannels * 14;
		std::fill((float*)outs, (float*)outs + n, 0.0f);
		std::fill((float*)frequencies, (float*)frequencies + n, 0.0f);
		std::fill((float*)levels, (float*)levels + n, 0.0f);
		std::fill((float*)bandwidths, (float*)bandwidths + BGModule::maxChannels, 0.0f);
		lowLP = false;
		highHP = false;
	}

	void copyTo(PEQ14ExpanderMessage* o) {
		o->valid = valid;
		const int n = BGModule::maxChannels * 14;
		std::copy((float*)outs, (float*)outs + n, (float*)o->outs);
		std::copy((float*)frequencies, (float*)frequencies + n, (float*)o->frequencies);
		std::copy((float*)levels, (float*)levels + n, (float*)o->levels);
		std::copy((float*)bandwidths, (float*)bandwidths + BGModule::maxChannels, (float*)o->bandwidths);
		o->lowLP = lowLP;
		o->highHP = highHP;
	}
};

} // namespace bogaudio
