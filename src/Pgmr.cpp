
#include "Pgmr.hpp"

#define SELECT_TRIGGERS "SELECT_TRIGGERS"
#define SAVE_LAST_TRIGGERED_TO_PATCH "save_last_triggered_to_patch"
#define LAST_TRIGGERED_STEP "last_triggered_step"
#define LAST_TRIGGERED_ELEMENTS_COUNT "last_triggered_elements_count"

void Pgmr::reset() {
	std::lock_guard<SpinLock> lock(_elementsLock);

	for (int c = 0; c < maxChannels; ++c) {
		_lastSteps[c] = -1;
		_allPulseGens[c].process(1000.0f);
	}
	for (auto* element : _elements) {
		element->reset();
	}
}

void Pgmr::sampleRateChange() {
	_sampleTime = APP->engine->getSampleTime();
}

json_t* Pgmr::saveToJson(json_t* root) {
	root = OutputRangeAddressableSequenceModule::saveToJson(root);
	json_object_set_new(root, SELECT_TRIGGERS, json_boolean(_selectTriggers));
	json_object_set_new(root, SAVE_LAST_TRIGGERED_TO_PATCH, json_boolean(_saveLastTriggeredToPatch));
	if (_saveLastTriggeredToPatch) {
		json_t* a = json_array();
		for (int c = 0; c < maxChannels; ++c) {
			json_array_append_new(a, json_integer(_step[c]));
		}
		json_object_set_new(root, LAST_TRIGGERED_STEP, a);
		json_object_set_new(root, LAST_TRIGGERED_ELEMENTS_COUNT, json_integer(_elements.size()));
	}
	return root;
}

void Pgmr::loadFromJson(json_t* root) {
	OutputRangeAddressableSequenceModule::loadFromJson(root);

	json_t* st = json_object_get(root, SELECT_TRIGGERS);
	if (st) {
		_selectTriggers = json_is_true(st);
	}

	json_t* sl = json_object_get(root, SAVE_LAST_TRIGGERED_TO_PATCH);
	if (sl) {
		_saveLastTriggeredToPatch = json_is_true(sl);
		if (_saveLastTriggeredToPatch) {
			json_t* a = json_object_get(root, LAST_TRIGGERED_STEP);
			json_t* sz = json_object_get(root, LAST_TRIGGERED_ELEMENTS_COUNT);
			if (a && json_array_size(a) == maxChannels && sz) {
				_restoreLastTriggeredExpectedElementsN = json_integer_value(sz);
				std::vector<int> restoreSteps(maxChannels);
				for (int c = 0; c < maxChannels; ++c) {
					json_t* s = json_array_get(a, c);
					if (s) {
						restoreSteps[c] = json_integer_value(s);
					}
				}
				_restoreLastTriggered = new std::function<void()>([this, restoreSteps]() {
					for (int c = 0; c < maxChannels; ++c) {
						setStep(c, restoreSteps[c], _elements.size());
					}
				});
			}
		}
	}
}

void Pgmr::modulate() {
	_selectOnClock = params[SELECT_ON_CLOCK_PARAM].getValue() > 0.5f;
}

void Pgmr::processAlways(const ProcessArgs& args) {
	if (expanderConnected()) {
		PgmrExpanderMessage* te = toExpander();
		te->baseID = _id;
		te->position = 1;
		te->rangeOffset = _rangeOffset;
		te->rangeScale = _rangeScale;
	}
}

void Pgmr::processChannel(const ProcessArgs& args, int c) {
	std::lock_guard<SpinLock> lock(_elementsLock);
	std::vector<PgmrStep*>& steps = _elements;
	int stepsN = steps.size();

	if (c == 0) {
		for (int i = 0; i < stepsN; ++i) {
			steps[i]->lightSum = 0.0f;
		}
	}

	int step = nextStep(
		c,
		NULL,
		inputs[CLOCK_INPUT],
		NULL,
		params[DIRECTION_PARAM],
		NULL,
		inputs[SELECT_INPUT],
		stepsN
	);
	for (int i = 0; i < stepsN; ++i) {
		if (steps[i]->triggers[c].process(steps[i]->selectParam.getValue() + steps[i]->selectInput.getPolyVoltage(c))) {
			step = setStep(c, i, stepsN);
		}
	}

	{
		float out = steps[step]->aParam.getValue();
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[A_OUTPUT].setChannels(_channels);
		outputs[A_OUTPUT].setVoltage(out, c);
	}
	{
		float out = steps[step]->bParam.getValue();
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[B_OUTPUT].setChannels(_channels);
		outputs[B_OUTPUT].setVoltage(out, c);
	}
	{
		float out = steps[step]->cParam.getValue();
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[C_OUTPUT].setChannels(_channels);
		outputs[C_OUTPUT].setVoltage(out, c);
	}
	{
		float out = steps[step]->dParam.getValue();
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[D_OUTPUT].setChannels(_channels);
		outputs[D_OUTPUT].setVoltage(out, c);
	}

	if (step != _lastSteps[c]) {
		_lastSteps[c] = step;
		_allPulseGens[c].trigger(0.001f);
		steps[step]->pulseGens[c].trigger(0.001f);
	}
	outputs[SELECT_ALL_OUTPUT].setChannels(_channels);
	outputs[SELECT_ALL_OUTPUT].setVoltage(_allPulseGens[c].process(_sampleTime) * 5.0f, c);

	for (int i = 0; i < stepsN; ++i) {
		steps[i]->selectedOutput.setChannels(_channels);
		steps[i]->selectedOutput.setVoltage((steps[i]->pulseGens[c].process(_sampleTime) || (!_selectTriggers && step == i)) * 5.0f, c);

		steps[i]->lightSum += step == i;
	}

	if (c == _channels - 1) {
		for (int i = 0; i < stepsN; ++i) {
			steps[i]->selectedLight.value = steps[i]->lightSum * _inverseChannels;
		}
	}
}

