#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

using namespace rack;

namespace bogaudio {

struct MatrixBaseModule : BGModule {
	enum Clipping {
		SOFT_CLIPPING,
		HARD_CLIPPING
	};

	Clipping _clippingMode = SOFT_CLIPPING;
	float _inputGainDb = 0.0f;
	float _inputGainLevel = 1.0f;

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void modulate() override;
};

struct MatrixBaseModuleWidget : ModuleWidget {
	void appendContextMenu(Menu* menu) override;
};

struct MatrixModule : MatrixBaseModule {
	static constexpr int maxN = 16;
	int _n;
	int _firstParamID;
	int _firstInputID;
	int _firstOutputID;

	float* _paramValues = NULL;
	Saturator* _saturators = NULL;

	MatrixModule(int n, int firstParamID, int firstInputID, int firstOutputID)
	: _n(n)
	, _firstParamID(firstParamID)
	, _firstInputID(firstInputID)
	, _firstOutputID(firstOutputID)
	{
		assert(_n <= maxN);
		_paramValues = new float[_n * _n] {};
		_saturators = new Saturator[_n * maxChannels];
	}
	virtual ~MatrixModule() {
		delete[] _paramValues;
		delete[] _saturators;
	}

	int channels() override;
	void modulate() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

struct KnobMatrixModule : MatrixModule {
	bool _indicatorKnobs = true;

	KnobMatrixModule(int n, int firstParamID, int firstInputID, int firstOutputID)
	: MatrixModule(n, firstParamID, firstInputID, firstOutputID)
	{}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
};

struct KnobMatrixModuleWidget : MatrixBaseModuleWidget {
	std::vector<IndicatorKnob19*> _knobs;

	void createKnob(math::Vec& position, KnobMatrixModule* module, int id);
	void redrawKnobs();
	void appendContextMenu(Menu* menu) override;
};

struct SwitchMatrixModule : MatrixModule {
	enum Inverting {
		CLICK_INVERTING,
		PARAM_INVERTING,
		NO_INVERTING
	};

	Inverting _inverting = PARAM_INVERTING;
	bool _rowExclusive = false;
	bool _columnExclusive = false;
	std::vector<ParamQuantity*> _switchParamQuantities;

	SwitchMatrixModule(int n, int firstParamID, int firstInputID, int firstOutputID)
	: MatrixModule(n, firstParamID, firstInputID, firstOutputID)
	{}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void setInverting(Inverting inverting);
	void configSwitchParam(int id, const char* label);
	void switchChanged(int id, float value);
	void setRowExclusive(bool e);
	void setColumnExclusive(bool e);
};

struct SwitchMatrixModuleWidget : MatrixBaseModuleWidget {
	template<class W> void createSwitch(math::Vec& position, SwitchMatrixModule* module, int id) {
		auto s = dynamic_cast<W*>(createParam<W>(position, module, id));
		if (module) {
			s->setClickToInvertCallback([module]() { return module->_inverting == SwitchMatrixModule::CLICK_INVERTING; });
			s->setOnChangeCallback([module](int id, float value) { module->switchChanged(id, value); });
		}
		addParam(s);
	}

	void appendContextMenu(Menu* menu) override;
};

} // namespace bogaudio
