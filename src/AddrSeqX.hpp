#pragma once

#include "AddrSeq_shared.hpp"

namespace bogaudio {

typedef ChainableExpanderModule<AddrSeqExpanderMessage, AddrSeqStep, 8, BGModule> AddrSeqXBase;

struct AddrSeqX : AddrSeqXBase, OutputRange {
	enum ParamsIds {
		OUT1_PARAM,
		OUT2_PARAM,
		OUT3_PARAM,
		OUT4_PARAM,
		OUT5_PARAM,
		OUT6_PARAM,
		OUT7_PARAM,
		OUT8_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	enum LightsIds {
		OUT1_LIGHT,
		OUT2_LIGHT,
		OUT3_LIGHT,
		OUT4_LIGHT,
		OUT5_LIGHT,
		OUT6_LIGHT,
		OUT7_LIGHT,
		OUT8_LIGHT,
		NUM_LIGHTS
	};

	AddrSeqX() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<OutputRangeParamQuantity>(OUT1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1", " V");
		configParam<OutputRangeParamQuantity>(OUT2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2", " V");
		configParam<OutputRangeParamQuantity>(OUT3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3", " V");
		configParam<OutputRangeParamQuantity>(OUT4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4", " V");
		configParam<OutputRangeParamQuantity>(OUT5_PARAM, -1.0f, 1.0f, 0.0f, "Step 5", " V");
		configParam<OutputRangeParamQuantity>(OUT6_PARAM, -1.0f, 1.0f, 0.0f, "Step 6", " V");
		configParam<OutputRangeParamQuantity>(OUT7_PARAM, -1.0f, 1.0f, 0.0f, "Step 7", " V");
		configParam<OutputRangeParamQuantity>(OUT8_PARAM, -1.0f, 1.0f, 0.0f, "Step 8", " V");

		setLocalElements({
			new AddrSeqStep(params[OUT1_PARAM], lights[OUT1_LIGHT]),
			new AddrSeqStep(params[OUT2_PARAM], lights[OUT2_LIGHT]),
			new AddrSeqStep(params[OUT3_PARAM], lights[OUT3_LIGHT]),
			new AddrSeqStep(params[OUT4_PARAM], lights[OUT4_LIGHT]),
			new AddrSeqStep(params[OUT5_PARAM], lights[OUT5_LIGHT]),
			new AddrSeqStep(params[OUT6_PARAM], lights[OUT6_LIGHT]),
			new AddrSeqStep(params[OUT7_PARAM], lights[OUT7_LIGHT]),
			new AddrSeqStep(params[OUT8_PARAM], lights[OUT8_LIGHT])
		});
		setBaseModelPredicate([](Model* m) { return m == modelAddrSeq || m == modelAddrSeqX; });
		setExpanderModelPredicate([](Model* m) { return m == modelAddrSeqX; });
	}

	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
