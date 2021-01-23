
#include "Ranalyzer.hpp"

#define TRIGGER_ON_LOAD "triggerOnLoad"
#define DISPLAY_TRACES "display_traces"
#define DISPLAY_TRACES_ALL "all"
#define DISPLAY_TRACES_TEST_RETURN "test_return"
#define DISPLAY_TRACES_ANALYSIS "analysis"
#define WINDOW_TYPE "window_type"
#define WINDOW_TYPE_NONE "none"
#define WINDOW_TYPE_TAPER "taper"
#define WINDOW_TYPE_HAMMING "hamming"
#define WINDOW_TYPE_KAISER "Kaiser"

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
	_rangeMinHz = 0.0f;
	_rangeMaxHz = 0.5f * _sampleRate;
	if (_sampleRate >= 96000.0f) {
		_core.setParams(_sampleRate, 1, AnalyzerCore::QUALITY_FIXED_32K, AnalyzerCore::WINDOW_NONE);
	}
	else {
		_core.setParams(_sampleRate, 1, AnalyzerCore::QUALITY_FIXED_16K, AnalyzerCore::WINDOW_NONE);
	}
	setWindow(_windowType);
	_run = false;
	_flush = false;
	if (!_initialDelay) {
		_initialDelay = new Timer(_sampleRate, initialDelaySeconds);
	}
}

json_t* Ranalyzer::toJson(json_t* root) {
	frequencyPlotToJson(root);
	frequencyRangeToJson(root);
	amplitudePlotToJson(root);
	json_object_set_new(root, TRIGGER_ON_LOAD, json_boolean(_triggerOnLoad));

	switch (_displayTraces) {
		case ALL_TRACES: {
			json_object_set_new(root, DISPLAY_TRACES, json_string(DISPLAY_TRACES_ALL));
			break;
		}
		case TEST_RETURN_TRACES: {
			json_object_set_new(root, DISPLAY_TRACES, json_string(DISPLAY_TRACES_TEST_RETURN));
			break;
		}
		case ANALYSIS_TRACES: {
			json_object_set_new(root, DISPLAY_TRACES, json_string(DISPLAY_TRACES_ANALYSIS));
			break;
		}
	}

	switch (_windowType) {
		case NONE_WINDOW_TYPE: {
			json_object_set_new(root, WINDOW_TYPE, json_string(WINDOW_TYPE_NONE));
			break;
		}
		case TAPER_WINDOW_TYPE: {
			json_object_set_new(root, WINDOW_TYPE, json_string(WINDOW_TYPE_TAPER));
			break;
		}
		case HAMMING_WINDOW_TYPE: {
			json_object_set_new(root, WINDOW_TYPE, json_string(WINDOW_TYPE_HAMMING));
			break;
		}
		case KAISER_WINDOW_TYPE: {
			json_object_set_new(root, WINDOW_TYPE, json_string(WINDOW_TYPE_KAISER));
			break;
		}
	}

	return root;
}

