#pragma once

#include "Matrix88_shared.hpp"

namespace bogaudio {

typedef ChainableExpanderModule<Matrix88ExpanderMessage, Matrix88Element, 1, MutesMatrixExpanderModule> Matrix88MBase;

struct Matrix88M : Matrix88MBase {
	enum ParamsIds {
		MUTE11_PARAM,
		MUTE21_PARAM,
		MUTE31_PARAM,
		MUTE41_PARAM,
		MUTE51_PARAM,
		MUTE61_PARAM,
		MUTE71_PARAM,
		MUTE81_PARAM,
		MUTE12_PARAM,
		MUTE22_PARAM,
		MUTE32_PARAM,
		MUTE42_PARAM,
		MUTE52_PARAM,
		MUTE62_PARAM,
		MUTE72_PARAM,
		MUTE82_PARAM,
		MUTE13_PARAM,
		MUTE23_PARAM,
		MUTE33_PARAM,
		MUTE43_PARAM,
		MUTE53_PARAM,
		MUTE63_PARAM,
		MUTE73_PARAM,
		MUTE83_PARAM,
		MUTE14_PARAM,
		MUTE24_PARAM,
		MUTE34_PARAM,
		MUTE44_PARAM,
		MUTE54_PARAM,
		MUTE64_PARAM,
		MUTE74_PARAM,
		MUTE84_PARAM,
		MUTE15_PARAM,
		MUTE25_PARAM,
		MUTE35_PARAM,
		MUTE45_PARAM,
		MUTE55_PARAM,
		MUTE65_PARAM,
		MUTE75_PARAM,
		MUTE85_PARAM,
		MUTE16_PARAM,
		MUTE26_PARAM,
		MUTE36_PARAM,
		MUTE46_PARAM,
		MUTE56_PARAM,
		MUTE66_PARAM,
		MUTE76_PARAM,
		MUTE86_PARAM,
		MUTE17_PARAM,
		MUTE27_PARAM,
		MUTE37_PARAM,
		MUTE47_PARAM,
		MUTE57_PARAM,
		MUTE67_PARAM,
		MUTE77_PARAM,
		MUTE87_PARAM,
		MUTE18_PARAM,
		MUTE28_PARAM,
		MUTE38_PARAM,
		MUTE48_PARAM,
		MUTE58_PARAM,
		MUTE68_PARAM,
		MUTE78_PARAM,
		MUTE88_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	Param** _mutes = NULL;

	Matrix88M() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(MUTE11_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1A");
		configParam(MUTE21_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2A");
		configParam(MUTE31_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3A");
		configParam(MUTE41_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4A");
		configParam(MUTE51_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5A");
		configParam(MUTE61_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6A");
		configParam(MUTE71_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7A");
		configParam(MUTE81_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8A");

		configParam(MUTE12_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1B");
		configParam(MUTE22_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2B");
		configParam(MUTE32_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3B");
		configParam(MUTE42_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4B");
		configParam(MUTE52_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5B");
		configParam(MUTE62_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6B");
		configParam(MUTE72_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7B");
		configParam(MUTE82_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8B");

		configParam(MUTE13_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1C");
		configParam(MUTE23_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2C");
		configParam(MUTE33_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3C");
		configParam(MUTE43_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4C");
		configParam(MUTE53_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5C");
		configParam(MUTE63_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6C");
		configParam(MUTE73_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7C");
		configParam(MUTE83_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8C");

		configParam(MUTE14_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1D");
		configParam(MUTE24_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2D");
		configParam(MUTE34_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3D");
		configParam(MUTE44_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4D");
		configParam(MUTE54_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5D");
		configParam(MUTE64_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6D");
		configParam(MUTE74_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7D");
		configParam(MUTE84_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8D");

		configParam(MUTE15_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1E");
		configParam(MUTE25_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2E");
		configParam(MUTE35_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3E");
		configParam(MUTE45_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4E");
		configParam(MUTE55_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5E");
		configParam(MUTE65_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6E");
		configParam(MUTE75_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7E");
		configParam(MUTE85_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8E");

		configParam(MUTE16_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1F");
		configParam(MUTE26_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2F");
		configParam(MUTE36_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3F");
		configParam(MUTE46_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4F");
		configParam(MUTE56_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5F");
		configParam(MUTE66_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6F");
		configParam(MUTE76_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7F");
		configParam(MUTE86_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8F");

		configParam(MUTE17_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1G");
		configParam(MUTE27_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2G");
		configParam(MUTE37_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3G");
		configParam(MUTE47_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4G");
		configParam(MUTE57_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5G");
		configParam(MUTE67_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6G");
		configParam(MUTE77_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7G");
		configParam(MUTE87_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8G");

		configParam(MUTE18_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1H");
		configParam(MUTE28_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2H");
		configParam(MUTE38_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3H");
		configParam(MUTE48_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4H");
		configParam(MUTE58_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5H");
		configParam(MUTE68_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6H");
		configParam(MUTE78_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7H");
		configParam(MUTE88_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8H");

		_mutes = new Param*[64];
		for (int i = 0; i < 64; ++i) {
			_mutes[i] = &params[MUTE11_PARAM + i];
		}
		setLocalElements({new Matrix88Element(_mutes, NULL, &_soloByColumns)});
		setBaseModelPredicate([](Model* m) { return m == modelMatrix88 || m == modelMatrix88Cv; });
		setExpanderModelPredicate([](Model* m) { return m == modelMatrix88Cv; });
	}
	virtual ~Matrix88M() {
		delete[] _mutes;
	}

	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
