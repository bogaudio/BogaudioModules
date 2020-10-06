#pragma once

#include "Matrix44_shared.hpp"

namespace bogaudio {

typedef ChainableExpanderModule<Matrix44ExpanderMessage, Matrix44Element, 1, BGModule> Matrix44CvmBase;

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
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(MUTE11_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1A");
		configParam(MUTE21_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2A");
		configParam(MUTE31_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3A");
		configParam(MUTE41_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4A");
		configParam(MUTE12_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1B");
		configParam(MUTE22_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2B");
		configParam(MUTE32_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3B");
		configParam(MUTE42_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4B");
		configParam(MUTE13_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1C");
		configParam(MUTE23_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2C");
		configParam(MUTE33_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3C");
		configParam(MUTE43_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4C");
		configParam(MUTE14_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1D");
		configParam(MUTE24_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2D");
		configParam(MUTE34_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3D");
		configParam(MUTE44_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4D");

		_mutes = new Param*[16];
		_cvs = new Input*[16];
		for (int i = 0; i < 16; ++i) {
			_mutes[i] = &params[MUTE11_PARAM + i];
			_cvs[i] = &inputs[CV11_INPUT + i];
		}
		setLocalElements({new Matrix44Element(_mutes, _cvs)});
		setBaseModelPredicate([](Model* m) { return m == modelMatrix44; });
	}
	virtual ~Matrix44Cvm() {
		delete[] _mutes;
		delete[] _cvs;
	}

	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
