
#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct PitchModeListener {
	virtual void pitchModeChanged() = 0;
};

struct LFOBase : Module {
	enum PitchMode {
		UNKNOWN_PITCH_MODE,
		CLASSIC_PITCH_MODE,
		COMPLIANT_PITCH_MODE
	};

	PitchMode _pitchMode = UNKNOWN_PITCH_MODE;
	PitchModeListener* _pitchModeListener = NULL;

	LFOBase(int np, int ni, int no, int nl) {
		config(np, ni, no, nl);
	}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	bool isCompliantPitchMode() { return _pitchMode != CLASSIC_PITCH_MODE; }
	void setPitchMode(PitchMode mode);
	void setPitchModeListener(PitchModeListener* listener) { _pitchModeListener = listener; }
	void setFrequency(bool slow, Param& frequency, Input& pitch, Phasor& phasor);
};

struct PitchModeMenuItem : MenuItem {
	LFOBase* _module;
	bool _compliant;

	PitchModeMenuItem(LFOBase* module, const char* label, bool compliant)
	: _module(module)
	, _compliant(compliant)
	{
		this->text = label;
	}

	void onAction(const event::Action& e) override {
		_module->setPitchMode(_compliant ? LFOBase::COMPLIANT_PITCH_MODE : LFOBase::CLASSIC_PITCH_MODE);
	}

	void step() override {
		bool compliantMode = _module->isCompliantPitchMode();
		bool on = _compliant ? compliantMode : !compliantMode;
		rightText = on ? "✔" : "";
	}
};

struct LFOBaseWidget : ModuleWidget, PitchModeListener {
	LFOBase* _module;
	SVGPanel* _panel;
	const char* _classicSVGName;
	const char* _compliantSVGName;
	SVGKnob* _frequencyKnob = NULL;

	LFOBaseWidget(
		LFOBase* module,
		SVGPanel* panel,
		const char* classicSVGName,
		const char* compliantSVGName
	)
	: _module(module)
	, _panel(panel)
	, _classicSVGName(classicSVGName)
	, _compliantSVGName(compliantSVGName)
	{
		setModule(module);
		setSVG();
		if (_module) {
			_module->setPitchModeListener(this);
		}
	}

	void setSVG() {
		// FIXME.v1
		if (_module && !_module->isCompliantPitchMode()) {
			_panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, _classicSVGName)));
			// if (_frequencyKnob) {
			// 	_frequencyKnob->minValue = -8.0f;
			// 	_frequencyKnob->maxValue = 5.0f;
			// 	_frequencyKnob->dirty = true;
			// }
		}
		else {
			_panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, _compliantSVGName)));
			// if (_frequencyKnob) {
			// 	_frequencyKnob->minValue = -5.0f;
			// 	_frequencyKnob->maxValue = 8.0f;
			// 	_frequencyKnob->dirty = true;
			// }
		}
		_panel->dirty = true;
	}

	void pitchModeChanged() override {
		setSVG();
	}

	void appendContextMenu(Menu* menu) override {
		LFOBase* lfo = dynamic_cast<LFOBase*>(module);
		assert(lfo);
		menu->addChild(new MenuLabel());
		menu->addChild(new PitchModeMenuItem(lfo, "Pitch: 0V = C-3 = 2.04HZ", true));
		menu->addChild(new PitchModeMenuItem(lfo, "Pitch: 0V = C0 = 16.35HZ", false));
	}
};

} // namespace bogaudio
