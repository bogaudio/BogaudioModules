
#include "analyzer_base.hpp"
#include "dsp/signal.hpp"

ChannelAnalyzer::~ChannelAnalyzer() {
	{
		std::lock_guard<std::mutex> lock(_workerMutex);
		_workerStop = true;
	}
	_workerCV.notify_one();
	_worker.join();
	delete[] _workerBuf;
	delete[] _stepBuf;
	if (_averagedBins) {
		delete _averagedBins;
	}
}

void ChannelAnalyzer::step(float sample) {
	_stepBuf[_stepBufI++] = sample;
	if (_stepBufI >= _stepBufN) {
		_stepBufI = 0;

		{
			std::lock_guard<std::mutex> lock(_workerMutex);
			for (int i = 0; i < _stepBufN; ++i) {
				_workerBuf[_workerBufWriteI] = _stepBuf[i];
				_workerBufWriteI = (_workerBufWriteI + 1) % _workerBufN;
				if (_workerBufWriteI == _workerBufReadI) {
					_workerBufWriteI = _workerBufReadI = 0;
					break;
				}
			}
		}
		_workerCV.notify_one();
	}
}

void ChannelAnalyzer::work() {
	bool process = false;
	MAIN: while (true) {
		if (_workerStop) {
			return;
		}

		if (process) {
			process = false;

			_analyzer.process();
			_analyzer.postProcess();
			float* bins = _bins0;
			if (_currentBins == _bins0) {
				bins = _bins1;
			}
			if (_averagedBins) {
				float* frame = _averagedBins->getInputFrame();
				_analyzer.getMagnitudes(frame, _binsN);
				_averagedBins->commitInputFrame();
				const float* averages = _averagedBins->getAverages();
				std::copy(averages, averages + _binsN, bins);
			}
			else {
				_analyzer.getMagnitudes(bins, _binsN);
			}
			_currentBins = bins;
			_currentOutBuf = _currentBins;
		}

		while (_workerBufReadI != _workerBufWriteI) {
			float sample = _workerBuf[_workerBufReadI];
			_workerBufReadI = (_workerBufReadI + 1) % _workerBufN;
			if (_analyzer.step(sample)) {
				process = true;
				goto MAIN;
			}
		}

		std::unique_lock<std::mutex> lock(_workerMutex);
		while (!(_workerBufReadI != _workerBufWriteI || _workerStop)) {
			_workerCV.wait(lock);
		}
	}
}


void AnalyzerCore::setParams(int averageN, Quality quality, Window window) {
	bool reset = false;
	if (_averageN != averageN) {
		_averageN = averageN;
		reset = true;
	}
	if (_quality != quality) {
		_quality = quality;
		reset = true;
	}
	if (_window != window) {
		_window = window;
		reset = true;
	}
	if (reset) {
		resetChannels();
	}
}

void AnalyzerCore::resetChannels() {
	_size = size();
	_binsN = _size / _binAverageN;

	std::lock_guard<std::mutex> lock(_channelsMutex);
	for (int i = 0; i < _nChannels; ++i) {
		if (_channels[i]) {
			delete _channels[i];
			_channels[i] = NULL;
		}
	}
}

SpectrumAnalyzer::Size AnalyzerCore::size() {
	switch (_quality) {
		case QUALITY_FIXED_16K: {
			return SpectrumAnalyzer::SIZE_16384;
		}
		case QUALITY_FIXED_32K: {
			return SpectrumAnalyzer::SIZE_32768;
		}
		default:;
	}

	if (APP->engine->getSampleRate() < 96000.0f) {
		switch (_quality) {
			case QUALITY_ULTRA_ULTRA: {
				return SpectrumAnalyzer::SIZE_16384;
			}
			case QUALITY_ULTRA: {
				return SpectrumAnalyzer::SIZE_8192;
			}
			case QUALITY_HIGH: {
				return SpectrumAnalyzer::SIZE_4096;
			}
			default: {
				return SpectrumAnalyzer::SIZE_1024;
			}
		}
	}
	else {
		switch (_quality) {
			case QUALITY_ULTRA_ULTRA: {
				return SpectrumAnalyzer::SIZE_32768;
			}
			case QUALITY_ULTRA: {
				return SpectrumAnalyzer::SIZE_16384;
			}
			case QUALITY_HIGH: {
				return SpectrumAnalyzer::SIZE_8192;
			}
			default: {
				return SpectrumAnalyzer::SIZE_2048;
			}
		}
	}
}