void Pgmr::elementsChanged() {
	if (_restoreLastTriggered && (int)_elements.size() == _restoreLastTriggeredExpectedElementsN) {
		(*_restoreLastTriggered)();
		delete _restoreLastTriggered;
		_restoreLastTriggered = NULL;
	}
}


struct PgmrWidget : AddressableSequenceBaseModuleWidget {
	static constexpr int hp = 15;

	PgmrWidget(Pgmr* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Pgmr");
		createScrews();

		// generated by svg_widgets.rb
		auto directionParamPosition = Vec(28.0, 263.7);
		auto selectOnClockParamPosition = Vec(28.0, 277.2);
		auto cva1ParamPosition = Vec(54.5, 40.5);
		auto cvb1ParamPosition = Vec(54.5, 94.5);
		auto cvc1ParamPosition = Vec(54.5, 148.5);
		auto cvd1ParamPosition = Vec(54.5, 202.5);
		auto select1ParamPosition = Vec(58.5, 267.0);
		auto cva2ParamPosition = Vec(99.5, 40.5);
		auto cvb2ParamPosition = Vec(99.5, 94.5);
		auto cvc2ParamPosition = Vec(99.5, 148.5);
		auto cvd2ParamPosition = Vec(99.5, 202.5);
		auto select2ParamPosition = Vec(103.5, 267.0);
		auto cva3ParamPosition = Vec(144.5, 40.5);
		auto cvb3ParamPosition = Vec(144.5, 94.5);
		auto cvc3ParamPosition = Vec(144.5, 148.5);
		auto cvd3ParamPosition = Vec(144.5, 202.5);
		auto select3ParamPosition = Vec(148.5, 267.0);
		auto cva4ParamPosition = Vec(189.5, 40.5);
		auto cvb4ParamPosition = Vec(189.5, 94.5);
		auto cvc4ParamPosition = Vec(189.5, 148.5);
		auto cvd4ParamPosition = Vec(189.5, 202.5);
		auto select4ParamPosition = Vec(193.5, 267.0);

		auto clockInputPosition = Vec(10.5, 226.0);
		auto selectInputPosition = Vec(10.5, 290.0);
		auto select1InputPosition = Vec(55.5, 290.0);
		auto select2InputPosition = Vec(100.5, 290.0);
		auto select3InputPosition = Vec(145.5, 290.0);
		auto select4InputPosition = Vec(190.5, 290.0);

		auto aOutputPosition = Vec(10.5, 59.0);
		auto bOutputPosition = Vec(10.5, 94.0);
		auto cOutputPosition = Vec(10.5, 129.0);
		auto dOutputPosition = Vec(10.5, 164.0);
		auto selectAllOutputPosition = Vec(10.5, 330.0);
		auto select1OutputPosition = Vec(55.5, 330.0);
		auto select2OutputPosition = Vec(100.5, 330.0);
		auto select3OutputPosition = Vec(145.5, 330.0);
		auto select4OutputPosition = Vec(190.5, 330.0);

		auto select1LightPosition = Vec(64.3, 255.0);
		auto select2LightPosition = Vec(109.3, 255.0);
		auto select3LightPosition = Vec(154.3, 255.0);
		auto select4LightPosition = Vec(199.3, 255.0);
		// end generated by svg_widgets.rb

		addParam(createParam<IndicatorButtonGreen9>(directionParamPosition, module, Pgmr::DIRECTION_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(selectOnClockParamPosition, module, Pgmr::SELECT_ON_CLOCK_PARAM));
		addParam(createParam<Knob26>(cva1ParamPosition, module, Pgmr::CVA1_PARAM));
		addParam(createParam<Knob26>(cvb1ParamPosition, module, Pgmr::CVB1_PARAM));
		addParam(createParam<Knob26>(cvc1ParamPosition, module, Pgmr::CVC1_PARAM));
		addParam(createParam<Knob26>(cvd1ParamPosition, module, Pgmr::CVD1_PARAM));
		addParam(createParam<Button18>(select1ParamPosition, module, Pgmr::SELECT1_PARAM));
		addParam(createParam<Knob26>(cva2ParamPosition, module, Pgmr::CVA2_PARAM));
		addParam(createParam<Knob26>(cvb2ParamPosition, module, Pgmr::CVB2_PARAM));
		addParam(createParam<Knob26>(cvc2ParamPosition, module, Pgmr::CVC2_PARAM));
		addParam(createParam<Knob26>(cvd2ParamPosition, module, Pgmr::CVD2_PARAM));
		addParam(createParam<Button18>(select2ParamPosition, module, Pgmr::SELECT2_PARAM));
		addParam(createParam<Knob26>(cva3ParamPosition, module, Pgmr::CVA3_PARAM));
		addParam(createParam<Knob26>(cvb3ParamPosition, module, Pgmr::CVB3_PARAM));
		addParam(createParam<Knob26>(cvc3ParamPosition, module, Pgmr::CVC3_PARAM));
		addParam(createParam<Knob26>(cvd3ParamPosition, module, Pgmr::CVD3_PARAM));
		addParam(createParam<Button18>(select3ParamPosition, module, Pgmr::SELECT3_PARAM));
		addParam(createParam<Knob26>(cva4ParamPosition, module, Pgmr::CVA4_PARAM));
		addParam(createParam<Knob26>(cvb4ParamPosition, module, Pgmr::CVB4_PARAM));
		addParam(createParam<Knob26>(cvc4ParamPosition, module, Pgmr::CVC4_PARAM));
		addParam(createParam<Knob26>(cvd4ParamPosition, module, Pgmr::CVD4_PARAM));
		addParam(createParam<Button18>(select4ParamPosition, module, Pgmr::SELECT4_PARAM));

		addInput(createInput<Port24>(clockInputPosition, module, Pgmr::CLOCK_INPUT));
		addInput(createInput<Port24>(selectInputPosition, module, Pgmr::SELECT_INPUT));
		addInput(createInput<Port24>(select1InputPosition, module, Pgmr::SELECT1_INPUT));
		addInput(createInput<Port24>(select2InputPosition, module, Pgmr::SELECT2_INPUT));
		addInput(createInput<Port24>(select3InputPosition, module, Pgmr::SELECT3_INPUT));
		addInput(createInput<Port24>(select4InputPosition, module, Pgmr::SELECT4_INPUT));

		addOutput(createOutput<Port24>(aOutputPosition, module, Pgmr::A_OUTPUT));
		addOutput(createOutput<Port24>(bOutputPosition, module, Pgmr::B_OUTPUT));
		addOutput(createOutput<Port24>(cOutputPosition, module, Pgmr::C_OUTPUT));
		addOutput(createOutput<Port24>(dOutputPosition, module, Pgmr::D_OUTPUT));
		addOutput(createOutput<Port24>(selectAllOutputPosition, module, Pgmr::SELECT_ALL_OUTPUT));
		addOutput(createOutput<Port24>(select1OutputPosition, module, Pgmr::SELECT1_OUTPUT));
		addOutput(createOutput<Port24>(select2OutputPosition, module, Pgmr::SELECT2_OUTPUT));
		addOutput(createOutput<Port24>(select3OutputPosition, module, Pgmr::SELECT3_OUTPUT));
		addOutput(createOutput<Port24>(select4OutputPosition, module, Pgmr::SELECT4_OUTPUT));

		addChild(createLight<BGSmallLight<GreenLight>>(select1LightPosition, module, Pgmr::SELECT1_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(select2LightPosition, module, Pgmr::SELECT2_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(select3LightPosition, module, Pgmr::SELECT3_LIGHT));
		addChild(createLight<BGSmallLight<GreenLight>>(select4LightPosition, module, Pgmr::SELECT4_LIGHT));
	}

	void contextMenu(Menu* menu) override {
		AddressableSequenceBaseModuleWidget::contextMenu(menu);

		auto m = dynamic_cast<Pgmr*>(module);
		assert(m);
		OptionsMenuItem* so = new OptionsMenuItem("Output on selected step");
		so->addItem(OptionMenuItem("Gate", [m]() { return !m->_selectTriggers; }, [m]() { m->_selectTriggers = false; }));
		so->addItem(OptionMenuItem("Trigger", [m]() { return m->_selectTriggers; }, [m]() { m->_selectTriggers = true; }));
		OptionsMenuItem::addToMenu(so, menu);

		menu->addChild(new BoolOptionMenuItem("Save last selected step to patch", [m]() { return &m->_saveLastTriggeredToPatch; }));

		OutputRangeOptionMenuItem::addOutputRangeOptionsToMenu(module, menu);
	}
};

Model* modelPgmr = createModel<Pgmr, PgmrWidget>("Bogaudio-Pgmr", "PGMR", "4-step programmer and sequencer", "Sequencer", "Polyphonic");
