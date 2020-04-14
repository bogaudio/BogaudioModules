
#include "Pgmr.hpp"

#define SELECT_TRIGGERS "SELECT_TRIGGERS"

void PgmrStep::reset() {
	for (int c = 0; c < BGModule::maxChannels; ++c) {
		triggers[c].reset();
		pulseGens[c].process(1000.0f);
	}
}


PgmrRegistry::Base::Base(Pgmr& b) : module(b) {
	std::copy(b._localSteps, b._localSteps + 4, std::back_inserter(steps));
}

int PgmrRegistry::registerBase(Pgmr& b) {
	std::lock_guard<std::mutex> lock(_lock);

	int id = _nextID;
	++_nextID;
	auto p = _bases.emplace(id, Base(b));
	b.setSteps(p.first->second.steps);
	return id;
}

void PgmrRegistry::deregisterBase(int id) {
	std::lock_guard<std::mutex> lock(_lock);
	_bases.erase(id);
}

void PgmrRegistry::registerExpander(int baseID, int position, PgmrX& x) {
	std::lock_guard<std::mutex> lock(_lock);

	assert(position > 0);
	auto base = _bases.find(baseID);
	if (base != _bases.end()) {
		int i = 4 * position;
		if (i < (int)base->second.steps.size()) {
			assert(!base->second.steps[i]);
			std::copy(x._localSteps, x._localSteps + 4, base->second.steps.begin() + i);
		}
		else {
			base->second.steps.resize(i + 4, NULL);
			std::copy(x._localSteps, x._localSteps + 4, base->second.steps.begin() + i);
		}
		for (auto i = base->second.steps.begin(), n = base->second.steps.end(); i != n; ++i) {
			if (!*i) {
				return;
			}
		}
		base->second.module.setSteps(base->second.steps);
	}
}

void PgmrRegistry::deregisterExpander(int baseID, int position) {
	std::lock_guard<std::mutex> lock(_lock);

	auto base = _bases.find(baseID);
	if (base != _bases.end()) {
		int n = 4 * position;
		if (n < (int)base->second.steps.size()) {
			int i = 0;
			for (; i < n; ++i) {
				if (!base->second.steps[i]) {
					break;
				}
			}
			base->second.steps.resize(i);
			base->second.module.setSteps(base->second.steps);
		}
	}
}

PgmrRegistry& PgmrRegistry::registry() {
	static PgmrRegistry instance;
	return instance;
}


void Pgmr::reset() {
	std::lock_guard<SpinLock> lock(_stepsLock);

	for (int c = 0; c < maxChannels; ++c) {
		_lastSteps[c] = -1;
		_allPulseGens[c].process(1000.0f);
	}
	for (int i = 0, n = _steps.size(); i < n; ++i) {
		_steps[i]->reset();
	}
}

void Pgmr::sampleRateChange() {
	_sampleTime = APP->engine->getSampleTime();
}

json_t* Pgmr::dataToJson() {
	json_t* root = OutputRangeAddressableSequenceModule::dataToJson();
	json_object_set_new(root, SELECT_TRIGGERS, json_boolean(_selectTriggers));
	return root;
}

