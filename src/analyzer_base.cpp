
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
	if (APP->engine->getSampleRate() < 96000.0f) {
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

void AnalyzerCore::stepChannel(int channelIndex, Input& input) {
	assert(channelIndex >= 0);
	assert(channelIndex < _nChannels);

	if (input.isConnected()) {
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
		_channels[channelIndex]->step(input.getVoltage());
	}
	else if (_channels[channelIndex]) {
		std::lock_guard<std::mutex> lock(_channelsMutex);
		delete _channels[channelIndex];
		_channels[channelIndex] = NULL;
	}
}


void AnalyzerDisplay::draw(const DrawArgs& args) {
	float rangeMinHz = 0.0f;
	float rangeMaxHz = 0.0f;
	float rangeDb = 80.0f;
	if (_module) {
		rangeMinHz = _module->_rangeMinHz;
		rangeMaxHz = _module->_rangeMaxHz;
		rangeDb = _module->_rangeDb;
	}
	else {
		rangeMaxHz = 0.5f * APP->engine->getSampleRate();
	}

	drawBackground(args);

	float strokeWidth = std::max(1.0f, 3.0f - getZoom());
	_xAxisLogFactor = (rangeMaxHz - rangeMinHz) / rangeMaxHz;
	_xAxisLogFactor *= 1.0f - baseXAxisLogFactor;
	_xAxisLogFactor = 1.0f - _xAxisLogFactor;

	nvgSave(args.vg);
	nvgScissor(args.vg, _insetAround, _insetAround, _size.x - _insetAround, _size.y - _insetAround);
	if (_module) {
		drawHeader(args);
	}
	drawYAxis(args, strokeWidth, rangeDb);
	drawXAxis(args, strokeWidth, rangeMinHz, rangeMaxHz);
	if (_module) {
		for (int i = 0; i < _module->_core._nChannels; ++i) {
			if (_module->_core._channels[i]) {
				drawGraph(args, _module->_core.getBins(i), _module->_core._binsN, _channelColors[i % channelColorsN], strokeWidth, rangeMinHz, rangeMaxHz, rangeDb);
			}
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

void AnalyzerDisplay::drawYAxis(const DrawArgs& args, float strokeWidth, float rangeDb) {
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
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(rangeDb - _positiveDisplayDB + 12.0)/rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);
	drawText(args, "12", textX, lineY + 5.0, textR);

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(rangeDb - _positiveDisplayDB)/rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStrokeWidth(args.vg, strokeWidth * 1.5);
	nvgStroke(args.vg);
	nvgStrokeWidth(args.vg, strokeWidth);
	drawText(args, "0", textX, lineY + 2.3, textR);

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(rangeDb - _positiveDisplayDB - 12.0)/rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);
	drawText(args, "-12", textX, lineY + 10, textR);

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(rangeDb - _positiveDisplayDB - 24.0)/rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);
	drawText(args, "-24", textX, lineY + 10, textR);

	nvgBeginPath(args.vg);
	lineY = _insetTop + (_graphSize.y - _graphSize.y*(rangeDb - _positiveDisplayDB - 48.0)/rangeDb);
	nvgMoveTo(args.vg, lineX, lineY);
	nvgLineTo(args.vg, _size.x - _insetRight, lineY);
	nvgStroke(args.vg);
	drawText(args, "-48", textX, lineY + 10, textR);

	if (rangeDb > 100.0) {
		nvgBeginPath(args.vg);
		lineY = _insetTop + (_graphSize.y - _graphSize.y*(rangeDb - _positiveDisplayDB - 96.0)/rangeDb);
		nvgMoveTo(args.vg, lineX, lineY);
		nvgLineTo(args.vg, _size.x - _insetRight, lineY);
		nvgStroke(args.vg);
		drawText(args, "-96", textX, lineY + 10, textR);
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

	drawText(args, "dB", textX, _size.y - _insetBottom, textR);

	nvgRestore(args.vg);
}

void AnalyzerDisplay::drawXAxis(const DrawArgs& args, float strokeWidth, float rangeMinHz, float rangeMaxHz) {
	nvgSave(args.vg);
	nvgStrokeColor(args.vg, _axisColor);
	nvgStrokeWidth(args.vg, strokeWidth);

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
	const float* bins,
	int binsN,
	NVGcolor color,
	float strokeWidth,
	float rangeMinHz,
	float rangeMaxHz,
	float rangeDb
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
		int height = binValueToHeight(bins[pointsOffset + i], rangeDb);
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

int AnalyzerDisplay::binValueToHeight(float value, float rangeDb) {
	const float minDB = -(rangeDb - _positiveDisplayDB);
	value /= 10.0f; // arbitrarily use 5.0f as reference "maximum" baseline signal (e.g. raw output of an oscillator)...but signals are +/-5, so 10 total.
	value = powf(value, 0.5f); // undoing magnitude scaling of levels back from the FFT?
	value = amplitudeToDecibels(value);
	value = std::max(minDB, value);
	value = std::min(_positiveDisplayDB, value);
	value -= minDB;
	value /= rangeDb;
	return roundf(_graphSize.y * value);
}
