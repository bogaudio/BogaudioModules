#pragma once

#include "PEQ14_shared.hpp"
#include "dsp/filters/multimode.hpp"

namespace bogaudio {

struct PEQ14XV : ExpanderModule<PEQ14ExpanderMessage, ExpandableModule<PEQ14ExpanderMessage, BGModule>> {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		MultimodeFilter* filters[14] {};
		 Amplifier amplifiers[14];

		 Engine();
		 ~Engine();
	};

	float _sampleRate = 1000.0f;
	Engine* _engines[maxChannels] {};
	Saturator _saturator;
	PEQ14ExpanderMessage* _baseMessage = NULL;

	PEQ14XV() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		setBaseModelPredicate([](Model* m) { return m == modelPEQ14 || m == modelPEQ14XO || m == modelPEQ14XR || m == modelPEQ14XV; });
		setExpanderModelPredicate([](Model* m) { return m == modelPEQ14XO || m == modelPEQ14XR || m == modelPEQ14XV; });
	}

	void sampleRateChange() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