SpectrumAnalyzer::WindowType AnalyzerCore::window() {
	switch (_window) {
		case WINDOW_NONE: {
			return SpectrumAnalyzer::WINDOW_NONE;
		}
		case WINDOW_HAMMING: {
			return SpectrumAnalyzer::WINDOW_HAMMING;
		}
		default: {
			return SpectrumAnalyzer::WINDOW_KAISER;
		}
	}
}

float AnalyzerCore::getPeak(int channel) {
	assert(channel >= 0 && channel < _nChannels);
	float max = 0.0f;
	int maxBin = 0;
	const float* bins = getBins(channel);
	for (int bin = 0; bin < _binsN; ++bin) {
		if (bins[bin] > max) {
			max = bins[bin];
			maxBin = bin;
		}
	}

	const int bandsPerBin = _size / _binsN;
	const float fWidth = (APP->engine->getSampleRate() / 2.0f) / (float)(_size / bandsPerBin);

	return (maxBin + 0.5f)*fWidth;
	// ??
	// float sum = 0.0f;
	// float sumWeights = 0.0f;
	// int i = std::max(0, maxBin - 1);
	// int j = std::max(_binsN - 1, maxBin + 1);
	// for (; i <= j; ++i) {
	// 	sum += bins[i] * fWidth * i;
	// 	sumWeights += bins[i];
	// }
	// return sum / sumWeights;
}

void AnalyzerCore::stepChannel(int channelIndex, Input& input) {
	assert(channelIndex >= 0);
	assert(channelIndex < _nChannels);

	if (input.isConnected()) {
		stepChannelSample(channelIndex, input.getVoltageSum());
	}
	else if (_channels[channelIndex]) {
		std::lock_guard<std::mutex> lock(_channelsMutex);
		delete _channels[channelIndex];
		_channels[channelIndex] = NULL;
	}
}

void AnalyzerCore::stepChannelSample(int channelIndex, float sample) {
	assert(channelIndex >= 0);
	assert(channelIndex < _nChannels);

	if (!_channels[channelIndex]) {
		std::lock_guard<std::mutex> lock(_channelsMutex);
		_channels[channelIndex] = new ChannelAnalyzer(
			_size,
			_overlap,
			window(),
			APP->engine->getSampleRate(),
			_averageN,
			_binAverageN,
			_outBufs + 2 * channelIndex * _outBufferN,
			_outBufs + (2 * channelIndex + 1) * _outBufferN,
			_currentOutBufs[channelIndex]
		);
	}
	_channels[channelIndex]->step(sample);
}


#define FREQUENCY_PLOT_KEY "frequency_plot"
#define FREQUENCY_PLOT_LOG_KEY "log"
#define FREQUENCY_PLOT_LINEAR_KEY "linear"

void AnalyzerBase::frequencyPlotToJson(json_t* root) {
	switch (_frequencyPlot) {
		case LOG_FP: {
			json_object_set_new(root, FREQUENCY_PLOT_KEY, json_string(FREQUENCY_PLOT_LOG_KEY));
			break;
		}
		case LINEAR_FP: {
			json_object_set_new(root, FREQUENCY_PLOT_KEY, json_string(FREQUENCY_PLOT_LINEAR_KEY));
			break;
		}
	}
}