void Pgmr::dataFromJson(json_t* root) {
	OutputRangeAddressableSequenceModule::dataFromJson(root);
	json_t* st = json_object_get(root, SELECT_TRIGGERS);
	if (st) {
		_selectTriggers = json_is_true(st);
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
	std::lock_guard<SpinLock> lock(_stepsLock);
	int steps = _steps.size();
	if (c == 0) {
		for (int i = 0, n = _steps.size(); i < n; ++i) {
			_steps[i]->lightSum = 0.0f;
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
		steps
	);
	for (int i = 0; i < steps; ++i) {
		if (_steps[i]->triggers[c].process(_steps[i]->selectParam.getValue() + _steps[i]->selectInput.getPolyVoltage(c))) {
			step = setStep(c, i, steps);
		}
	}

	{
		float out = _steps[step]->aParam.getValue();
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[A_OUTPUT].setChannels(_channels);
		outputs[A_OUTPUT].setVoltage(out, c);
	}
	{
		float out = _steps[step]->bParam.getValue();
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[B_OUTPUT].setChannels(_channels);
		outputs[B_OUTPUT].setVoltage(out, c);
	}
	{
		float out = _steps[step]->cParam.getValue();
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[C_OUTPUT].setChannels(_channels);
		outputs[C_OUTPUT].setVoltage(out, c);
	}
	{
		float out = _steps[step]->dParam.getValue();
		out += _rangeOffset;
		out *= _rangeScale;
		outputs[D_OUTPUT].setChannels(_channels);
		outputs[D_OUTPUT].setVoltage(out, c);
	}

	if (step != _lastSteps[c]) {
		_lastSteps[c] = step;
		_allPulseGens[c].trigger(0.001f);
		_steps[step]->pulseGens[c].trigger(0.001f);
	}
	outputs[SELECT_ALL_OUTPUT].setChannels(_channels);
	outputs[SELECT_ALL_OUTPUT].setVoltage(_allPulseGens[c].process(_sampleTime) * 5.0f, c);

	for (int i = 0; i < steps; ++i) {
		_steps[i]->selectedOutput.setChannels(_channels);
		_steps[i]->selectedOutput.setVoltage((_steps[i]->pulseGens[c].process(_sampleTime) || (!_selectTriggers && step == i)) * 5.0f, c);

		_steps[i]->lightSum += step == i;
	}

	if (c == _channels - 1) {
		for (int i = 0, n = _steps.size(); i < n; ++i) {
			_steps[i]->selectedLight.value = _steps[i]->lightSum / (float)_channels;
		}
	}
}

void Pgmr::setSteps(std::vector<PgmrStep*>& steps) {
	std::lock_guard<SpinLock> lock(_stepsLock);
	_steps = steps;
}


struct PgmrWidget : AddressableSequenceBaseModuleWidget {
	static constexpr int hp = 15;

	PgmrWidget(Pgmr* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Pgmr.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

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
		auto dOutputPosition = Vec(10.5, 163.0);
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

		addChild(createLight<SmallLight<GreenLight>>(select1LightPosition, module, Pgmr::SELECT1_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(select2LightPosition, module, Pgmr::SELECT2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(select3LightPosition, module, Pgmr::SELECT3_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(select4LightPosition, module, Pgmr::SELECT4_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		AddressableSequenceBaseModuleWidget::appendContextMenu(menu);

		auto m = dynamic_cast<Pgmr*>(module);
		assert(m);
		OptionsMenuItem* so = new OptionsMenuItem("Output on selected step");
		so->addItem(OptionMenuItem("Gate", [m]() { return !m->_selectTriggers; }, [m]() { m->_selectTriggers = false; }));
		so->addItem(OptionMenuItem("Trigger", [m]() { return m->_selectTriggers; }, [m]() { m->_selectTriggers = true; }));
		OptionsMenuItem::addToMenu(so, menu);

		OutputRangeOptionMenuItem::addOutputRangeOptionsToMenu(module, menu);
	}
};

Model* modelPgmr = createModel<Pgmr, PgmrWidget>("Bogaudio-Pgmr", "PGMR", "4-step programmer and sequencer", "Sequencer", "Polyphonic");


void PgmrX::processAlways(const ProcessArgs& args) {
	int position = 0;
	int baseID = 0;
	if (baseConnected()) {
		PgmrExpanderMessage* bm = fromBase();
		baseID = bm->baseID;
		position = bm->position;
		_rangeOffset = bm->rangeOffset;
		_rangeScale = bm->rangeScale;
	}

	if (_registered && (position <= 0 || position != _position)) {
		PgmrRegistry::registry().deregisterExpander(_baseID, _position);
		_registered = false;
		_baseID = 0;
		_position = 0;
	}
	else if (!_registered && position > 0) {
		_registered = true;
		_baseID = baseID;
		_position = position;
		PgmrRegistry::registry().registerExpander(_baseID, _position, *this);
	}

	if (_position < 1) {
		for (int i = 0; i < 4; ++i) {
			_localSteps[i]->selectedLight.value = 0.0f;
		}
	}
	if (expanderConnected()) {
		PgmrExpanderMessage* te = toExpander();
		te->baseID = _baseID;
		te->position = _position > 0 ? _position + 1 : 0;
		te->rangeOffset = _rangeOffset;
		te->rangeScale = _rangeScale;
	}
}

struct PgmrXWidget : ModuleWidget {
	static constexpr int hp = 12;

	PgmrXWidget(PgmrX* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PgmrX.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto cva1ParamPosition = Vec(9.5, 40.5);
		auto cvb1ParamPosition = Vec(9.5, 94.5);
		auto cvc1ParamPosition = Vec(9.5, 148.5);
		auto cvd1ParamPosition = Vec(9.5, 202.5);
		auto select1ParamPosition = Vec(13.5, 267.0);
		auto cva2ParamPosition = Vec(54.5, 40.5);
		auto cvb2ParamPosition = Vec(54.5, 94.5);
		auto cvc2ParamPosition = Vec(54.5, 148.5);
		auto cvd2ParamPosition = Vec(54.5, 202.5);
		auto select2ParamPosition = Vec(58.5, 267.0);
		auto cva3ParamPosition = Vec(99.5, 40.5);
		auto cvb3ParamPosition = Vec(99.5, 94.5);
		auto cvc3ParamPosition = Vec(99.5, 148.5);
		auto cvd3ParamPosition = Vec(99.5, 202.5);
		auto select3ParamPosition = Vec(103.5, 267.0);
		auto cva4ParamPosition = Vec(144.5, 40.5);
		auto cvb4ParamPosition = Vec(144.5, 94.5);
		auto cvc4ParamPosition = Vec(144.5, 148.5);
		auto cvd4ParamPosition = Vec(144.5, 202.5);
		auto select4ParamPosition = Vec(148.5, 267.0);

		auto select1InputPosition = Vec(10.5, 290.0);
		auto select2InputPosition = Vec(55.5, 290.0);
		auto select3InputPosition = Vec(100.5, 290.0);
		auto select4InputPosition = Vec(145.5, 290.0);

		auto select1OutputPosition = Vec(10.5, 330.0);
		auto select2OutputPosition = Vec(55.5, 330.0);
		auto select3OutputPosition = Vec(100.5, 330.0);
		auto select4OutputPosition = Vec(145.5, 330.0);

		auto select1LightPosition = Vec(19.3, 255.0);
		auto select2LightPosition = Vec(64.3, 255.0);
		auto select3LightPosition = Vec(109.3, 255.0);
		auto select4LightPosition = Vec(154.3, 255.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(cva1ParamPosition, module, PgmrX::CVA1_PARAM));
		addParam(createParam<Knob26>(cvb1ParamPosition, module, PgmrX::CVB1_PARAM));
		addParam(createParam<Knob26>(cvc1ParamPosition, module, PgmrX::CVC1_PARAM));
		addParam(createParam<Knob26>(cvd1ParamPosition, module, PgmrX::CVD1_PARAM));
		addParam(createParam<Button18>(select1ParamPosition, module, PgmrX::SELECT1_PARAM));
		addParam(createParam<Knob26>(cva2ParamPosition, module, PgmrX::CVA2_PARAM));
		addParam(createParam<Knob26>(cvb2ParamPosition, module, PgmrX::CVB2_PARAM));
		addParam(createParam<Knob26>(cvc2ParamPosition, module, PgmrX::CVC2_PARAM));
		addParam(createParam<Knob26>(cvd2ParamPosition, module, PgmrX::CVD2_PARAM));
		addParam(createParam<Button18>(select2ParamPosition, module, PgmrX::SELECT2_PARAM));
		addParam(createParam<Knob26>(cva3ParamPosition, module, PgmrX::CVA3_PARAM));
		addParam(createParam<Knob26>(cvb3ParamPosition, module, PgmrX::CVB3_PARAM));
		addParam(createParam<Knob26>(cvc3ParamPosition, module, PgmrX::CVC3_PARAM));
		addParam(createParam<Knob26>(cvd3ParamPosition, module, PgmrX::CVD3_PARAM));
		addParam(createParam<Button18>(select3ParamPosition, module, PgmrX::SELECT3_PARAM));
		addParam(createParam<Knob26>(cva4ParamPosition, module, PgmrX::CVA4_PARAM));
		addParam(createParam<Knob26>(cvb4ParamPosition, module, PgmrX::CVB4_PARAM));
		addParam(createParam<Knob26>(cvc4ParamPosition, module, PgmrX::CVC4_PARAM));
		addParam(createParam<Knob26>(cvd4ParamPosition, module, PgmrX::CVD4_PARAM));
		addParam(createParam<Button18>(select4ParamPosition, module, PgmrX::SELECT4_PARAM));

		addInput(createInput<Port24>(select1InputPosition, module, PgmrX::SELECT1_INPUT));
		addInput(createInput<Port24>(select2InputPosition, module, PgmrX::SELECT2_INPUT));
		addInput(createInput<Port24>(select3InputPosition, module, PgmrX::SELECT3_INPUT));
		addInput(createInput<Port24>(select4InputPosition, module, PgmrX::SELECT4_INPUT));

		addOutput(createOutput<Port24>(select1OutputPosition, module, PgmrX::SELECT1_OUTPUT));
		addOutput(createOutput<Port24>(select2OutputPosition, module, PgmrX::SELECT2_OUTPUT));
		addOutput(createOutput<Port24>(select3OutputPosition, module, PgmrX::SELECT3_OUTPUT));
		addOutput(createOutput<Port24>(select4OutputPosition, module, PgmrX::SELECT4_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(select1LightPosition, module, PgmrX::SELECT1_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(select2LightPosition, module, PgmrX::SELECT2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(select3LightPosition, module, PgmrX::SELECT3_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(select4LightPosition, module, PgmrX::SELECT4_LIGHT));
	}
};

Model* modelPgmrX = createModel<PgmrX, PgmrXWidget>("Bogaudio-PgmrX", "PGMRX", "4-step chainable expander for PGMR", "Sequencer", "Expander", "Polyphonic");
