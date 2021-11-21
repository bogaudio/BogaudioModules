#pragma once

#include "Matrix44_shared.hpp"

namespace bogaudio {

typedef ChainableExpanderModule<Matrix44ExpanderMessage, Matrix44Element, 1, MutesMatrixExpanderModule> Matrix44CvmBase;

struct Matrix44Cvm : Matrix44CvmBase {
	enum ParamsIds {
		MUTE11_PARAM,
		MUTE21_PARAM,
		MUTE31_PARAM,
		MUTE41_PARAM,
		MUTE12_PARAM,
		MUTE22_PARAM,
		MUTE32_PARAM,
		MUTE42_PARAM,
		MUTE13_PARAM,
		MUTE23_PARAM,
		MUTE33_PARAM,
		MUTE43_PARAM,
		MUTE14_PARAM,
		MUTE24_PARAM,
		MUTE34_PARAM,
		MUTE44_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV11_INPUT,
		CV21_INPUT,
		CV31_INPUT,
		CV41_INPUT,
		CV12_INPUT,
		CV22_INPUT,
		CV32_INPUT,
		CV42_INPUT,
		CV13_INPUT,
		CV23_INPUT,
		CV33_INPUT,
		CV43_INPUT,
		CV14_INPUT,
		CV24_INPUT,
		CV34_INPUT,
		CV44_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	Param** _mutes = NULL;
	Input** _cvs = NULL;

	Matrix44Cvm() {
		const std::vector<std::string> muteLabels = {"Unmuted", "Muted", "Soloed", "Soloed"};
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configSwitch(MUTE11_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1A", muteLabels);
		configSwitch(MUTE21_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2A", muteLabels);
		configSwitch(MUTE31_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3A", muteLabels);
		configSwitch(MUTE41_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4A", muteLabels);
		configSwitch(MUTE12_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1B", muteLabels);
		configSwitch(MUTE22_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2B", muteLabels);
		configSwitch(MUTE32_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3B", muteLabels);
		configSwitch(MUTE42_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4B", muteLabels);
		configSwitch(MUTE13_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1C", muteLabels);
		configSwitch(MUTE23_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2C", muteLabels);
		configSwitch(MUTE33_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3C", muteLabels);
		configSwitch(MUTE43_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4C", muteLabels);
		configSwitch(MUTE14_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1D", muteLabels);
		configSwitch(MUTE24_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2D", muteLabels);
		configSwitch(MUTE34_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3D", muteLabels);
		configSwitch(MUTE44_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4D", muteLabels);

		configInput(CV11_INPUT, "Level 1A CV");
		configInput(CV21_INPUT, "Level 2A CV");
		configInput(CV31_INPUT, "Level 3A CV");
		configInput(CV41_INPUT, "Level 4A CV");
		configInput(CV12_INPUT, "Level 1B CV");
		configInput(CV22_INPUT, "Level 2B CV");
		configInput(CV32_INPUT, "Level 3B CV");
		configInput(CV42_INPUT, "Level 4B CV");
		configInput(CV13_INPUT, "Level 1C CV");
		configInput(CV23_INPUT, "Level 2C CV");
		configInput(CV33_INPUT, "Level 3C CV");
		configInput(CV43_INPUT, "Level 4C CV");
		configInput(CV14_INPUT, "Level 1D CV");
		configInput(CV24_INPUT, "Level 2D CV");
		configInput(CV34_INPUT, "Level 3D CV");
		configInput(CV44_INPUT, "Level 4D CV");

		_mutes = new Param*[16];
		_cvs = new Input*[16];
		for (int i = 0; i < 16; ++i) {
			_mutes[i] = &params[MUTE11_PARAM + i];
			_cvs[i] = &inputs[CV11_INPUT + i];
		}
		setLocalElements({new Matrix44Element(_mutes, _cvs, &_soloByColumns)});
		setBaseModelPredicate([](Model* m) { return m == modelMatrix44; });
	}
	virtual ~Matrix44Cvm() {
		delete[] _mutes;
		delete[] _cvs;
	}

	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
