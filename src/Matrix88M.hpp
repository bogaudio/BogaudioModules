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
		const std::vector<std::string> muteLabels = {"Unmuted", "Muted", "Soloed", "Soloed"};
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configSwitch(MUTE11_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1A", muteLabels);
		configSwitch(MUTE21_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2A", muteLabels);
		configSwitch(MUTE31_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3A", muteLabels);
		configSwitch(MUTE41_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4A", muteLabels);
		configSwitch(MUTE51_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5A", muteLabels);
		configSwitch(MUTE61_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6A", muteLabels);
		configSwitch(MUTE71_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7A", muteLabels);
		configSwitch(MUTE81_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8A", muteLabels);

		configSwitch(MUTE12_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1B", muteLabels);
		configSwitch(MUTE22_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2B", muteLabels);
		configSwitch(MUTE32_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3B", muteLabels);
		configSwitch(MUTE42_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4B", muteLabels);
		configSwitch(MUTE52_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5B", muteLabels);
		configSwitch(MUTE62_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6B", muteLabels);
		configSwitch(MUTE72_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7B", muteLabels);
		configSwitch(MUTE82_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8B", muteLabels);

		configSwitch(MUTE13_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1C", muteLabels);
		configSwitch(MUTE23_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2C", muteLabels);
		configSwitch(MUTE33_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3C", muteLabels);
		configSwitch(MUTE43_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4C", muteLabels);
		configSwitch(MUTE53_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5C", muteLabels);
		configSwitch(MUTE63_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6C", muteLabels);
		configSwitch(MUTE73_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7C", muteLabels);
		configSwitch(MUTE83_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8C", muteLabels);

		configSwitch(MUTE14_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1D", muteLabels);
		configSwitch(MUTE24_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2D", muteLabels);
		configSwitch(MUTE34_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3D", muteLabels);
		configSwitch(MUTE44_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4D", muteLabels);
		configSwitch(MUTE54_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5D", muteLabels);
		configSwitch(MUTE64_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6D", muteLabels);
		configSwitch(MUTE74_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7D", muteLabels);
		configSwitch(MUTE84_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8D", muteLabels);

		configSwitch(MUTE15_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1E", muteLabels);
		configSwitch(MUTE25_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2E", muteLabels);
		configSwitch(MUTE35_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3E", muteLabels);
		configSwitch(MUTE45_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4E", muteLabels);
		configSwitch(MUTE55_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5E", muteLabels);
		configSwitch(MUTE65_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6E", muteLabels);
		configSwitch(MUTE75_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7E", muteLabels);
		configSwitch(MUTE85_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8E", muteLabels);

		configSwitch(MUTE16_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1F", muteLabels);
		configSwitch(MUTE26_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2F", muteLabels);
		configSwitch(MUTE36_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3F", muteLabels);
		configSwitch(MUTE46_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4F", muteLabels);
		configSwitch(MUTE56_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5F", muteLabels);
		configSwitch(MUTE66_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6F", muteLabels);
		configSwitch(MUTE76_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7F", muteLabels);
		configSwitch(MUTE86_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8F", muteLabels);

		configSwitch(MUTE17_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1G", muteLabels);
		configSwitch(MUTE27_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2G", muteLabels);
		configSwitch(MUTE37_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3G", muteLabels);
		configSwitch(MUTE47_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4G", muteLabels);
		configSwitch(MUTE57_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5G", muteLabels);
		configSwitch(MUTE67_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6G", muteLabels);
		configSwitch(MUTE77_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7G", muteLabels);
		configSwitch(MUTE87_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8G", muteLabels);

		configSwitch(MUTE18_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1H", muteLabels);
		configSwitch(MUTE28_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2H", muteLabels);
		configSwitch(MUTE38_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3H", muteLabels);
		configSwitch(MUTE48_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4H", muteLabels);
		configSwitch(MUTE58_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5H", muteLabels);
		configSwitch(MUTE68_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6H", muteLabels);
		configSwitch(MUTE78_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7H", muteLabels);
		configSwitch(MUTE88_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8H", muteLabels);

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
