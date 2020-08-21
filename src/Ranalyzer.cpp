
#include "Ranalyzer.hpp"

// #define ANALYSIS_TRACE 1

#define RANGE_KEY "range"
#define RANGE_DB_KEY "range_db"
#define DISPLAY_ALL "display_all"

void Ranalyzer::reset() {
	_trigger.reset();
	_triggerPulseGen.process(10.0f);
	_eocPulseGen.process(10.0f);
	_core.resetChannels();
	_chirp.reset();
	_run = false;
}

void Ranalyzer::sampleRateChange() {
	reset();
	_sampleRate = APP->engine->getSampleRate();
	_sampleTime = 1.0f / _sampleRate;
	_maxFrequency = roundf(maxFrequencyNyquistRatio * _sampleRate);
	_chirp.setSampleRate(_sampleRate);
}

json_t* Ranalyzer::toJson(json_t* root) {
	json_object_set_new(root, RANGE_KEY, json_real(_range));
	json_object_set_new(root, RANGE_DB_KEY, json_real(_rangeDb));
#ifdef ANALYSIS_TRACE
	json_object_set_new(root, DISPLAY_ALL, json_boolean(_displayAll));
#endif
	return root;
}

void Ranalyzer::fromJson(json_t* root) {
	json_t* jr = json_object_get(root, RANGE_KEY);
	if (jr) {
		_range = clamp(json_real_value(jr), -0.9f, 0.8f);
	}

	json_t* jrd = json_object_get(root, RANGE_DB_KEY);
	if (jrd) {
		_rangeDb = clamp(json_real_value(jrd), 80.0f, 140.0f);
	}

#ifdef ANALYSIS_TRACE
	json_t* da = json_object_get(root, DISPLAY_ALL);
	if (da) {
		setDisplayAll(json_boolean_value(da));
	}
#endif
}

void Ranalyzer::modulate() {
	_rangeMinHz = 0.0f;
	_rangeMaxHz = 0.5f * _sampleRate;
	if (_range < 0.0f) {
		_rangeMaxHz *= 1.0f + _range;
	}
	else if (_range > 0.0f) {
		_rangeMinHz = _range * _rangeMaxHz;
	}

	_exponential = params[EXPONENTIAL_PARAM].getValue() > 0.5f;
	_loop = params[LOOP_PARAM].getValue() > 0.5f;
	_returnSampleDelay = clamp((int)roundf(params[DELAY_PARAM].getValue()), 2, maxResponseDelay);

	_frequency1 = clamp(params[FREQUENCY1_PARAM].getValue(), 0.0f, 1.0f);
	_frequency1 *= _frequency1;
	_frequency1 *= _maxFrequency - minFrequency;
	_frequency1 += minFrequency;

	_frequency2 = clamp(params[FREQUENCY2_PARAM].getValue(), 0.0f, 1.0f);
	_frequency2 *= _frequency2;
	_frequency2 *= _maxFrequency - minFrequency;
	_frequency2 += minFrequency;
}