void AnalyzerBase::frequencyPlotFromJson(json_t* root) {
	json_t* fp = json_object_get(root, FREQUENCY_PLOT_KEY);
	if (fp) {
		std::string fps = json_string_value(fp);
		if (fps == FREQUENCY_PLOT_LOG_KEY) {
			_frequencyPlot = LOG_FP;
		}
		else if (fps == FREQUENCY_PLOT_LINEAR_KEY) {
			_frequencyPlot = LINEAR_FP;
		}
	}
}

#define RANGE_KEY "range"

void AnalyzerBase::frequencyRangeToJson(json_t* root) {
	json_object_set_new(root, RANGE_KEY, json_real(_range));
}

void AnalyzerBase::frequencyRangeFromJson(json_t* root) {
	json_t* jr = json_object_get(root, RANGE_KEY);
	if (jr) {
		_range = clamp(json_real_value(jr), -0.9f, 0.8f);
	}
}

#define RANGE_DB_KEY "range_db"
#define AMPLITUDE_PLOT_KEY "amplitude_plot"
#define AMPLITUDE_PLOT_DECIBLES_80_KEY "decibels_80"
#define AMPLITUDE_PLOT_DECIBLES_140_KEY "decibels_140"
#define AMPLITUDE_PLOT_PERCENTAGE_KEY "percentage"

void AnalyzerBase::amplitudePlotToJson(json_t* root) {
	switch (_amplitudePlot) {
		case DECIBELS_80_AP: {
			json_object_set_new(root, AMPLITUDE_PLOT_KEY, json_string(AMPLITUDE_PLOT_DECIBLES_80_KEY));
			break;
		}
		case DECIBELS_140_AP: {
			json_object_set_new(root, AMPLITUDE_PLOT_KEY, json_string(AMPLITUDE_PLOT_DECIBLES_140_KEY));
			break;
		}
		case PERCENTAGE_AP: {
			json_object_set_new(root, AMPLITUDE_PLOT_KEY, json_string(AMPLITUDE_PLOT_PERCENTAGE_KEY));
			break;
		}
	}
}

void AnalyzerBase::amplitudePlotFromJson(json_t* root) {
	json_t* ap = json_object_get(root, AMPLITUDE_PLOT_KEY);
	if (ap) {
		std::string aps = json_string_value(ap);
		if (aps == AMPLITUDE_PLOT_DECIBLES_80_KEY) {
			_amplitudePlot = DECIBELS_80_AP;
		}
		else if (aps == AMPLITUDE_PLOT_DECIBLES_140_KEY) {
			_amplitudePlot = DECIBELS_140_AP;
		}
		else if (aps == AMPLITUDE_PLOT_PERCENTAGE_KEY) {
			_amplitudePlot = PERCENTAGE_AP;
		}
	}
	else { // backwards compatibility...
		json_t* jrd = json_object_get(root, RANGE_DB_KEY);
		if (jrd) {
			if ((float)json_real_value(jrd) == 140.0f) {
				_amplitudePlot = DECIBELS_140_AP;
			}
		}
	}
}


void AnalyzerBaseWidget::addFrequencyPlotContextMenu(Menu* menu) {
	auto m = dynamic_cast<AnalyzerBase*>(module);
	assert(m);

	OptionsMenuItem* mi = new OptionsMenuItem("Frequency plot");
	mi->addItem(OptionMenuItem("Logarithmic", [m]() { return m->_frequencyPlot == AnalyzerBase::LOG_FP; }, [m]() { m->_frequencyPlot = AnalyzerBase::LOG_FP; }));
	mi->addItem(OptionMenuItem("Linear", [m]() { return m->_frequencyPlot == AnalyzerBase::LINEAR_FP; }, [m]() { m->_frequencyPlot = AnalyzerBase::LINEAR_FP; }));
	OptionsMenuItem::addToMenu(mi, menu);
}

