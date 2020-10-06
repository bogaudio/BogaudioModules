#pragma once

#include "Matrix44_shared.hpp"

namespace bogaudio {

typedef ChainableExpandableModule<Matrix44ExpanderMessage, Matrix44Element, 1, KnobMatrixModule> Matrix44Base;

struct Matrix44 : Matrix44Base {
	enum ParamsIds {
		MIX11_PARAM,
		MIX21_PARAM,
		MIX31_PARAM,
		MIX41_PARAM,
		MIX12_PARAM,
		MIX22_PARAM,
		MIX32_PARAM,
		MIX42_PARAM,
		MIX13_PARAM,
		MIX23_PARAM,
		MIX33_PARAM,
		MIX43_PARAM,
		MIX14_PARAM,
		MIX24_PARAM,
		MIX34_PARAM,
		MIX44_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		NUM_OUTPUTS
	};

	Matrix44() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configMatrixModule(4, 4, MIX11_PARAM, IN1_INPUT, OUT1_OUTPUT);
		configParam(MIX11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1A", "%", 0.0f, 100.0f);
		configParam(MIX21_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2A", "%", 0.0f, 100.0f);
		configParam(MIX31_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3A", "%", 0.0f, 100.0f);
		configParam(MIX41_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4A", "%", 0.0f, 100.0f);
		configParam(MIX12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1B", "%", 0.0f, 100.0f);
		configParam(MIX22_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2B", "%", 0.0f, 100.0f);
		configParam(MIX32_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3B", "%", 0.0f, 100.0f);
		configParam(MIX42_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4B", "%", 0.0f, 100.0f);
		configParam(MIX13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1C", "%", 0.0f, 100.0f);
		configParam(MIX23_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2C", "%", 0.0f, 100.0f);
		configParam(MIX33_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3C", "%", 0.0f, 100.0f);
		configParam(MIX43_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4C", "%", 0.0f, 100.0f);
		configParam(MIX14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1D", "%", 0.0f, 100.0f);
		configParam(MIX24_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2D", "%", 0.0f, 100.0f);
		configParam(MIX34_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3D", "%", 0.0f, 100.0f);
		configParam(MIX44_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4D", "%", 0.0f, 100.0f);

		setLocalElements({new Matrix44Element(NULL, NULL)});
		registerBase();
		setExpanderModelPredicate([](Model* m) { return m == modelMatrix44Cvm; });
	}

	void elementsChanged() override;
	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
