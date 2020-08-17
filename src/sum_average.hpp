#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

using namespace rack;

namespace bogaudio {

struct SumAverageModule : MatrixBaseModule {
	bool _sum = true;

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
};

struct SumAverageModuleWidget : MatrixBaseModuleWidget {
	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