void AnalyzerBaseWidget::addFrequencyRangeContextMenu(Menu* menu) {
	auto m = dynamic_cast<AnalyzerBase*>(module);
	assert(m);

	OptionsMenuItem* mi = new OptionsMenuItem("Frequency range");
	mi->addItem(OptionMenuItem("Lower 10%", [m]() { return m->_range == -0.9f; }, [m]() { m->_range = -0.9f; }));
	mi->addItem(OptionMenuItem("Lower 25%", [m]() { return m->_range == -0.75f; }, [m]() { m->_range = -0.75f; }));
	mi->addItem(OptionMenuItem("Lower 50%", [m]() { return m->_range == -0.5f; }, [m]() { m->_range = -0.5f; }));
	mi->addItem(OptionMenuItem("Full", [m]() { return m->_range == 0.0f; }, [m]() { m->_range = 0.0f; }));
	mi->addItem(OptionMenuItem("Upper 50%", [m]() { return m->_range == 0.5f; }, [m]() { m->_range = 0.5f; }));
	mi->addItem(OptionMenuItem("Upper 25%", [m]() { return m->_range == 0.75f; }, [m]() { m->_range = 0.75f; }));
	OptionsMenuItem::addToMenu(mi, menu);
}

void AnalyzerBaseWidget::addAmplitudePlotContextMenu(Menu* menu, bool linearOption) {
	auto m = dynamic_cast<AnalyzerBase*>(module);
	assert(m);

	OptionsMenuItem* mi = new OptionsMenuItem("Amplitude plot");
	mi->addItem(OptionMenuItem("Decibels to -60dB", [m]() { return m->_amplitudePlot == AnalyzerBase::DECIBELS_80_AP; }, [m]() { m->_amplitudePlot = AnalyzerBase::DECIBELS_80_AP; }));
	mi->addItem(OptionMenuItem("Decibels To -120dB", [m]() { return m->_amplitudePlot == AnalyzerBase::DECIBELS_140_AP; }, [m]() { m->_amplitudePlot = AnalyzerBase::DECIBELS_140_AP; }));
	if (linearOption) {
		mi->addItem(OptionMenuItem("Linear percentage", [m]() { return m->_amplitudePlot == AnalyzerBase::PERCENTAGE_AP; }, [m]() { m->_amplitudePlot = AnalyzerBase::PERCENTAGE_AP; }));
	}
	OptionsMenuItem::addToMenu(mi, menu);
}


void AnalyzerDisplay::setChannelBinsReader(int channel, BinsReader* br) {
	assert(_channelBinsReaders);
	assert(_module);
	assert(channel < _module->_core._nChannels);
	if (_channelBinsReaders[channel]) {
		delete _channelBinsReaders[channel];
	}
	_channelBinsReaders[channel] = br; // br now owned here.
}

void AnalyzerDisplay::displayChannel(int channel, bool display) {
	assert(_displayChannel);
	assert(_module);
	assert(channel < _module->_core._nChannels);
	_displayChannel[channel] = display;
}

