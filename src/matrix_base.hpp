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
	void appendContextMenu(Menu* menu) override {
		MatrixBaseModule* m = dynamic_cast<MatrixBaseModule*>(module);
		assert(m);
		menu->addChild(new MenuLabel());

		OptionsMenuItem* g = new OptionsMenuItem("Input gain");
		g->addItem(OptionMenuItem("Unity", [m]() { return (int)m->_inputGainDb == 0; }, [m]() { m->_inputGainDb = 0.0f; }));
		g->addItem(OptionMenuItem("-3db", [m]() { return (int)m->_inputGainDb == -3; }, [m]() { m->_inputGainDb = -3.0f; }));
		g->addItem(OptionMenuItem("-6db", [m]() { return (int)m->_inputGainDb == -6; }, [m]() { m->_inputGainDb = -6.0f; }));
		g->addItem(OptionMenuItem("-12db", [m]() { return (int)m->_inputGainDb == -12; }, [m]() { m->_inputGainDb = -12.0f; }));
		OptionsMenuItem::addToMenu(g, menu);

		OptionsMenuItem* c = new OptionsMenuItem("Output clipping");
		c->addItem(OptionMenuItem("Soft/saturated (better for audio)", [m]() { return m->_clippingMode == MatrixBaseModule::SOFT_CLIPPING; }, [m]() { m->_clippingMode = MatrixBaseModule::SOFT_CLIPPING; }));
		c->addItem(OptionMenuItem("Hard/clipped (better for CV)", [m]() { return m->_clippingMode == MatrixBaseModule::HARD_CLIPPING; }, [m]() { m->_clippingMode = MatrixBaseModule::HARD_CLIPPING; }));
		OptionsMenuItem::addToMenu(c, menu);
	}
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
		_paramValues = new float[_n * _n];
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

	void createKnob(math::Vec& position, KnobMatrixModule* module, int id) {
		IndicatorKnob19* knob = dynamic_cast<IndicatorKnob19*>(createParam<IndicatorKnob19>(position, module, id));
		if (module) {
			knob->setDrawColorsCallback([module]() { return module->_indicatorKnobs; });
		}
		addParam(knob);
		_knobs.push_back(knob);
	}

	void redrawKnobs() {
		for (IndicatorKnob19* knob : _knobs) {
			knob->redraw();
		}
	}

	void appendContextMenu(Menu* menu) override {
		KnobMatrixModule* m = dynamic_cast<KnobMatrixModule*>(module);
		assert(m);
		MatrixBaseModuleWidget::appendContextMenu(menu);
		menu->addChild(new OptionMenuItem(
			"Indicator knobs",
			[m]() { return m->_indicatorKnobs; },
			[m, this]() { m->_indicatorKnobs = !m->_indicatorKnobs; this->redrawKnobs(); }
		));
	}
};

struct SwitchMatrixModule : MatrixModule {
	enum Inverting {
		CLICK_INVERTING,
		PARAM_INVERTING,
		NO_INVERTING
	};

	Inverting _inverting = CLICK_INVERTING;
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

	void appendContextMenu(Menu* menu) override {
		SwitchMatrixModule* m = dynamic_cast<SwitchMatrixModule*>(module);
		assert(m);
		MatrixBaseModuleWidget::appendContextMenu(menu);

		OptionsMenuItem* i = new OptionsMenuItem("Inverting");
		i->addItem(OptionMenuItem("On second click", [m]() { return m->_inverting == SwitchMatrixModule::CLICK_INVERTING; }, [m]() { m->setInverting(SwitchMatrixModule::CLICK_INVERTING); }));
		i->addItem(OptionMenuItem("By param entry (right-click)", [m]() { return m->_inverting == SwitchMatrixModule::PARAM_INVERTING; }, [m]() { m->setInverting(SwitchMatrixModule::PARAM_INVERTING); }));
		i->addItem(OptionMenuItem("Disabled", [m]() { return m->_inverting == SwitchMatrixModule::NO_INVERTING; }, [m]() { m->setInverting(SwitchMatrixModule::NO_INVERTING); }));
		OptionsMenuItem::addToMenu(i, menu);

		menu->addChild(new OptionMenuItem("Exclusive by rows", [m]() { return m->_rowExclusive; }, [m]() { m->setRowExclusive(!m->_rowExclusive); }));
		menu->addChild(new OptionMenuItem("Exclusive by columns", [m]() { return m->_columnExclusive; }, [m]() { m->setColumnExclusive(!m->_columnExclusive); }));
	}
};

} // namespace bogaudio
