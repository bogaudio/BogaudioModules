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

		configInput(CV11_INPUT, "Level 1A CV");
		configInput(CV21_INPUT, "Level 2A CV");
		configInput(CV31_INPUT, "Level 3A CV");
		configInput(CV41_INPUT, "Level 4A CV");
		configInput(CV51_INPUT, "Level 5A CV");
		configInput(CV61_INPUT, "Level 6A CV");
		configInput(CV71_INPUT, "Level 7A CV");
		configInput(CV81_INPUT, "Level 8A CV");
		configInput(CV12_INPUT, "Level 1B CV");
		configInput(CV22_INPUT, "Level 2B CV");
		configInput(CV32_INPUT, "Level 3B CV");
		configInput(CV42_INPUT, "Level 4B CV");
		configInput(CV52_INPUT, "Level 5B CV");
		configInput(CV62_INPUT, "Level 6B CV");
		configInput(CV72_INPUT, "Level 7B CV");
		configInput(CV82_INPUT, "Level 8B CV");
		configInput(CV13_INPUT, "Level 1C CV");
		configInput(CV23_INPUT, "Level 2C CV");
		configInput(CV33_INPUT, "Level 3C CV");
		configInput(CV43_INPUT, "Level 4C CV");
		configInput(CV53_INPUT, "Level 5C CV");
		configInput(CV63_INPUT, "Level 6C CV");
		configInput(CV73_INPUT, "Level 7C CV");
		configInput(CV83_INPUT, "Level 8C CV");
		configInput(CV14_INPUT, "Level 1D CV");
		configInput(CV24_INPUT, "Level 2D CV");
		configInput(CV34_INPUT, "Level 3D CV");
		configInput(CV44_INPUT, "Level 4D CV");
		configInput(CV54_INPUT, "Level 5D CV");
		configInput(CV64_INPUT, "Level 6D CV");
		configInput(CV74_INPUT, "Level 7D CV");
		configInput(CV84_INPUT, "Level 8D CV");
		configInput(CV15_INPUT, "Level 1E CV");
		configInput(CV25_INPUT, "Level 2E CV");
		configInput(CV35_INPUT, "Level 3E CV");
		configInput(CV45_INPUT, "Level 4E CV");
		configInput(CV55_INPUT, "Level 5E CV");
		configInput(CV65_INPUT, "Level 6E CV");
		configInput(CV75_INPUT, "Level 7E CV");
		configInput(CV85_INPUT, "Level 8E CV");
		configInput(CV16_INPUT, "Level 1F CV");
		configInput(CV26_INPUT, "Level 2F CV");
		configInput(CV36_INPUT, "Level 3F CV");
		configInput(CV46_INPUT, "Level 4F CV");
		configInput(CV56_INPUT, "Level 5F CV");
		configInput(CV66_INPUT, "Level 6F CV");
		configInput(CV76_INPUT, "Level 7F CV");
		configInput(CV86_INPUT, "Level 8F CV");
		configInput(CV17_INPUT, "Level 1G CV");
		configInput(CV27_INPUT, "Level 2G CV");
		configInput(CV37_INPUT, "Level 3G CV");
		configInput(CV47_INPUT, "Level 4G CV");
		configInput(CV57_INPUT, "Level 5G CV");
		configInput(CV67_INPUT, "Level 6G CV");
		configInput(CV77_INPUT, "Level 7G CV");
		configInput(CV87_INPUT, "Level 8G CV");
		configInput(CV18_INPUT, "Level 1H CV");
		configInput(CV28_INPUT, "Level 2H CV");
		configInput(CV38_INPUT, "Level 3H CV");
		configInput(CV48_INPUT, "Level 4H CV");
		configInput(CV58_INPUT, "Level 5H CV");
		configInput(CV68_INPUT, "Level 6H CV");
		configInput(CV78_INPUT, "Level 7H CV");
		configInput(CV88_INPUT, "Level 8H CV");

		_cvs = new Input*[64];
		for (int i = 0; i < 64; ++i) {
			_cvs[i] = &inputs[CV11_INPUT + i];
		}
		setLocalElements({new Matrix88Element(NULL, _cvs, NULL)});
		setBaseModelPredicate([](Model* m) { return m == modelMatrix88 || m == modelMatrix88M; });
		setExpanderModelPredicate([](Model* m) { return m == modelMatrix88M; });
	}
	virtual ~Matrix88Cv() {
		delete[] _cvs;
	}

	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