void AnalyzerDisplay::draw(const DrawArgs& args) {
	if (_module) {
		_module->_core._channelsMutex.lock();
	}

	FrequencyPlot frequencyPlot = LOG_FP;
	AmplitudePlot amplitudePlot = DECIBELS_80_AP;
	float rangeMinHz = 0.0f;
	float rangeMaxHz = 0.0f;
	if (_module) {
		frequencyPlot = _module->_frequencyPlot;
		amplitudePlot = _module->_amplitudePlot;
		rangeMinHz = _module->_rangeMinHz;
		rangeMaxHz = _module->_rangeMaxHz;
	}
	else {
		rangeMaxHz = 0.5f * APP->engine->getSampleRate();
	}

	drawBackground(args);

	float strokeWidth = std::max(1.0f, 3.0f - getZoom());
	if (frequencyPlot == LINEAR_FP) {
		_xAxisLogFactor = 1.0f;
	}
	else {
		_xAxisLogFactor = (rangeMaxHz - rangeMinHz) / rangeMaxHz;
		_xAxisLogFactor *= 1.0f - baseXAxisLogFactor;
		_xAxisLogFactor = 1.0f - _xAxisLogFactor;
	}

	nvgSave(args.vg);
	nvgScissor(args.vg, _insetAround, _insetAround, _size.x - _insetAround, _size.y - _insetAround);
	if (_module) {
		drawHeader(args);
	}
	drawYAxis(args, strokeWidth, amplitudePlot);
	drawXAxis(args, strokeWidth, frequencyPlot, rangeMinHz, rangeMaxHz);
	if (_module) {
		for (int i = 0; i < _module->_core._nChannels; ++i) {
			if (_displayChannel[i]) {
				if (_module->_core._channels[i]) {
					GenericBinsReader br(_module, i);
					drawGraph(args, br, _channelColors[i % channelColorsN], strokeWidth, frequencyPlot, rangeMinHz, rangeMaxHz, amplitudePlot);
				}
				else if (_channelBinsReaders[i]) {
					drawGraph(args, *_channelBinsReaders[i], _channelColors[i % channelColorsN], strokeWidth, frequencyPlot, rangeMinHz, rangeMaxHz, amplitudePlot);
				}
			}
		}
	}
	nvgRestore(args.vg);

	if (_module) {
		_module->_core._channelsMutex.unlock();
	}
}

