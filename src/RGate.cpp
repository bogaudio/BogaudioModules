
#include "RGate.hpp"

#define RESET_MODE "reset_mode"
#define INITIAL_CLOCK_SECONDS "initial_clock_seconds"
#define POLY_INPUT "poly_input"

void RGate::Engine::reset(bool triggers, bool hard, float initialClock) {
	if (triggers) {
		clockTrigger.reset();
		runTrigger.reset();
		initialGatePulseGen.process(10.0f);
	}
	if (hard) {
		secondsSinceLastClock = -1.0f;
		clockSeconds = initialClock;
		dividedSeconds = -1.0f;
		multipliedSeconds = -1.0f;
		gateSeconds = 0.0f;
	}
	dividerCount = 0;
	dividedProgressSeconds = 0.0f;
}

void RGate::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset(true, true, _initialClockPeriod);
	}
}

void RGate::sampleRateChange() {
	_sampleTime = APP->engine->getSampleTime();
}

json_t* RGate::toJson(json_t* root) {
	root = OutputRangeModule<BGModule>::toJson(root);
	json_object_set_new(root, RESET_MODE, json_integer(_resetMode));
	json_object_set_new(root, INITIAL_CLOCK_SECONDS, json_real(_initialClockPeriod));
	json_object_set_new(root, POLY_INPUT, json_integer(_polyInputID));
	return root;
}

void RGate::fromJson(json_t* root) {
	OutputRangeModule<BGModule>::fromJson(root);

	json_t* rm = json_object_get(root, RESET_MODE);
	if (rm) {
		ResetMode m = (ResetMode)json_integer_value(rm);
		switch (m) {
			case HARD_RESETMODE:
			case SOFT_RESETMODE: {
				_resetMode = m;
				break;
			}
			default: {
				_resetMode = defaultResetMode;
			}
		}
	}

	json_t* ics = json_object_get(root, INITIAL_CLOCK_SECONDS);
	if (ics) {
		_initialClockPeriod = std::max(0.0f, (float)json_real_value(ics));
	}

	json_t* p = json_object_get(root, POLY_INPUT);
	if (p) {
		_polyInputID = json_integer_value(p);
	}
}

bool RGate::active() {
	return outputs[GATE_OUTPUT].isConnected();
}

int RGate::channels() {
	return inputs[_polyInputID].getChannels();
}

void RGate::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset(true, true, _initialClockPeriod);
}