void Ranalyzer::processAll(const ProcessArgs& args) {
	bool triggered = _trigger.process(params[TRIGGER_PARAM].getValue()*5.0f + inputs[TRIGGER_INPUT].getVoltage());
	if (!_run) {
		if (triggered || _loop) {
			_run = true;
			_bufferCount = _currentReturnSampleDelay = _returnSampleDelay;
			_chirp.reset();
			_chirp.setParams(_frequency1, _frequency2, (float)_core.size() / _sampleRate, !_exponential);
			_triggerPulseGen.trigger(0.001f);
		}
	}

	float out = 0.0f;
	if (_run) {
		if (inputs[TEST_INPUT].isConnected()) {
			out = inputs[TEST_INPUT].getVoltage();
		}
		else {
			out = _chirp.next() * 5.0f;
		}
		_inputBuffer.push(out);
		if (_bufferCount > 0) {
			--_bufferCount;
		}
		else {
			_core.stepChannelSample(0, _inputBuffer.value(_currentReturnSampleDelay - 1));
			_core.stepChannelSample(1, inputs[RETURN_INPUT].getVoltage());
		}

		if (_chirp.isCycleComplete()) {
			_run = false;
			_flush = true;
			_bufferCount = _currentReturnSampleDelay;
		}
	}
	else if (_flush) {
		_core.stepChannelSample(0, _inputBuffer.value(_currentReturnSampleDelay - 1));
		_core.stepChannelSample(1, inputs[RETURN_INPUT].getVoltage());
		--_bufferCount;
		if (_bufferCount < 1) {
			_flush = false;
			_eocPulseGen.trigger(0.001f);
		}
	}

	outputs[SEND_OUTPUT].setVoltage(out);
	outputs[TRIGGER_OUTPUT].setVoltage(_triggerPulseGen.process(_sampleTime) * 5.0f);
	outputs[EOC_OUTPUT].setVoltage(_eocPulseGen.process(_sampleTime) * 5.0f);
}

void Ranalyzer::setDisplayAll(bool displayAll) {
	_displayAll = displayAll;
	if (_channelDisplayListener) {
		if (_displayAll) {
			_channelDisplayListener->displayChannels(true, true, true);
		}
		else {
			_channelDisplayListener->displayChannels(false, false, true);
		}
	}
}

void Ranalyzer::setChannelDisplayListener(ChannelDisplayListener* listener) {
	_channelDisplayListener = listener;
}


struct AnalysisBinsReader : AnalyzerDisplay::BinsReader {
	AnalysisBinsReader(Ranalyzer* module) : AnalyzerDisplay::BinsReader(module) {}

	float at(int i) override {
		assert(_base->_core._nChannels == 3);

		float test = _base->_core.getBins(0)[i];
		float response = _base->_core.getBins(1)[i];
		if (test > 0.0001f) {
			return response / test;
		}
		return response;
	}
};


struct RanalyzerDisplay : AnalyzerDisplay, ChannelDisplayListener {
	RanalyzerDisplay(Ranalyzer* module, Vec size, bool drawInset)
	: AnalyzerDisplay(module, size, drawInset)
	{}

	void displayChannels(bool c0, bool c1, bool c2) override {
		displayChannel(0, c0);
		displayChannel(1, c1);
		displayChannel(2, c2);
	}
};


struct RanalyzerWidget : BGModuleWidget {
	static constexpr int hp = 45;