void AnalyzerDisplay::drawBackground(const DrawArgs& args) {
	nvgSave(args.vg);
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0, 0, _size.x, _size.y);
	nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
	nvgFill(args.vg);
	if (_drawInset) {
		nvgStrokeColor(args.vg, nvgRGBA(0x50, 0x50, 0x50, 0xff));
		nvgStroke(args.vg);
	}
	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawHeader(const DrawArgs& args) {
	nvgSave(args.vg);

	const int textY = -4;
	const int charPx = 5;
	const int sLen = 100;
	char s[sLen];
	int x = _insetAround + 2;

	int n = snprintf(s, sLen, "Peaks (+/-%0.1f):", (APP->engine->getSampleRate() / 2.0f) / (float)(_module->_core._size / _module->_core._binAverageN));
	drawText(args, s, x, _insetTop + textY);
	x += n * charPx - 0;

	int spacing = 3;
	if (_size.x > 300) {
		x += 5;
		spacing = 20;
	}
	for (int i = 0; i < _module->_core._nChannels; ++i) {
		if (_module->_core._channels[i]) {
			snprintf(s, sLen, "%c:%7.1f", 'A' + i, _module->_core.getPeak(i));
			drawText(args, s, x, _insetTop + textY, 0.0, &_channelColors[i % channelColorsN]);
		}
		x += 9 * charPx + spacing;
	}

	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawYAxis(const DrawArgs& args, float strokeWidth, AmplitudePlot plot) {
	nvgSave(args.vg);
	nvgStrokeColor(args.vg, _axisColor);
	nvgStrokeWidth(args.vg, strokeWidth);
	const int lineX = _insetLeft - 2;
	const int textX = 9;
	const float textR = -M_PI/2.0;

	nvgBeginPath(args.vg);
	int lineY = _insetTop;
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);

	switch (plot) {
		case DECIBELS_80_AP:
		case DECIBELS_140_AP: {
			float rangeDb = plot == DECIBELS_140_AP ? 140.0 : 80.0;
			auto line = [&](float db, float sw, const char* label, float labelOffset) {
				nvgBeginPath(args.vg);
				int y = _insetTop + (_graphSize.y - _graphSize.y*(rangeDb - _positiveDisplayDB + db)/rangeDb);
				nvgMoveTo(args.vg, lineX, y);
				nvgLineTo(args.vg, _size.x - _insetRight, y);
				nvgStrokeWidth(args.vg, strokeWidth * sw);
				nvgStroke(args.vg);
				drawText(args, label, textX, y + labelOffset, textR);
			};
			line(12.0, 1.0, "12", 5.0);
			line(0.0, 2.0, "0", 2.3);
			line(-12.0, 1.0, "-12", 10.0);
			line(-24.0, 1.0, "-24", 10.0);
			line(-48.0, 1.0, "-48", 10.0);
			if (rangeDb > 100.0) {
				line(-96.0, 1.0, "-96", 10.0);
			}
			drawText(args, "dB", textX, _size.y - _insetBottom, textR);
			break;
		}

		case PERCENTAGE_AP: {
			auto line = [&](float pct, float sw, const char* label, float labelOffset) {
				nvgBeginPath(args.vg);
				int y = _insetTop + (_graphSize.y - _graphSize.y*(pct / (100.0 * _totalLinearAmplitude)));
				nvgMoveTo(args.vg, lineX, y);
				nvgLineTo(args.vg, _size.x - _insetRight, y);
				nvgStrokeWidth(args.vg, strokeWidth * sw);
				nvgStroke(args.vg);
				drawText(args, label, textX, y + labelOffset, textR);
			};
			line(180.0, 1.0, "180", 8.0);
			line(160.0, 1.0, "160", 8.0);
			line(140.0, 1.0, "140", 8.0);
			line(120.0, 1.0, "120", 8.0);
			line(100.0, 2.0, "100", 8.0);
			line(80.0, 1.0, "80", 5.0);
			line(60.0, 1.0, "60", 5.0);
			line(40.0, 1.0, "40", 5.0);
			line(20.0, 1.0, "20", 5.0);
			drawText(args, "%", textX, _size.y - _insetBottom, textR);
			break;
		}
	}

	nvgBeginPath(args.vg);
	lineY = _insetTop + _graphSize.y + 1;
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, lineX, _insetTop);
	nvgLineTo(args.vg, lineX, lineY);
	nvgStroke(args.vg);

	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawXAxis(const DrawArgs& args, float strokeWidth, FrequencyPlot plot, float rangeMinHz, float rangeMaxHz) {
	nvgSave(args.vg);
	nvgStrokeColor(args.vg, _axisColor);
	nvgStrokeWidth(args.vg, strokeWidth);

	switch (plot) {
		case LOG_FP: {
			float hz = 100.0f;
			while (hz < rangeMaxHz && hz < 1001.0) {
				if (hz >= rangeMinHz) {
					drawXAxisLine(args, hz, rangeMinHz, rangeMaxHz);
				}
				hz += 100.0;
			}
			hz = 2000.0;
			while (hz < rangeMaxHz && hz < 10001.0) {
				if (hz >= rangeMinHz) {
					drawXAxisLine(args, hz, rangeMinHz, rangeMaxHz);
				}
				hz += 1000.0;
			}
			hz = 20000.0;
			while (hz < rangeMaxHz && hz < 100001.0) {
				if (hz >= rangeMinHz) {
					drawXAxisLine(args, hz, rangeMinHz, rangeMaxHz);
				}
				hz += 10000.0;
			}
			hz = 200000.0;
			while (hz < rangeMaxHz && hz < 1000001.0) {
				if (hz >= rangeMinHz) {
					drawXAxisLine(args, hz, rangeMinHz, rangeMaxHz);
				}
				hz += 100000.0;
			}

			drawText(args, " Hz", _insetLeft, _size.y - 2);
			if (rangeMinHz <= 100.0f) {
				float x = (100.0 - rangeMinHz) / (rangeMaxHz - rangeMinHz);
				x = powf(x, _xAxisLogFactor);
				if (x < 1.0) {
					x *= _graphSize.x;
					drawText(args, "100", _insetLeft + x - 8, _size.y - 2);
				}
			}
			if (rangeMinHz <= 1000.0f) {
				float x = (1000.0 - rangeMinHz) / (rangeMaxHz - rangeMinHz);
				x = powf(x, _xAxisLogFactor);
				if (x < 1.0) {
					x *= _graphSize.x;
					drawText(args, "1k", _insetLeft + x - 4, _size.y - 2);
				}
			}
			if (rangeMinHz <= 10000.0f) {
				float x = (10000.0 - rangeMinHz) / (rangeMaxHz - rangeMinHz);
				x = powf(x, _xAxisLogFactor);
				if (x < 1.0) {
					x *= _graphSize.x;
					drawText(args, "10k", _insetLeft + x - 7, _size.y - 2);
				}
			}
			if (rangeMinHz <= 100000.0f) {
				float x = (100000.0 - rangeMinHz) / (rangeMaxHz - rangeMinHz);
				x = powf(x, _xAxisLogFactor);
				if (x < 1.0) {
					x *= _graphSize.x;
					drawText(args, "100k", _insetLeft + x - 9, _size.y - 2);
				}
			}

			if (rangeMinHz > 10000.0f) {
				hz = 200000.0f;
				float lastX = 0.0f;
				while (hz < rangeMaxHz) {
					if (rangeMinHz <= hz) {
						float x = (hz - rangeMinHz) / (rangeMaxHz - rangeMinHz);
						x = powf(x, _xAxisLogFactor);
						if (x > lastX + 0.1f && x < 1.0f) {
							lastX = x;
							x *= _graphSize.x;
							const int sLen = 32;
							char s[sLen];
							snprintf(s, sLen, "%dk", (int)(hz / 1000.0f));
							drawText(args, s, _insetLeft + x - 7, _size.y - 2);
						}
					}
					hz += 100000.0f;
				}
			}
			else if (rangeMinHz > 1000.0f) {
				hz = 20000.0f;
				float lastX = 0.0f;
				while (hz < 80000.0f) {
					if (rangeMinHz <= hz) {
						float x = (hz - rangeMinHz) / (rangeMaxHz - rangeMinHz);
						x = powf(x, _xAxisLogFactor);
						if (x > lastX + 0.1f && x < 1.0f) {
							lastX = x;
							x *= _graphSize.x;
							const int sLen = 32;
							char s[sLen];
							snprintf(s, sLen, "%dk", (int)(hz / 1000.0f));
							drawText(args, s, _insetLeft + x - 7, _size.y - 2);
						}
					}
					hz += 10000.0f;
				}
			}
			break;
		}

		case LINEAR_FP: {
			float range = rangeMaxHz - rangeMinHz;
			float spacing = 100.0f;
			if (range > 100000.0f) {
				spacing = 10000.0f;
			}
			else if (range > 25000.0f) {
				spacing = 5000.0f;
			}
			else if (range > 10000.0f) {
				spacing = 1000.0f;
			}
			else if (range > 2500.0f) {
				spacing = 500.0f;
			}
			const char* suffix = "";
			float divisor = 1.0f;
			if (spacing > 100.f) {
				suffix = "k";
				divisor = 1000.0f;
			}

			drawText(args, "Hz", _insetLeft, _size.y - 2);
			float hz = 0.0f;
			float lastX = 0.0f;
			float xMin = 0.1f;
			if (_graphSize.x > 400) {
				xMin = 0.05f;
			}
			while (hz < rangeMaxHz) {
				if (hz > rangeMinHz) {
					drawXAxisLine(args, hz, rangeMinHz, rangeMaxHz);

					float x = (hz - rangeMinHz) / (rangeMaxHz - rangeMinHz);
					if (x > lastX + xMin && x < 0.99f) {
						lastX = x;
						x *= _graphSize.x;
						const int sLen = 32;
						char s[sLen];
						float dhz = hz / divisor;
						int n = 0;
						if (dhz - (int)dhz < 0.0001f) {
							n = snprintf(s, sLen, "%d%s", (int)dhz, suffix);
						}
						else {
							n = snprintf(s, sLen, "%0.1f%s", dhz, suffix);
						}
						drawText(args, s, _insetLeft + x - (n <= 2 ? 5 : 8), _size.y - 2);
					}
				}
				hz += spacing;
			}
			break;
		}
	}

	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawXAxisLine(const DrawArgs& args, float hz, float rangeMinHz, float rangeMaxHz) {
	float x = (hz - rangeMinHz) / (rangeMaxHz - rangeMinHz);
	x = powf(x, _xAxisLogFactor);
	if (x < 1.0) {
		x *= _graphSize.x;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, _insetLeft + x, _insetTop);
		nvgLineTo(args.vg, _insetLeft + x, _insetTop + _graphSize.y);
		nvgStroke(args.vg);
	}
}

