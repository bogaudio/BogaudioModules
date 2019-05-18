
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
	delete[] _bins0;
	delete[] _bins1;
	if (_averagedBins) {
		delete _averagedBins;
	}
}

float ChannelAnalyzer::getPeak() {
	float max = 0.0f;
	int maxBin = 0;
	const float* bins = getBins();
	for (int bin = 0; bin < _binsN; ++bin) {
		if (bins[bin] > max) {
			max = bins[bin];
			maxBin = bin;
		}
	}

	const int bandsPerBin = _analyzer._size / _binsN;
	const float fWidth = (_analyzer._sampleRate / 2.0f) / (float)(_analyzer._size / bandsPerBin);
	float sum = 0.0f;
	float sumWeights = 0.0f;
	int i = std::max(0, maxBin - 1);
	int j = std::max(_binsN - 1, maxBin + 1);
	for (; i <= j; ++i) {
		sum += bins[i] * fWidth * i;
		sumWeights += bins[i];
	}
	return sum / sumWeights;
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
	std::lock_guard<std::mutex> lock(_channelsMutex);
	for (int i = 0; i < _nChannels; ++i) {
		if (_channels[i]) {
			delete _channels[i];
			_channels[i] = NULL;
		}
	}
}

SpectrumAnalyzer::Size AnalyzerCore::size() {
	if (engineGetSampleRate() < 96000.0f) {
		switch (_quality) {
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

void AnalyzerCore::stepChannel(int channelIndex, Input& input) {
	assert(channelIndex >= 0);
	assert(channelIndex < _nChannels);

	if (input.active) {
		if (!_channels[channelIndex]) {
			std::lock_guard<std::mutex> lock(_channelsMutex);
			_channels[channelIndex] = new ChannelAnalyzer(
				size(),
				_overlap,
				window(),
				engineGetSampleRate(),
				_averageN,
				_binAverageN
			);
		}
		_channels[channelIndex]->step(input.value);
	}
	else if (_channels[channelIndex]) {
		std::lock_guard<std::mutex> lock(_channelsMutex);
		delete _channels[channelIndex];
		_channels[channelIndex] = NULL;
	}
}


void AnalyzerDisplay::draw(const DrawArgs& args) {
	// FIXME.v1
	if (!_module) {
		return;
	}

	drawBackground(args.vg);
	float strokeWidth = 2.0f; // FIXME.v1 std::max(1.0f, 3 - gRackScene->zoomWidget->zoom);
	_xAxisLogFactor = (_module->_rangeMaxHz - _module->_rangeMinHz) / _module->_rangeMaxHz;
	_xAxisLogFactor *= 1.0f - baseXAxisLogFactor;
	_xAxisLogFactor = 1.0f - _xAxisLogFactor;

	nvgSave(args.vg);
	nvgScissor(args.vg, _insetAround, _insetAround, _size.x - _insetAround, _size.y - _insetAround);
	drawHeader(args.vg);
	drawYAxis(args.vg, strokeWidth);
	drawXAxis(args.vg, strokeWidth);
	for (int i = 0; i < _module->_core._nChannels; ++i) {
		ChannelAnalyzer* channel = _module->_core._channels[i];
		if (channel) {
			drawGraph(args.vg, channel->getBins(), channel->_binsN, _channelColors[i % channelColorsN], strokeWidth);
		}
	}
	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawBackground(const DrawArgs& args) {
	nvgSave(args.vg);
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0, 0, _size.x, _size.y);
	nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
	nvgFill(args.vg);
	if (_drawInset) {
		nvgStrokeColor(args.vg, nvgRGBA(0xc0, 0xc0, 0xc0, 0xff));
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

	int n = snprintf(s, sLen, "Peaks (+/-%0.1f):", (engineGetSampleRate() / 2.0f) / (float)(_module->_core.size() / _module->_core._binAverageN));
	drawText(args.vg, s, x, _insetTop + textY);
	x += n * charPx - 0;

	int spacing = 3;
	if (_size.x > 300) {
		x += 5;
		spacing = 20;
	}
	for (int i = 0; i < _module->_core._nChannels; ++i) {
		ChannelAnalyzer* channel = _module->_core._channels[i];
		if (channel) {
			snprintf(s, sLen, "%c:%7.1f", 'A' + i, channel->getPeak());
			drawText(args.vg, s, x, _insetTop + textY, 0.0, &_channelColors[i % channelColorsN]);
		}
		x += 9 * charPx + spacing;
	}

	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawYAxis(const DrawArgs& args, float strokeWidth) {
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

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(_module->_rangeDb - _positiveDisplayDB + 12.0)/_module->_rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);
	drawText(args.vg, "12", textX, lineY + 5.0, textR);

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(_module->_rangeDb - _positiveDisplayDB)/_module->_rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStrokeWidth(args.vg, strokeWidth * 1.5);
	nvgStroke(args.vg);
	nvgStrokeWidth(args.vg, strokeWidth);
	drawText(args.vg, "0", textX, lineY + 2.3, textR);

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(_module->_rangeDb - _positiveDisplayDB - 12.0)/_module->_rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);
	drawText(args.vg, "-12", textX, lineY + 10, textR);

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(_module->_rangeDb - _positiveDisplayDB - 24.0)/_module->_rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);
	drawText(args.vg, "-24", textX, lineY + 10, textR);

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(_module->_rangeDb - _positiveDisplayDB - 48.0)/_module->_rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);
	drawText(args.vg, "-48", textX, lineY + 10, textR);

	if (_module->_rangeDb > 100.0) {
		nvgBeginPath(args.vg);
		lineY = _insetTop + (_graphSize.y - _graphSize.y*(_module->_rangeDb - _positiveDisplayDB - 96.0)/_module->_rangeDb);
		nvgMoveTo(args.vg, lineX, lineY);
		nvgLineTo(args.vg, _size.x - _insetRight, lineY);
		nvgStroke(args.vg);
		drawText(args.vg, "-96", textX, lineY + 10, textR);
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

	drawText(args.vg, "dB", textX, _size.y - _insetBottom, textR);

	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawXAxis(const DrawArgs& args, float strokeWidth) {
	nvgSave(args.vg);
	nvgStrokeColor(args.vg, _axisColor);
	nvgStrokeWidth(args.vg, strokeWidth);

	float hz = 100.0f;
	while (hz < _module->_rangeMaxHz && hz < 1001.0) {
		if (hz >= _module->_rangeMinHz) {
			drawXAxisLine(args.vg, hz);
		}
		hz += 100.0;
	}
	hz = 2000.0;
	while (hz < _module->_rangeMaxHz && hz < 10001.0) {
		if (hz >= _module->_rangeMinHz) {
			drawXAxisLine(args.vg, hz);
		}
		hz += 1000.0;
	}
	hz = 20000.0;
	while (hz < _module->_rangeMaxHz && hz < 100001.0) {
		if (hz >= _module->_rangeMinHz) {
			drawXAxisLine(args.vg, hz);
		}
		hz += 10000.0;
	}

	drawText(args.vg, "Hz", _insetLeft, _size.y - 2);
	if (_module->_rangeMinHz <= 100.0f) {
		float x = (100.0 - _module->_rangeMinHz) / (_module->_rangeMaxHz - _module->_rangeMinHz);
		x = powf(x, _xAxisLogFactor);
		if (x < 1.0) {
			x *= _graphSize.x;
			drawText(args.vg, "100", _insetLeft + x - 8, _size.y - 2);
		}
	}
	if (_module->_rangeMinHz <= 1000.0f) {
		float x = (1000.0 - _module->_rangeMinHz) / (_module->_rangeMaxHz - _module->_rangeMinHz);
		x = powf(x, _xAxisLogFactor);
		if (x < 1.0) {
			x *= _graphSize.x;
			drawText(args.vg, "1k", _insetLeft + x - 4, _size.y - 2);
		}
	}
	if (_module->_rangeMinHz <= 10000.0f) {
		float x = (10000.0 - _module->_rangeMinHz) / (_module->_rangeMaxHz - _module->_rangeMinHz);
		x = powf(x, _xAxisLogFactor);
		if (x < 1.0) {
			x *= _graphSize.x;
			drawText(args.vg, "10k", _insetLeft + x - 4, _size.y - 2);
		}
	}
	if (_module->_rangeMinHz > 1000.0f) {
		hz = 20000.0f;
		float lastX = 0.0f;
		while (hz < _module->_rangeMaxHz) {
			if (_module->_rangeMinHz <= hz) {
				float x = (hz - _module->_rangeMinHz) / (_module->_rangeMaxHz - _module->_rangeMinHz);
				x = powf(x, _xAxisLogFactor);
				if (x > lastX + 0.075f && x < 1.0f) {
					lastX = x;
					x *= _graphSize.x;
					const int sLen = 32;
					char s[sLen];
					snprintf(s, sLen, "%dk", (int)(hz / 1000.0f));
					drawText(args.vg, s, _insetLeft + x - 7, _size.y - 2);
				}
			}
			hz += 10000.0f;
		}
	}

	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawXAxisLine(const DrawArgs& args, float hz) {
	float x = (hz - _module->_rangeMinHz) / (_module->_rangeMaxHz - _module->_rangeMinHz);
	x = powf(x, _xAxisLogFactor);
	if (x < 1.0) {
		x *= _graphSize.x;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, _insetLeft + x, _insetTop);
		nvgLineTo(args.vg, _insetLeft + x, _insetTop + _graphSize.y);
		nvgStroke(args.vg);
	}
}

void AnalyzerDisplay::drawGraph(const DrawArgs& args, const float* bins, int binsN, NVGcolor color, float strokeWidth) {
	float range = (_module->_rangeMaxHz - _module->_rangeMinHz) / (0.5f * engineGetSampleRate());
	int pointsN = roundf(range * (_module->_core.size() / 2));
	range = _module->_rangeMinHz / (0.5f * engineGetSampleRate());
	int pointsOffset = roundf(range * (_module->_core.size() / 2));
	nvgSave(args.vg);
	nvgScissor(args.vg, _insetLeft, _insetTop, _graphSize.x, _graphSize.y);
	nvgStrokeColor(args.vg, color);
	nvgStrokeWidth(args.vg, strokeWidth);
	nvgBeginPath(args.vg);
	for (int i = 0; i < pointsN; ++i) {
		int height = binValueToHeight(bins[pointsOffset + i]);
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

int AnalyzerDisplay::binValueToHeight(float value) {
	const float minDB = -(_module->_rangeDb - _positiveDisplayDB);
	value /= 10.0f; // arbitrarily use 5.0f as reference "maximum" baseline signal (e.g. raw output of an oscillator)...but signals are +/-5, so 10 total.
	value = powf(value, 0.5f); // undoing magnitude scaling of levels back from the FFT?
	value = amplitudeToDecibels(value);
	value = std::max(minDB, value);
	value = std::min(_positiveDisplayDB, value);
	value -= minDB;
	value /= _module->_rangeDb;
	return roundf(_graphSize.y * value);
}