void Ranalyzer::fromJson(json_t* root) {
	frequencyPlotFromJson(root);
	frequencyRangeFromJson(root);
	amplitudePlotFromJson(root);

	json_t* t = json_object_get(root, TRIGGER_ON_LOAD);
	if (t) {
		_triggerOnLoad = json_boolean_value(t);
	}

	json_t* dt = json_object_get(root, DISPLAY_TRACES);
	if (dt) {
		std::string dts = json_string_value(dt);
		if (dts == DISPLAY_TRACES_ALL) {
			setDisplayTraces(ALL_TRACES);
		}
		else if (dts == DISPLAY_TRACES_TEST_RETURN) {
			setDisplayTraces(TEST_RETURN_TRACES);
		}
		else if (dts == DISPLAY_TRACES_ANALYSIS) {
			setDisplayTraces(ANALYSIS_TRACES);
		}
	}

	json_t* wt = json_object_get(root, WINDOW_TYPE);
	if (wt) {
		std::string wts = json_string_value(wt);
		if (wts == WINDOW_TYPE_NONE) {
			setWindow(NONE_WINDOW_TYPE);
		}
		else if (wts == WINDOW_TYPE_TAPER) {
			setWindow(TAPER_WINDOW_TYPE);
		}
		else if (wts == WINDOW_TYPE_HAMMING) {
			setWindow(HAMMING_WINDOW_TYPE);
		}
		else if (wts == WINDOW_TYPE_KAISER) {
			setWindow(KAISER_WINDOW_TYPE);
		}
	}
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
	bool maybeTriggerOnLoad = false;
	if (_initialDelay && !_initialDelay->next()) {
		maybeTriggerOnLoad = true;
		delete _initialDelay;
		_initialDelay = NULL;
	}

	bool triggered = _trigger.process(params[TRIGGER_PARAM].getValue()*5.0f + inputs[TRIGGER_INPUT].getVoltage());
	if (!_run) {
		if (triggered || (!_initialDelay && _loop) || (maybeTriggerOnLoad && _triggerOnLoad)) {
			_run = true;
			_outBufferCount = _currentReturnSampleDelay = _returnSampleDelay;
			_chirp.reset();
			_cycleN = _core.size();
			_cycleI = 0;
			_chirp.setParams(_frequency1, _frequency2, _core.size() / (double)_sampleRate, !_exponential);
			_triggerPulseGen.trigger(0.001f);
			_useTestInput = inputs[TEST_INPUT].isConnected();
		}
	}

	float out = 0.0f;
	if (_run) {
		if (_useTestInput) {
			out = inputs[TEST_INPUT].getVoltage();
		}
		else {
			out = _chirp.next() * 5.0f;
		}

		_inputBuffer.push(out);
		if (_outBufferCount > 0) {
			--_outBufferCount;
		}
		else {
			float w = _window ? _window->at(_cycleI - _currentReturnSampleDelay) : 1.0f;
			_core.stepChannelSample(0, w * _inputBuffer.value(_currentReturnSampleDelay - 1));
			_core.stepChannelSample(1, w * inputs[RETURN_INPUT].getVoltage());
		}

		++_cycleI;
		if (_cycleI >= _cycleN) {
			_run = false;
			_flush = true;
			_analysisBufferCount = _currentReturnSampleDelay;
		}
	}
	if (_flush) {
		float w = _window ? _window->at(_cycleN - _analysisBufferCount) : 1.0f;
		_core.stepChannelSample(0, w * _inputBuffer.value((_run ? _currentReturnSampleDelay : _analysisBufferCount) - 1));
		_core.stepChannelSample(1, w * inputs[RETURN_INPUT].getVoltage());
		--_analysisBufferCount;
		if (_analysisBufferCount < 1) {
			_flush = false;
			_eocPulseGen.trigger(0.001f);
		}
	}

	outputs[SEND_OUTPUT].setVoltage(out);
	outputs[TRIGGER_OUTPUT].setVoltage(_triggerPulseGen.process(_sampleTime) * 5.0f);
	outputs[EOC_OUTPUT].setVoltage(_eocPulseGen.process(_sampleTime) * 5.0f);
}

void Ranalyzer::setDisplayTraces(Traces traces) {
	_displayTraces = traces;
	if (_channelDisplayListener) {
		switch (_displayTraces) {
			case ALL_TRACES: {
				_channelDisplayListener->displayChannels(true, true, true);
				break;
			}
			case TEST_RETURN_TRACES: {
				_channelDisplayListener->displayChannels(true, true, false);
				break;
			}
			case ANALYSIS_TRACES: {
				_channelDisplayListener->displayChannels(false, false, true);
				break;
			}
		}
	}
}

void Ranalyzer::setChannelDisplayListener(ChannelDisplayListener* listener) {
	_channelDisplayListener = listener;
}

void Ranalyzer::setWindow(WindowType wt) {
	if (!_window || _windowType != wt || _window->size() != _core.size()) {
		if (_window) {
			delete _window;
			_window = NULL;
		}

		_windowType = wt;
		switch (_windowType) {
			case NONE_WINDOW_TYPE: break;
			case TAPER_WINDOW_TYPE: {
				_window = new PlanckTaperWindow(_core.size(), (int)(_core.size() * 0.03f));
				break;
			}
			case HAMMING_WINDOW_TYPE: {
				_window = new HammingWindow(_core.size());
				break;
			}
			case KAISER_WINDOW_TYPE: {
				_window = new KaiserWindow(_core.size());
				break;
			}
		}
	}
}


struct AnalysisBinsReader : AnalyzerDisplay::BinsReader {
	float* _testBins;
	float* _responseBins;

	AnalysisBinsReader(float* testBins, float* responseBins) : _testBins(testBins), _responseBins(responseBins) {}

	float at(int i) override {
		float test = AnalyzerDisplay::binValueToDb(_testBins[i]);
		float response = AnalyzerDisplay::binValueToDb(_responseBins[i]);
		return AnalyzerDisplay::dbToBinValue(response = test);
	}

