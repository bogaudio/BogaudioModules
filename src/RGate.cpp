
#include "RGate.hpp"

#define RUN_MODE "run_mode"
#define INITIAL_PULSE_SECONDS "initial_pulse_seconds"
#define POLY_INPUT "poly_input"

void RGate::Engine::reset(bool triggers, bool hard) {
	if (triggers) {
		clockTrigger.reset();
		runTrigger.reset();
		initialGatePulseGen.process(10.0f);
	}
	if (hard) {
		secondsSinceLastClock = -1.0f;
		clockSeconds = -1.0f;
		dividedSeconds = -1.0f;
		multipliedSeconds = -1.0f;
		gateSeconds = 0.0f;
	}
	dividerCount = 0;
	dividedProgressSeconds = 0.0f;
}

void RGate::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void RGate::sampleRateChange() {
	_sampleTime = APP->engine->getSampleTime();
}

json_t* RGate::toJson(json_t* root) {
	json_object_set_new(root, RUN_MODE, json_integer(_runMode));
	json_object_set_new(root, INITIAL_PULSE_SECONDS, json_real(_initialPulseSeconds));
	json_object_set_new(root, POLY_INPUT, json_integer(_polyInputID));
	return root;
}

void RGate::fromJson(json_t* root) {
	json_t* rm = json_object_get(root, RUN_MODE);
	if (rm) {
		RunMode m = (RunMode)json_integer_value(rm);
		switch (m) {
			case GATED_RUN_RUNMODE:
			case GATED_RUN_RESET_SOFT_RUNMODE:
			case GATED_RUN_RESET_HARD_RUNMODE:
			case TRIGGERED_RUN_RUNMODE:
			case TRIGGERED_RUN_RESET_SOFT_RUNMODE:
			case TRIGGERED_RUN_RESET_HARD_RUNMODE:
			case RESET_SOFT_RUNMODE:
			case RESET_HARD_RUNMODE: {
				_runMode = m;
				break;
			}
			default: {
				_runMode = defaultRunMode;
			}
		}
	}

	json_t* ips = json_object_get(root, INITIAL_PULSE_SECONDS);
	if (ips) {
		_initialPulseSeconds = std::max(0.0f, (float)json_real_value(ips));
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
	_engines[c]->reset();
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

	bool runTriggered = e.runTrigger.process(inputs[RUN_INPUT].getPolyVoltage(c));
	switch (_runMode) {
		case GATED_RUN_RUNMODE: {
			_running = e.runTrigger.isHigh() ? YES_RUNNING : NO_RUNNING;
			break;
		}
		case GATED_RUN_RESET_SOFT_RUNMODE: {
			_running = e.runTrigger.isHigh() ? YES_RUNNING : NO_RUNNING;
			if (runTriggered) {
				e.reset(false, false);
			}
			break;
		}
		case GATED_RUN_RESET_HARD_RUNMODE: {
			_running = e.runTrigger.isHigh() ? YES_RUNNING : NO_RUNNING;
			if (runTriggered) {
				e.reset(false, true);
			}
			break;
		}
		case TRIGGERED_RUN_RUNMODE: {
			if (runTriggered) {
				switch (_running) {
					case UNKNOWN_RUNNING:
					case NO_RUNNING: {
						_running = YES_RUNNING;
						break;
					}
					case YES_RUNNING: {
						_running = NO_RUNNING;
						break;
					}
				}
			}
			break;
		}
		case TRIGGERED_RUN_RESET_SOFT_RUNMODE: {
			if (runTriggered) {
				switch (_running) {
					case UNKNOWN_RUNNING:
					case NO_RUNNING: {
						_running = YES_RUNNING;
						break;
					}
					case YES_RUNNING: {
						_running = NO_RUNNING;
						break;
					}
				}
				e.reset(false, false);
			}
			break;
		}
		case TRIGGERED_RUN_RESET_HARD_RUNMODE: {
			if (runTriggered) {
				switch (_running) {
					case UNKNOWN_RUNNING:
					case NO_RUNNING: {
						_running = YES_RUNNING;
						break;
					}
					case YES_RUNNING: {
						_running = NO_RUNNING;
						break;
					}
				}
				e.reset(false, true);
			}
			break;
		}
		case RESET_SOFT_RUNMODE: {
			if (runTriggered) {
				e.reset(false, false);
			}
			break;
		}
		case RESET_HARD_RUNMODE: {
			if (runTriggered) {
				e.reset(false, true);
			}
			break;
		}
	}

	bool clock = false;
	if (inputs[CLOCK_INPUT].isConnected()) {
		clock = e.clockTrigger.process(inputs[CLOCK_INPUT].getPolyVoltage(c));
		if (clock) {
			if (e.secondsSinceLastClock > 0.0f) {
				e.clockSeconds = e.secondsSinceLastClock;
			}
			e.secondsSinceLastClock = 0.0f;

			if (_running == UNKNOWN_RUNNING) {
				_running = YES_RUNNING;
			}
		}
		e.secondsSinceLastClock += _sampleTime;
	}

	float out = 0.0f;
	if (e.clockSeconds > 0.0f) {
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

		float multipliedProgressSeconds = e.dividedProgressSeconds / e.multipliedSeconds;
		multipliedProgressSeconds -= (float)(int)multipliedProgressSeconds;
		multipliedProgressSeconds *= e.multipliedSeconds;
		out = (float)(multipliedProgressSeconds <= e.gateSeconds);
	}
	else {
		if (clock) {
			++e.dividerCount;
			if (_initialPulseSeconds > 0.0f) {
				e.initialGatePulseGen.trigger(_initialPulseSeconds);
			}
		}
		e.dividedProgressSeconds += _sampleTime;
		out = (float)e.initialGatePulseGen.process(_sampleTime);
	}

	outputs[GATE_OUTPUT].setChannels(_channels);
	outputs[GATE_OUTPUT].setVoltage(out * (float)(_running == YES_RUNNING) * 10.0f, c);
}

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
		auto runInputPosition = Vec(10.5, 288.0);
		auto clockMultipleInputPosition = Vec(40.5, 288.0);
		auto clockInputPosition = Vec(10.5, 325.0);

		auto gateOutputPosition = Vec(40.5, 325.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(lengthParamPosition, module, RGate::LENGTH_PARAM));
		addParam(createParam<Knob26>(clockDivideParamPosition, module, RGate::CLOCK_DIVIDE_PARAM));
		addParam(createParam<Knob26>(clockMultiplyParamPosition, module, RGate::CLOCK_MULTIPLY_PARAM));

		addInput(createInput<Port24>(lengthInputPosition, module, RGate::LENGTH_INPUT));
		addInput(createInput<Port24>(clockDivideInputPosition, module, RGate::CLOCK_DIVIDE_INPUT));
		addInput(createInput<Port24>(runInputPosition, module, RGate::RUN_INPUT));
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

		OptionsMenuItem* r = new OptionsMenuItem("RUN port");
		r->addItem(OptionMenuItem("Toggle run on trigger, hard reset on start", [m]() { return m->_runMode == RGate::TRIGGERED_RUN_RESET_HARD_RUNMODE; }, [m]() {  m->_runMode = RGate::TRIGGERED_RUN_RESET_HARD_RUNMODE; }));
		r->addItem(OptionMenuItem("Toggle run on trigger, soft reset on start", [m]() { return m->_runMode == RGate::TRIGGERED_RUN_RESET_SOFT_RUNMODE; }, [m]() {  m->_runMode = RGate::TRIGGERED_RUN_RESET_SOFT_RUNMODE; }));
		r->addItem(OptionMenuItem("Toggle run on trigger", [m]() { return m->_runMode == RGate::TRIGGERED_RUN_RUNMODE; }, [m]() {  m->_runMode = RGate::TRIGGERED_RUN_RUNMODE; }));
		r->addItem(OptionMenuItem("Run when gate high, hard reset on rising edge", [m]() { return m->_runMode == RGate::GATED_RUN_RESET_HARD_RUNMODE; }, [m]() {  m->_runMode = RGate::GATED_RUN_RESET_HARD_RUNMODE; }));
		r->addItem(OptionMenuItem("Run when gate high, soft reset on rising edge", [m]() { return m->_runMode == RGate::GATED_RUN_RESET_SOFT_RUNMODE; }, [m]() {  m->_runMode = RGate::GATED_RUN_RESET_SOFT_RUNMODE; }));
		r->addItem(OptionMenuItem("Run when gate high", [m]() { return m->_runMode == RGate::GATED_RUN_RUNMODE; }, [m]() {  m->_runMode = RGate::GATED_RUN_RUNMODE; }));
		r->addItem(OptionMenuItem("Hard reset on trigger", [m]() { return m->_runMode == RGate::RESET_HARD_RUNMODE; }, [m]() {  m->_runMode = RGate::RESET_HARD_RUNMODE; }));
		r->addItem(OptionMenuItem("Soft reset on trigger", [m]() { return m->_runMode == RGate::RESET_SOFT_RUNMODE; }, [m]() {  m->_runMode = RGate::RESET_SOFT_RUNMODE; }));
		OptionsMenuItem::addToMenu(r, menu);

		OptionsMenuItem* i = new OptionsMenuItem("Initial pulse duration");
		i->addItem(OptionMenuItem("No pulse", [m]() { return m->_initialPulseSeconds == 0.0f; }, [m]() {  m->_initialPulseSeconds = 0.0f; }));
		i->addItem(OptionMenuItem("1ms", [m]() { return m->_initialPulseSeconds == 0.001f; }, [m]() {  m->_initialPulseSeconds = 0.001f; }));
		i->addItem(OptionMenuItem("100ms", [m]() { return m->_initialPulseSeconds == 0.1f; }, [m]() {  m->_initialPulseSeconds = 0.1f; }));
		i->addItem(OptionMenuItem("200ms", [m]() { return m->_initialPulseSeconds == 0.2f; }, [m]() {  m->_initialPulseSeconds = 0.2f; }));
		i->addItem(OptionMenuItem("300ms", [m]() { return m->_initialPulseSeconds == 0.3f; }, [m]() {  m->_initialPulseSeconds = 0.3f; }));
		i->addItem(OptionMenuItem("400ms", [m]() { return m->_initialPulseSeconds == 0.4f; }, [m]() {  m->_initialPulseSeconds = 0.4f; }));
		i->addItem(OptionMenuItem("500ms", [m]() { return m->_initialPulseSeconds == 0.5f; }, [m]() {  m->_initialPulseSeconds = 0.4f; }));
		OptionsMenuItem::addToMenu(i, menu);
	}
};

Model* modelRGate = createModel<RGate, RGateWidget>("Bogaudio-RGate", "RGATE", "Clock-relative gate generator & clock divider/multiplier", "Clock modulator", "Polyphonic");
