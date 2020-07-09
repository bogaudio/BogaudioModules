#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/equalizer.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct FollowerBase : BGModule {
	typedef EQParamQuantity EFGainParamQuantity;

	static constexpr float efGainMinDecibels = Equalizer::cutDb;
	static constexpr float efGainMaxDecibels = Equalizer::gainDb;

	float sensitivity(Param& dampParam, Input* dampInput, int c);
	float gain(Param& gainParam, Input* gainInput, int c);
};

} // namespace bogaudio