void AnalyzerDisplay::drawGraph(
	const DrawArgs& args,
	BinsReader& bins,
	NVGcolor color,
	float strokeWidth,
	FrequencyPlot freqPlot,
	float rangeMinHz,
	float rangeMaxHz,
	AmplitudePlot ampPlot
) {
	float range = (rangeMaxHz - rangeMinHz) / (0.5f * APP->engine->getSampleRate());
	int pointsN = roundf(range * (_module->_core._size / 2));
	range = rangeMinHz / (0.5f * APP->engine->getSampleRate());
	int pointsOffset = roundf(range * (_module->_core._size / 2));
	nvgSave(args.vg);
	nvgScissor(args.vg, _insetLeft, _insetTop, _graphSize.x, _graphSize.y);
	nvgStrokeColor(args.vg, color);
	nvgStrokeWidth(args.vg, strokeWidth);
	nvgBeginPath(args.vg);
	for (int i = 0; i < pointsN; ++i) {
		int height = binValueToHeight(bins.at(pointsOffset + i), ampPlot);
		if (i == 0) {
			nvgMoveTo(args.vg, _insetLeft, _insetTop + (_graphSize.y - height));
		}
		else {
			float x = _graphSize.x * powf(i / (float)pointsN, _xAxisLogFactor);
			nvgLineTo(args.vg, _insetLeft + x, _insetTop + (_graphSize.y - height));
		}
	}
	nvgStroke(args.vg);
	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawText(const DrawArgs& args, const char* s, float x, float y, float rotation, const NVGcolor* color) {
	nvgSave(args.vg);
	nvgTranslate(args.vg, x, y);
	nvgRotate(args.vg, rotation);
	nvgFontSize(args.vg, 10);
	nvgFontFaceId(args.vg, _font->handle);
	nvgFillColor(args.vg, color ? *color : _textColor);
	nvgText(args.vg, 0, 0, s, NULL);
	nvgRestore(args.vg);
}

int AnalyzerDisplay::binValueToHeight(float value, AmplitudePlot plot) {
	switch (plot) {
		case DECIBELS_80_AP:
		case DECIBELS_140_AP: {
			float rangeDb = plot == DECIBELS_140_AP ? 140.0 : 80.0;
			const float minDB = -(rangeDb - _positiveDisplayDB);
			value = binValueToDb(value);
			value = std::max(minDB, value);
			value = std::min(_positiveDisplayDB, value);
			value -= minDB;
			value /= rangeDb;
			return roundf(_graphSize.y * value);
		}

		case PERCENTAGE_AP: {
			value = binValueToAmplitude(value);
			value = std::min(value, 2.0f);
			value = std::max(value, 0.0f);
			return roundf(_graphSize.y * value / _totalLinearAmplitude);
		}
	}
}

float AnalyzerDisplay::binValueToAmplitude(float value) {
	value /= 10.0f; // arbitrarily use 5.0f as reference "maximum" baseline signal (e.g. raw output of an oscillator)...but signals are +/-5, so 10 total.
	return powf(value, 0.5f); // undoing magnitude scaling of levels back from the FFT?
}

float AnalyzerDisplay::binValueToDb(float value) {
	return amplitudeToDecibels(binValueToAmplitude(value));
}

float AnalyzerDisplay::dbToBinValue(float db) {
	db = decibelsToAmplitude(db);
	db *= db;
	return db * 10.0f;
}