void RGate::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void RGate::modulateChannel(int c) {
	Engine& e = *_engines[c];

	e.gatePercentage = clamp(params[LENGTH_PARAM].getValue(), 0.0f, 1.0f);
	if (inputs[LENGTH_INPUT].isConnected()) {
		e.gatePercentage *= clamp(inputs[LENGTH_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}

	float division = clamp(params[CLOCK_DIVIDE_PARAM].getValue(), 0.0f, 1.0f);
	if (inputs[CLOCK_DIVIDE_INPUT].isConnected()) {
		division *= clamp(inputs[CLOCK_DIVIDE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	division *= division;
	division *= 63.0f;
	division += 1.0f;
	e.division = clamp((int)roundf(division), 1, 64);

	float multiplication = clamp(params[CLOCK_MULTIPLY_PARAM].getValue(), 0.0f, 1.0f);
	if (inputs[CLOCK_MULTIPLE_INPUT].isConnected()) {
		multiplication *= clamp(inputs[CLOCK_MULTIPLE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	multiplication *= multiplication;
	multiplication *= 63.0f;
	multiplication += 1.0f;
	e.multiplication = clamp((int)roundf(multiplication), 1, 64);
}

void RGate::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	if (e.runTrigger.process(inputs[RESET_INPUT].getPolyVoltage(c))) {
		switch (_resetMode) {
			case HARD_RESETMODE: {
				e.reset(false, true, _initialClockPeriod);
				break;
			}
			case SOFT_RESETMODE: {
				e.reset(false, false, _initialClockPeriod);
				break;
			}
		}
	}

	float out = -1.0f;
	bool clock = false;
	if (inputs[CLOCK_INPUT].isConnected()) {
		clock = e.clockTrigger.process(inputs[CLOCK_INPUT].getPolyVoltage(c));
		if (clock) {
			if (e.secondsSinceLastClock > 0.0f) {
				e.clockSeconds = e.secondsSinceLastClock;
			}
			e.secondsSinceLastClock = 0.0f;
		}

		if (e.secondsSinceLastClock >= 0.0f) {
			e.secondsSinceLastClock += _sampleTime;
			e.dividedSeconds = e.clockSeconds * (float)e.division;
			e.multipliedSeconds = e.dividedSeconds / (float)e.multiplication;
			e.gateSeconds = std::max(0.001f, e.multipliedSeconds * e.gatePercentage);
			if (clock) {
				if (e.dividerCount < 1) {
					e.dividedProgressSeconds = 0.0f;
				}
				else {
					e.dividedProgressSeconds += _sampleTime;
				}
				++e.dividerCount;
				if (e.dividerCount >= e.division) {
					e.dividerCount = 0;
				}
			}
			else {
				e.dividedProgressSeconds += _sampleTime;
			}

			if (e.dividedProgressSeconds < e.dividedSeconds) {
				float multipliedProgressSeconds = e.dividedProgressSeconds / e.multipliedSeconds;
				multipliedProgressSeconds -= (float)(int)multipliedProgressSeconds;
				multipliedProgressSeconds *= e.multipliedSeconds;
				out += 2.0f * (float)(multipliedProgressSeconds <= e.gateSeconds);
			}
		}
	}

	out += _rangeOffset;
	out *= _rangeScale;
	outputs[GATE_OUTPUT].setChannels(_channels);
	outputs[GATE_OUTPUT].setVoltage(out, c);
}

struct IPQuantity : Quantity {
	RGate* _module;

	IPQuantity(RGate* m) : _module(m) {}

	void setValue(float value) override {
		value = clamp(value, getMinValue(), getMaxValue());
		if (_module) {
			_module->_initialClockPeriod = value;
		}
	}

	float getValue() override {
		if (_module) {
			return _module->_initialClockPeriod;
		}
		return RGate::defaultInitialClockPeriod;
	}

	float getMinValue() override { return 0.0f; }
	float getMaxValue() override { return 1.0; }
	float getDefaultValue() override { return RGate::defaultInitialClockPeriod; }
	float getDisplayValue() override { return getValue() * 1000.0f; }
	void setDisplayValue(float displayValue) override { setValue(displayValue / 1000.0f); }
	std::string getLabel() override { return "Initial clock"; }
	std::string getUnit() override { return "ms"; }
};

struct IPSlider : ui::Slider {
	IPSlider(RGate* module) {
		quantity = new IPQuantity(module);
		box.size.x = 200.0f;
	}
	virtual ~IPSlider() {
		delete quantity;
	}
};

struct IPMenuItem : MenuItem {
	RGate* _module;

	IPMenuItem(RGate* m) : _module(m) {
		this->text = "Initial clock";
		this->rightText = "▸";
	}

	Menu* createChildMenu() override {
		Menu* menu = new Menu;
		menu->addChild(new IPSlider(_module));
		return menu;
	}
};

struct RGateWidget : BGModuleWidget {
	static constexpr int hp = 5;

	RGateWidget(RGate* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "RGate");
		createScrews();

		// generated by svg_widgets.rb
		auto lengthParamPosition = Vec(18.5, 48.0);
		auto clockDivideParamPosition = Vec(24.5, 129.0);
		auto clockMultiplyParamPosition = Vec(24.5, 200.0);

		auto lengthInputPosition = Vec(10.5, 251.0);
		auto clockDivideInputPosition = Vec(40.5, 251.0);
		auto resetInputPosition = Vec(10.5, 288.0);
		auto clockMultipleInputPosition = Vec(40.5, 288.0);
		auto clockInputPosition = Vec(10.5, 325.0);

		auto gateOutputPosition = Vec(40.5, 325.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(lengthParamPosition, module, RGate::LENGTH_PARAM));
		addParam(createParam<Knob26>(clockDivideParamPosition, module, RGate::CLOCK_DIVIDE_PARAM));
		addParam(createParam<Knob26>(clockMultiplyParamPosition, module, RGate::CLOCK_MULTIPLY_PARAM));

		addInput(createInput<Port24>(lengthInputPosition, module, RGate::LENGTH_INPUT));
		addInput(createInput<Port24>(clockDivideInputPosition, module, RGate::CLOCK_DIVIDE_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, RGate::RESET_INPUT));
		addInput(createInput<Port24>(clockMultipleInputPosition, module, RGate::CLOCK_MULTIPLE_INPUT));
		addInput(createInput<Port24>(clockInputPosition, module, RGate::CLOCK_INPUT));

		addOutput(createOutput<Port24>(gateOutputPosition, module, RGate::GATE_OUTPUT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<RGate*>(module);
		assert(m);

		OptionsMenuItem* p = new OptionsMenuItem("Polyphony channels from");
		p->addItem(OptionMenuItem("CLOCK input", [m]() { return m->_polyInputID == RGate::CLOCK_INPUT; }, [m]() { m->_polyInputID = RGate::CLOCK_INPUT; }));
		p->addItem(OptionMenuItem("LEN input", [m]() { return m->_polyInputID == RGate::LENGTH_INPUT; }, [m]() { m->_polyInputID = RGate::LENGTH_INPUT; }));
		OptionsMenuItem::addToMenu(p, menu);

		OptionsMenuItem* r = new OptionsMenuItem("RESET mode");
		r->addItem(OptionMenuItem("Hard: reset clock period and divider", [m]() { return m->_resetMode == RGate::HARD_RESETMODE; }, [m]() {  m->_resetMode = RGate::HARD_RESETMODE; }));
		r->addItem(OptionMenuItem("Soft: reseet clock divider", [m]() { return m->_resetMode == RGate::SOFT_RESETMODE; }, [m]() {  m->_resetMode = RGate::SOFT_RESETMODE; }));
		OptionsMenuItem::addToMenu(r, menu);

		menu->addChild(new IPMenuItem(m));

		OptionsMenuItem* mi = new OptionsMenuItem("Range");
		mi->addItem(OutputRangeOptionMenuItem(m, "0V-10V", 1.0f, 5.0f));
		mi->addItem(OutputRangeOptionMenuItem(m, "0V-5V", 1.0f, 2.5f));
		mi->addItem(OutputRangeOptionMenuItem(m, "+/-10V", 0.0f, 10.0f));
		mi->addItem(OutputRangeOptionMenuItem(m, "+/-5V", 0.0f, 5.0f));
		OptionsMenuItem::addToMenu(mi, menu);
	}
};

Model* modelRGate = createModel<RGate, RGateWidget>("Bogaudio-RGate", "RGATE", "Clock-relative gate generator & clock divider/multiplier", "Clock modulator", "Polyphonic");