	static std::unique_ptr<BinsReader> factory(AnalyzerCore& core) {
		assert(core._nChannels == 3);
		return std::unique_ptr<BinsReader>(new AnalysisBinsReader(core.getBins(0), core.getBins(1)));
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

	void drawHeader(const DrawArgs& args, float rangeMinHz, float rangeMaxHz) override {
		nvgSave(args.vg);

		const int textY = -4;
		const int charPx = 5;
		int x = _insetAround + 2;

		std::string s = format("Bin width %0.1f HZ", APP->engine->getSampleRate() / (float)(_module->_core._size / _module->_core._binAverageN));
		drawText(args, s.c_str(), x, _insetTop + textY);
		x += s.size() * charPx + 20;

		const char* labels[3] = { "TEST", "RESPONSE", "ANALYSIS" };
		for (int i = 0; i < 3; ++i) {
			if (_displayChannel[i]) {
				auto color = _channelColors[i % channelColorsN];
				nvgStrokeColor(args.vg, color);
				nvgStrokeWidth(args.vg, std::max(1.0f, 3.0f - getZoom()));
				nvgBeginPath(args.vg);
				float lineY = _insetTop - 7.0f;
				nvgMoveTo(args.vg, x, lineY);
				x += 10.0f;
				nvgLineTo(args.vg, x, lineY);
				x += 3.0f;
				nvgStroke(args.vg);

				drawText(args, labels[i], x, _insetTop + textY, 0.0, &color);
				x += strlen(labels[i]) * charPx + 20;
			}
		}

		nvgRestore(args.vg);
	}
};


struct RanalyzerWidget : AnalyzerBaseWidget {
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
			if (module) {
				display->setChannelBinsReaderFactory(2, AnalysisBinsReader::factory);
				module->setChannelDisplayListener(display);
				display->channelLabel(0, "Test");
				display->channelLabel(1, "Response");
				display->channelLabel(2, "Analysis");
			}
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
			OptionsMenuItem* mi = new OptionsMenuItem("Display traces");
			mi->addItem(OptionMenuItem("All", [a]() { return a->_displayTraces == Ranalyzer::ALL_TRACES; }, [a]() { a->setDisplayTraces(Ranalyzer::ALL_TRACES); }));
			mi->addItem(OptionMenuItem("Analysis only", [a]() { return a->_displayTraces == Ranalyzer::ANALYSIS_TRACES; }, [a]() { a->setDisplayTraces(Ranalyzer::ANALYSIS_TRACES); }));
			mi->addItem(OptionMenuItem("Test/return only", [a]() { return a->_displayTraces == Ranalyzer::TEST_RETURN_TRACES; }, [a]() { a->setDisplayTraces(Ranalyzer::TEST_RETURN_TRACES); }));
			OptionsMenuItem::addToMenu(mi, menu);
		}
		{
			OptionsMenuItem* mi = new OptionsMenuItem("Window");
			mi->addItem(OptionMenuItem("None", [a]() { return a->_windowType == Ranalyzer::NONE_WINDOW_TYPE; }, [a]() { a->setWindow(Ranalyzer::NONE_WINDOW_TYPE); }));
			mi->addItem(OptionMenuItem("Taper", [a]() { return a->_windowType == Ranalyzer::TAPER_WINDOW_TYPE; }, [a]() { a->setWindow(Ranalyzer::TAPER_WINDOW_TYPE); }));
			mi->addItem(OptionMenuItem("Hamming", [a]() { return a->_windowType == Ranalyzer::HAMMING_WINDOW_TYPE; }, [a]() { a->setWindow(Ranalyzer::HAMMING_WINDOW_TYPE); }));
			mi->addItem(OptionMenuItem("Kaiser", [a]() { return a->_windowType == Ranalyzer::KAISER_WINDOW_TYPE; }, [a]() { a->setWindow(Ranalyzer::KAISER_WINDOW_TYPE); }));
			OptionsMenuItem::addToMenu(mi, menu);
		}
		addFrequencyPlotContextMenu(menu);
		addFrequencyRangeContextMenu(menu);
		addAmplitudePlotContextMenu(menu, false);
		menu->addChild(new BoolOptionMenuItem("Trigger on load", [a]() { return &a->_triggerOnLoad; }));
	}
};

Model* modelRanalyzer = createModel<Ranalyzer, RanalyzerWidget>("Bogaudio-Ranalyzer", "RANALYZER", "Swept-sine frequency response analyzer", "Visual");
