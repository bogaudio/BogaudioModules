
#include "parametric_eq.hpp"
#include "dsp/pitch.hpp"

const float PEQChannel::maxDecibels = 6.0f;
const float PEQChannel::minDecibels = Amplifier::minDecibels;
constexpr float PEQChannel::maxFrequency;
constexpr float PEQChannel::minFrequency;
const float PEQChannel::maxFrequencySemitone = frequencyToSemitone(PEQChannel::maxFrequency);
const float PEQChannel::minFrequencySemitone = frequencyToSemitone(PEQChannel::minFrequency);

void PEQChannel::setSampleRate(float sampleRate) {
	_sampleRate = sampleRate;
	_levelSL.setParams(sampleRate, 0.05f, maxDecibels - minDecibels);
	_frequencySL.setParams(sampleRate, 0.5f, frequencyToSemitone(maxFrequency - minFrequency));
	_bandwidthSL.setParams(sampleRate, 0.05f, MultimodeFilter::maxQbw - MultimodeFilter::minQbw);
	_rms.setSampleRate(sampleRate);
}

void PEQChannel::setFilterMode(MultimodeFilter::Mode mode) {
	_mode = mode;
	_poles = _mode == MultimodeFilter::BANDPASS_MODE ? 4 : 12;
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
		float cv = clamp(_frequency2Input.getPolyVoltage(_c) / 5.0f, -1.0f, 1.0f);
		if (_frequencyCv2Param) {
			cv *= _frequencyCv2Param->getValue();
		}
		fcv += cv;
	}
	fcv *= _frequencyCv1Param.getValue();
	if (_fullFrequencyMode) {
		fcv *= maxFrequencySemitone - minFrequencySemitone;
	}
	else {
		fcv *= 12.0f;
	}

	frequency = _frequencyParam.getValue();
	frequency *= frequency;
	frequency *= maxFrequency;
	frequency = clamp(frequency, minFrequency, maxFrequency);
	frequency = frequencyToSemitone(frequency);
	frequency += fcv;
	frequency = clamp(frequency, minFrequencySemitone, maxFrequencySemitone);
	frequency = semitoneToFrequency(_frequencySL.next(frequency));

	bandwidth = MultimodeFilter::minQbw;
	if (_mode == MultimodeFilter::BANDPASS_MODE) {
		bandwidth = clamp(_bandwidthParam.getValue(), 0.0f, 1.0f);
		if (_bandwidthInput && _bandwidthInput->isConnected()) {
			bandwidth *= clamp(_bandwidthInput->getPolyVoltage(_c) / 10.0f, 0.0f, 1.0f);
		}
		bandwidth = MultimodeFilter::minQbw + bandwidth * (MultimodeFilter::maxQbw - MultimodeFilter::minQbw);
	}
	_filter->setParams(
		_sampleRate,
		MultimodeFilter::BUTTERWORTH_TYPE,
		_poles,
		_mode,
		frequency,
		bandwidth,
		MultimodeFilter::PITCH_BANDWIDTH_MODE
	);
}

void PEQChannel::next(float sample) {
	out = _amplifier.next(_filter->next(sample));
	rms = _rms.next(out / 5.0f);
}


 void PEQEngine::setFrequencyMode(bool full) {
	 for (int i = 0; i < _n; ++i) {
		 _channels[i]->setFrequencyMode(full);
	 }
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
	bandwidth = _channels[1]->bandwidth; // take from any bandpass-only channel.
	float out = 0.0f;
	for (int i = 0; i < _n; ++i) {
		PEQChannel& c = *_channels[i];
		c.next(sample);
		out += outs[i] = c.out;
		frequencies[i] = c.frequency;
		rmsSums[i] += c.rms;
	}
	return _saturator.next(out);
}


float PEQXFBase::scaleEF(float ef, float frequency, float bandwidth) {
	bandwidth = (bandwidth - MultimodeFilter::minQbw) / (MultimodeFilter::maxQbw - MultimodeFilter::minQbw);
	bandwidth *= MultimodeFilter::maxBWPitch;
	float minf = std::max(0.0f, powf(2.0f, -bandwidth) * frequency);
	float maxf = std::min(PEQChannel::maxFrequency, powf(2.0f, bandwidth) * frequency);
	float scale = (2.0f * (maxf - minf)) / PEQChannel::maxFrequency;
	scale = 1.0f / scale;
	scale = sqrtf(scale); // FIXME: someday prove this is correct.
	return 2.0f * scale * ef;
}


#define BAND_EXCLUDE "band_exclude"

json_t* BandExcludeModule::toJson(json_t* root) {
	json_object_set_new(root, BAND_EXCLUDE, json_boolean(_bandExclude));
	return root;
}

void BandExcludeModule::fromJson(json_t* root) {
	json_t* be = json_object_get(root, BAND_EXCLUDE);
	if (be) {
		_bandExclude = json_is_true(be);
	}
}


void BandExcludeModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<BandExcludeModule*>(module);
	assert(m);
	menu->addChild(new BoolOptionMenuItem("Exclude direct-output bands from mix", [m]() { return &m->_bandExclude; }));
}