	RanalyzerWidget(Ranalyzer* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Ranalyzer", false);

		{
			auto inset = Vec(75, 1);
			auto size = Vec(box.size.x - inset.x - 1, 378);
			auto display = new RanalyzerDisplay(module, size, false);
			display->box.pos = inset;
			display->box.size = size;
#ifdef ANALYSIS_TRACE
			if (module) {
				display->setChannelBinsReader(2, new AnalysisBinsReader(module));
				module->setChannelDisplayListener(display);
			}
#endif
			addChild(display);
		}

		// generated by svg_widgets.rb
		auto frequency1ParamPosition = Vec(24.5, 42.0);
		auto frequency2ParamPosition = Vec(24.5, 103.5);
		auto triggerParamPosition = Vec(18.0, 154.0);
		auto exponentialParamPosition = Vec(23.0, 213.0);
		auto loopParamPosition = Vec(62.0, 213.0);
		auto delayParamPosition = Vec(29.5, 249.5);

		auto triggerInputPosition = Vec(40.5, 151.0);
		auto testInputPosition = Vec(30.5, 181.0);
		auto returnInputPosition = Vec(40.5, 323.0);

		auto triggerOutputPosition = Vec(10.5, 286.0);
		auto eocOutputPosition = Vec(40.5, 286.0);
		auto sendOutputPosition = Vec(10.5, 323.0);
		// end generated by svg_widgets.rb

		{
			auto w = createParam<Knob26>(frequency1ParamPosition, module, Ranalyzer::FREQUENCY1_PARAM);
			auto k = dynamic_cast<BGKnob*>(w);
			k->skinChanged("dark");
			addParam(w);
		}
		{
			auto w = createParam<Knob26>(frequency2ParamPosition, module, Ranalyzer::FREQUENCY2_PARAM);
			auto k = dynamic_cast<BGKnob*>(w);
			k->skinChanged("dark");
			addParam(w);
		}
		addParam(createParam<Button18>(triggerParamPosition, module, Ranalyzer::TRIGGER_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(exponentialParamPosition, module, Ranalyzer::EXPONENTIAL_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(loopParamPosition, module, Ranalyzer::LOOP_PARAM));
		{
			auto w = createParam<Knob16>(delayParamPosition, module, Ranalyzer::DELAY_PARAM);
			auto k = dynamic_cast<SvgKnob*>(w);
			k->snap = true;
			addParam(w);
		}

		addInput(createInput<Port24>(triggerInputPosition, module, Ranalyzer::TRIGGER_INPUT));
		addInput(createInput<Port24>(testInputPosition, module, Ranalyzer::TEST_INPUT));
		addInput(createInput<Port24>(returnInputPosition, module, Ranalyzer::RETURN_INPUT));

		addOutput(createOutput<Port24>(triggerOutputPosition, module, Ranalyzer::TRIGGER_OUTPUT));
		addOutput(createOutput<Port24>(eocOutputPosition, module, Ranalyzer::EOC_OUTPUT));
		addOutput(createOutput<Port24>(sendOutputPosition, module, Ranalyzer::SEND_OUTPUT));
	}

	void contextMenu(Menu* menu) override {
		auto a = dynamic_cast<Ranalyzer*>(module);
		assert(a);

		menu->addChild(new MenuLabel());
		{
			OptionsMenuItem* mi = new OptionsMenuItem("Frequency range");
			mi->addItem(OptionMenuItem("Lower 25%", [a]() { return a->_range == -0.75f; }, [a]() { a->_range = -0.75f; }));
			mi->addItem(OptionMenuItem("Lower 50%", [a]() { return a->_range == -0.5f; }, [a]() { a->_range = -0.5f; }));
			mi->addItem(OptionMenuItem("Full", [a]() { return a->_range == 0.0f; }, [a]() { a->_range = 0.0f; }));
			mi->addItem(OptionMenuItem("Upper 50%", [a]() { return a->_range == 0.5f; }, [a]() { a->_range = 0.5f; }));
			mi->addItem(OptionMenuItem("Upper 25%", [a]() { return a->_range == 0.75f; }, [a]() { a->_range = 0.75f; }));
			OptionsMenuItem::addToMenu(mi, menu);
		}
		{
			OptionsMenuItem* mi = new OptionsMenuItem("Amplitude range");
			mi->addItem(OptionMenuItem("To -60dB", [a]() { return a->_rangeDb == 80.0f; }, [a]() { a->_rangeDb = 80.0f; }));
			mi->addItem(OptionMenuItem("To -120dB", [a]() { return a->_rangeDb == 140.0f; }, [a]() { a->_rangeDb = 140.0f; }));
			OptionsMenuItem::addToMenu(mi, menu);
		}
#ifdef ANALYSIS_TRACE
		{
			OptionsMenuItem* mi = new OptionsMenuItem("Display traces");
			mi->addItem(OptionMenuItem("All", [a]() { return a->_displayAll; }, [a]() { a->setDisplayAll(true); }));
			mi->addItem(OptionMenuItem("Analysis only", [a]() { return !a->_displayAll; }, [a]() { a->setDisplayAll(false); }));
			OptionsMenuItem::addToMenu(mi, menu);
		}
#endif
	}
};

Model* modelRanalyzer = createModel<Ranalyzer, RanalyzerWidget>("Bogaudio-Ranalyzer", "RANALYZER", "Swept-sine frequency response analyzer", "Visual");
