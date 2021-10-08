#pragma once

#include "lpg_common.hpp"
#include "dsp/filters/multimode.hpp"

extern Model* modelLLPG;

namespace bogaudio {

struct LLPG : BGModule {
	enum ParamsIds {
		RESPONSE_PARAM,
		SHAPE_PARAM,
		LPF_PARAM,
		VCA_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		GATE_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	static constexpr float maxFilterCutoff = 20000.0f;

	struct Engine {
		Trigger trigger;
		float gateSeconds = 0.0f;
		float gateElapsedSeconds = 0.0f;
		RiseFallShapedSlewLimiter slew;
		MultimodeFilter4 lpf;
		MultimodeFilter4 finalHP;
		Amplifier vca;

		void reset();
		void setSampleRate(float sr);
	};

	Engine* _engines[maxChannels] {};
	float _sampleRate = 0.0f;
	float _sampleTime = 0.0f;

	LLPG() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(RESPONSE_PARAM, 0.0f, 1.0f, 0.5f, "Response", "%", 0.0f, 100.0f);
		configParam(SHAPE_PARAM, -1.0f, 1.0f, -0.25f, "Shape");
		configParam<ScaledSquaringParamQuantity<(int)maxFilterCutoff>>(LPF_PARAM, 0.0f, 1.0f, 0.0f, "LPF cutoff", " HZ");
		configParam(VCA_PARAM, 0.0f, 1.0f, 0.0f, "VCA level", "%", 0.0f, 100.0f);

		configInput(GATE_INPUT, "GATE");
		configInput(IN_INPUT, "IN");

		configOutput(OUT_OUTPUT, "OUT");
	}

	void reset() override;
	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
