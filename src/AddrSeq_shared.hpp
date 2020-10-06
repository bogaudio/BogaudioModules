#pragma once

#include "bogaudio.hpp"
#include "addressable_sequence.hpp"
#include "expanders.hpp"

extern Model* modelAddrSeq;
extern Model* modelAddrSeqX;

namespace bogaudio {

struct AddrSeqExpanderMessage : ChainableExpanderMessage {
	float rangeOffset = 0.0f;
	float rangeScale = 10.0f;
};

struct AddrSeqStep {
	Param& param;
	Light& light;
	float lightSum = 0.0f;

	AddrSeqStep(
		Param& param,
		Light& light
	)
	: param(param)
	, light(light)
	{}
};

typedef ChainableRegistry<AddrSeqStep, 8> AddrSeqRegistry;

} // namespace bogaudio
