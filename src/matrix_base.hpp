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
		c->addItem(OptionMenuItem("Soft", [m]() { return m->_clippingMode == MatrixBaseModule::SOFT_CLIPPING; }, [m]() { m->_clippingMode = MatrixBaseModule::SOFT_CLIPPING; }));
		c->addItem(OptionMenuItem("Hard", [m]() { return m->_clippingMode == MatrixBaseModule::HARD_CLIPPING; }, [m]() { m->_clippingMode = MatrixBaseModule::HARD_CLIPPING; }));
		OptionsMenuItem::addToMenu(c, menu);
	}
};

struct MatrixModule : MatrixBaseModule {
	int _n;
	int _firstParamID;
	int _firstInputID;
	int _firstOutputID;
	Saturator* _saturators = NULL;

	MatrixModule(int n, int firstParamID, int firstInputID, int firstOutputID)
	: _n(n)
	, _firstParamID(firstParamID)
	, _firstInputID(firstInputID)
	, _firstOutputID(firstOutputID)
	{
		_saturators = new Saturator[_n * maxChannels];
	}
	virtual ~MatrixModule() {
		delete[] _saturators;
	}

	int channels() override;
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

} // namespace bogaudio
