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

		configInput(CV11_INPUT, "CV11");
		configInput(CV21_INPUT, "CV21");
		configInput(CV31_INPUT, "CV31");
		configInput(CV41_INPUT, "CV41");
		configInput(CV51_INPUT, "CV51");
		configInput(CV61_INPUT, "CV61");
		configInput(CV71_INPUT, "CV71");
		configInput(CV81_INPUT, "CV81");
		configInput(CV12_INPUT, "CV12");
		configInput(CV22_INPUT, "CV22");
		configInput(CV32_INPUT, "CV32");
		configInput(CV42_INPUT, "CV42");
		configInput(CV52_INPUT, "CV52");
		configInput(CV62_INPUT, "CV62");
		configInput(CV72_INPUT, "CV72");
		configInput(CV82_INPUT, "CV82");
		configInput(CV13_INPUT, "CV13");
		configInput(CV23_INPUT, "CV23");
		configInput(CV33_INPUT, "CV33");
		configInput(CV43_INPUT, "CV43");
		configInput(CV53_INPUT, "CV53");
		configInput(CV63_INPUT, "CV63");
		configInput(CV73_INPUT, "CV73");
		configInput(CV83_INPUT, "CV83");
		configInput(CV14_INPUT, "CV14");
		configInput(CV24_INPUT, "CV24");
		configInput(CV34_INPUT, "CV34");
		configInput(CV44_INPUT, "CV44");
		configInput(CV54_INPUT, "CV54");
		configInput(CV64_INPUT, "CV64");
		configInput(CV74_INPUT, "CV74");
		configInput(CV84_INPUT, "CV84");
		configInput(CV15_INPUT, "CV15");
		configInput(CV25_INPUT, "CV25");
		configInput(CV35_INPUT, "CV35");
		configInput(CV45_INPUT, "CV45");
		configInput(CV55_INPUT, "CV55");
		configInput(CV65_INPUT, "CV65");
		configInput(CV75_INPUT, "CV75");
		configInput(CV85_INPUT, "CV85");
		configInput(CV16_INPUT, "CV16");
		configInput(CV26_INPUT, "CV26");
		configInput(CV36_INPUT, "CV36");
		configInput(CV46_INPUT, "CV46");
		configInput(CV56_INPUT, "CV56");
		configInput(CV66_INPUT, "CV66");
		configInput(CV76_INPUT, "CV76");
		configInput(CV86_INPUT, "CV86");
		configInput(CV17_INPUT, "CV17");
		configInput(CV27_INPUT, "CV27");
		configInput(CV37_INPUT, "CV37");
		configInput(CV47_INPUT, "CV47");
		configInput(CV57_INPUT, "CV57");
		configInput(CV67_INPUT, "CV67");
		configInput(CV77_INPUT, "CV77");
		configInput(CV87_INPUT, "CV87");
		configInput(CV18_INPUT, "CV18");
		configInput(CV28_INPUT, "CV28");
		configInput(CV38_INPUT, "CV38");
		configInput(CV48_INPUT, "CV48");
		configInput(CV58_INPUT, "CV58");
		configInput(CV68_INPUT, "CV68");
		configInput(CV78_INPUT, "CV78");
		configInput(CV88_INPUT, "CV88");

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
