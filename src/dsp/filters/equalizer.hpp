#pragma once

#include "filters/multimode.hpp"

namespace bogaudio {
namespace dsp {

struct Equalizer : Filter {
	static constexpr float gainDb = 12.0f;
	static constexpr float cutDb = -36.0f;

	Amplifier _lowAmp;
	Amplifier _midAmp;
	Amplifier _highAmp;
	FourPoleButtworthLowpassFilter _lowFilter;
	TwoPoleButtworthBandpassFilter _midFilter;
	FourPoleButtworthHighpassFilter _highFilter;

	void setParams(
		float sampleRate,
		float lowDb,
		float midDb,
		float highDb
	);
	float next(float sample) override;
};

} // namespace dsp
} // namespace bogaudio
