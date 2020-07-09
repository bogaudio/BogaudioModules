#pragma once

#include "filters/filter.hpp"

namespace bogaudio {
namespace dsp {

struct ComplexBiquadFilter : BiquadFilter<float> {
	float _gain = 1.0f;
	float _zeroRadius = 1.0f;
	float _zeroTheta = M_PI;
	float _poleRadius = 0.9f;
	float _poleTheta = 0.0f;

	ComplexBiquadFilter() {
		updateParams();
	}

	void setComplexParams(
		float gain,
		float zeroRadius,
		float zeroTheta,
		float poleRadius,
		float poleTheta
	);
	void updateParams();
};

struct MultipoleFilter : Filter {
	enum Type {
		LP_TYPE,
		HP_TYPE
	};

	static constexpr int maxPoles = 20;
	static constexpr float maxRipple = 0.29f;
	Type _type = LP_TYPE;
	int _poles = 1;
	float _sampleRate = 0.0f;
	float _cutoff = 0.0f;
	float _ripple = 0.0f;
	BiquadFilter<double> _biquads[maxPoles / 2];

	MultipoleFilter() {}

	void setParams(
		Type type,
		int poles,
		float sampleRate,
		float cutoff,
		float ripple // FIXME: using this with more than two poles creates large gain, need compensation.
	);
	float next(float sample) override;
};

} // namespace dsp
} // namespace bogaudio
