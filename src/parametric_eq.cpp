#include "parametric_eq.hpp"
#include "dsp/pitch.hpp"

const float PEQChannel::maxDecibels = 6.0f;
const float PEQChannel::minDecibels = Amplifier::minDecibels;
constexpr float PEQChannel::maxFrequency;
constexpr float PEQChannel::minFrequency;

void PEQChannel::setSampleRate(float sampleRate) {
	_sampleRate = sampleRate;
	_levelSL.setParams(sampleRate, 0.05f, maxDecibels - minDecibels);
	_frequencySL.setParams(sampleRate, 0.5f, frequencyToSemitone(maxFrequency - minFrequency));
	_bandwidthSL.setParams(sampleRate, 0.05f, MultimodeFilter::maxQbw - MultimodeFilter::minQbw);
	_rms.setSampleRate(sampleRate);
}

void PEQChannel::setFilterMode(MultimodeFilter::Mode mode) {
	_mode = mode;
	_poles = _mode == MultimodeFilter::BANDPASS_MODE ? 2 : 4;
}

void PEQChannel::modulate() {
	float level = clamp(_levelParam.getValue(), 0.0f, 1.0f);
	if (_levelInput.isConnected()) {
		level *= clamp(_levelInput.getPolyVoltage(_c) / 10.0f, 0.0f, 1.0f);
	}
	level *= maxDecibels - minDecibels;
	level += minDecibels;
	_amplifier.setLevel(_levelSL.next(level));

	float fcv = 0.0f;
	if (_frequency1Input.isConnected()) {
		fcv += clamp(_frequency1Input.getPolyVoltage(_c) / 5.0f, -1.0f, 1.0f);
	}
	if (_frequency2Input.isConnected()) {
		fcv += clamp(_frequency2Input.getPolyVoltage(_c) / 5.0f, -1.0f, 1.0f);
	}
	fcv *= _frequencyCvParam.getValue();
	float f = _frequencyParam.getValue();
	f += fcv;
	f *= f;
	f *= maxFrequency;
	f = clamp(f, minFrequency, maxFrequency);
	f = semitoneToFrequency(_frequencySL.next(frequencyToSemitone(f)));

	float bw = MultimodeFilter::minQbw;
	if (_mode == MultimodeFilter::BANDPASS_MODE) {
		bw = clamp(_bandwidthParam.getValue(), 0.0f, 1.0f);
		if (_bandwidthInput && _bandwidthInput->isConnected()) {
			bw *= clamp(_bandwidthInput->getPolyVoltage(_c) / 10.0f, 0.0f, 1.0f);
		}
	}
	_filter.setParams(
		_sampleRate,
		MultimodeFilter::BUTTERWORTH_TYPE,
		_poles,
		_mode,
		f,
		bw,
		MultimodeFilter::PITCH_BANDWIDTH_MODE
	);
}

void PEQChannel::next(float sample) {
	out = _amplifier.next(_filter.next(sample));
	rms = _rms.next(out / 5.0f);
}


void PEQEngine::setSampleRate(float sr) {
	for (int i = 0; i < _n; ++i) {
		_channels[i]->setSampleRate(sr);
	}
}

void PEQEngine::modulate() {
	for (int i = 0; i < _n; ++i) {
		_channels[i]->modulate();
	}
}

float PEQEngine::next(float sample, float* rmsSums) {
	float out = 0.0f;
	for (int i = 0; i < _n; ++i) {
		PEQChannel& c = *_channels[i];
		c.next(sample);
		out += c.out;
		rmsSums[i] += c.rms;
	}
	return _saturator.next(out);
}
