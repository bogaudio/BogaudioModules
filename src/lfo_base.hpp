
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
	bool _slowMode = false;
	PitchModeListener* _pitchModeListener = NULL;

	LFOBase(int np, int ni, int no, int nl) {
		config(np, ni, no, nl);
	}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	bool isCompliantPitchMode() { return _pitchMode != CLASSIC_PITCH_MODE; }
	void setPitchMode(PitchMode mode);
	void setPitchModeListener(PitchModeListener* listener) { _pitchModeListener = listener; }
	float getPitchOffset();
	void setFrequency(Param& frequency, Input& pitch, Phasor& phasor);
};

struct LFOFrequencyParamQuantity : FrequencyParamQuantity {
	float offset() override {
		LFOBase* lfo = static_cast<LFOBase*>(module);
		return lfo->getPitchOffset();
	}
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
	SvgPanel* _panel = NULL;
	const char* _classicSvgName;
	const char* _compliantSvgName;
	BGKnob* _frequencyKnob = NULL;

	LFOBaseWidget(
		LFOBase* module,
		Vec size,
		const char* classicSvgName,
		const char* compliantSvgName
	)
	: _module(module)
	, _classicSvgName(classicSvgName)
	, _compliantSvgName(compliantSvgName)
	{
		setModule(module);
		box.size = size;
		setSvg();
		if (_module) {
			_module->setPitchModeListener(this);
		}
	}

	void setSvg() {
		if (_panel) {
			removeChild(_panel);
			delete _panel;
		}
		_panel = new SvgPanel();
		_panel->box.size = box.size;
		addChildBottom(_panel);

		if (_module && !_module->isCompliantPitchMode()) {
			_panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, _classicSvgName)));
			if (_frequencyKnob && _frequencyKnob->paramQuantity) {
				_frequencyKnob->paramQuantity->minValue = -8.0f;
				_frequencyKnob->paramQuantity->maxValue = 5.0f;
				_frequencyKnob->redraw();
			}
		}
		else {
			_panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, _compliantSvgName)));
			if (_frequencyKnob && _frequencyKnob->paramQuantity) {
				_frequencyKnob->paramQuantity->minValue = -5.0f;
				_frequencyKnob->paramQuantity->maxValue = 8.0f;
				_frequencyKnob->redraw();
			}
		}
	}

	void pitchModeChanged() override {
		setSvg();
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
