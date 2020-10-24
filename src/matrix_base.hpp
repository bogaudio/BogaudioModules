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
	bool _sum = true;

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	void modulate() override;
};

struct MatrixBaseModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

struct MatrixModule : MatrixBaseModule {
	static constexpr int maxN = 16;
	int _ins = 0;
	int _outs = 0;
	int _firstParamID = 0;
	int _firstInputID = 0;
	int _firstOutputID = 0;
	Input** _cvInputs = NULL; // owned elsewhere
	Param** _muteParams = NULL; // owned elsewhere
	bool* _soloByColumns = NULL; // owned elsewhere

	float* _paramValues = NULL;
	bogaudio::dsp::SlewLimiter* _sls = NULL;
	Saturator* _saturators = NULL;
	bool* _inActive = NULL;
	float _invActive = 0.0f;

	MatrixModule() {} // call configMatrixModule().
	MatrixModule(int ins, int outs, int firstParamID, int firstInputID, int firstOutputID) {
		configMatrixModule(ins, outs, firstParamID, firstInputID, firstOutputID);
	}
	virtual ~MatrixModule() {
		delete[] _paramValues;
		delete[] _sls;
		delete[] _saturators;
		delete[] _inActive;
	}

	void configMatrixModule(int ins, int outs, int firstParamID, int firstInputID, int firstOutputID);
	inline void setCVInputs(Input** cvs) { _cvInputs = cvs; }
	inline void setMuteParams(Param** mutes) { _muteParams = mutes; }
	inline void setSoloByColumns(bool* sbc) { _soloByColumns = sbc; }
	void sampleRateChange() override;
	int channels() override;
	void modulate() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

struct MatrixModuleWidget : MatrixBaseModuleWidget {
	void contextMenu(Menu* menu) override;
};

struct KnobMatrixModule : MatrixModule {
	bool _indicatorKnobs = true;

	KnobMatrixModule() {} // call configMatrixModule()
	KnobMatrixModule(int ins, int outs, int firstParamID, int firstInputID, int firstOutputID)
	: MatrixModule(ins, outs, firstParamID, firstInputID, firstOutputID)
	{}

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
};

struct KnobMatrixModuleWidget : MatrixModuleWidget {
	std::vector<IndicatorKnob19*> _knobs;

	void createKnob(math::Vec& position, KnobMatrixModule* module, int id);
	void redrawKnobs();
	void contextMenu(Menu* menu) override;
};

struct SwitchMatrixModule : MatrixModule {
	enum Inverting {
		CLICK_INVERTING,
		PARAM_INVERTING,
		NO_INVERTING
	};

	Inverting _inverting = NO_INVERTING;
	bool _rowExclusive = false;
	bool _columnExclusive = false;
	std::vector<ParamQuantity*> _switchParamQuantities;

	SwitchMatrixModule(int ins, int outs, int firstParamID, int firstInputID, int firstOutputID)
	: MatrixModule(ins, outs, firstParamID, firstInputID, firstOutputID)
	{}

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	void setInverting(Inverting inverting);
	void configSwitchParam(int id, const char* label);
	void switchChanged(int id, float value);
	void setRowExclusive(bool e);
	void setColumnExclusive(bool e);
};

struct SwitchMatrixModuleWidget : MatrixModuleWidget {
	template<class W> void createSwitch(math::Vec& position, SwitchMatrixModule* module, int id) {
		auto s = dynamic_cast<W*>(createParam<W>(position, module, id));
		if (module) {
			s->setClickToInvertCallback([module]() { return module->_inverting == SwitchMatrixModule::CLICK_INVERTING; });
			s->setOnChangeCallback([module](int id, float value) { module->switchChanged(id, value); });
		}
		addParam(s);
	}

	void contextMenu(Menu* menu) override;
};

struct MutesMatrixExpanderModule : BGModule {
	bool _soloByColumns = false;

	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
};

struct MutesMatrixExpanderModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
