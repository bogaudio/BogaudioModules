#pragma once

#include "Matrix88_shared.hpp"

namespace bogaudio {

typedef ChainableExpanderModule<Matrix88ExpanderMessage, Matrix88Element, 1, BGModule> Matrix88CvBase;

struct Matrix88Cv : Matrix88CvBase {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		CV11_INPUT,
		CV21_INPUT,
		CV31_INPUT,
		CV41_INPUT,
		CV51_INPUT,
		CV61_INPUT,
		CV71_INPUT,
		CV81_INPUT,
		CV12_INPUT,
		CV22_INPUT,
		CV32_INPUT,
		CV42_INPUT,
		CV52_INPUT,
		CV62_INPUT,
		CV72_INPUT,
		CV82_INPUT,
		CV13_INPUT,
		CV23_INPUT,
		CV33_INPUT,
		CV43_INPUT,
		CV53_INPUT,
		CV63_INPUT,
		CV73_INPUT,
		CV83_INPUT,
		CV14_INPUT,
		CV24_INPUT,
		CV34_INPUT,
		CV44_INPUT,
		CV54_INPUT,
		CV64_INPUT,
		CV74_INPUT,
		CV84_INPUT,
		CV15_INPUT,
		CV25_INPUT,
		CV35_INPUT,
		CV45_INPUT,
		CV55_INPUT,
		CV65_INPUT,
		CV75_INPUT,
		CV85_INPUT,
		CV16_INPUT,
		CV26_INPUT,
		CV36_INPUT,
		CV46_INPUT,
		CV56_INPUT,
		CV66_INPUT,
		CV76_INPUT,
		CV86_INPUT,
		CV17_INPUT,
		CV27_INPUT,
		CV37_INPUT,
		CV47_INPUT,
		CV57_INPUT,
		CV67_INPUT,
		CV77_INPUT,
		CV87_INPUT,
		CV18_INPUT,
		CV28_INPUT,
		CV38_INPUT,
		CV48_INPUT,
		CV58_INPUT,
		CV68_INPUT,
		CV78_INPUT,
		CV88_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	Input** _cvs = NULL;

	Matrix88Cv() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		_cvs = new Input*[64];
		for (int i = 0; i < 64; ++i) {
			_cvs[i] = &inputs[CV11_INPUT + i];
		}
		setLocalElements({new Matrix88Element(NULL, _cvs)});
		setBaseModelPredicate([](Model* m) { return m == modelMatrix88 || m == modelMatrix88M; });
		setExpanderModelPredicate([](Model* m) { return m == modelMatrix88M; });
	}
	virtual ~Matrix88Cv() {
		delete[] _cvs;
	}

	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
