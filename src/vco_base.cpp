#include "vco_base.hpp"
#include "dsp/pitch.hpp"

#define POLY_INPUT "poly_input"

float VCOBase::VCOFrequencyParamQuantity::offset() {
	auto vco = dynamic_cast<VCOBase*>(module);
	return vco->_slowMode ? vco->slowModeOffset : 0.0f;
}

float VCOBase::VCOFrequencyParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	auto vco = dynamic_cast<VCOBase*>(module);
	return vco->_linearMode ? (vco->_slowMode ? v : v * 1000.0f) : FrequencyParamQuantity::getDisplayValue();
}

void VCOBase::VCOFrequencyParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}

	auto vco = dynamic_cast<VCOBase*>(module);
	if (vco->_linearMode) {
		if (vco->_slowMode) {
			setValue(v / 1000.0f);
		}
		else {
			setValue(v);
		}
	}
	else {
		FrequencyParamQuantity::setDisplayValue(v);
	}
}

void VCOBase::Engine::reset() {
	syncTrigger.reset();
}

void VCOBase::Engine::sampleRateChange(float sampleRate) {
	phasor.setSampleRate(sampleRate);
	square.setSampleRate(sampleRate);
	saw.setSampleRate(sampleRate);
	squareDecimator.setParams(sampleRate, oversample);
	sawDecimator.setParams(sampleRate, oversample);
	triangleDecimator.setParams(sampleRate, oversample);
	squarePulseWidthSL.setParams(sampleRate, 0.1f, 2.0f);
}

void VCOBase::Engine::setFrequency(float f) {
	if (frequency != f && f < 0.475f * phasor._sampleRate) {
		frequency = f;
		phasor.setFrequency(frequency / (float)oversample);
		square.setFrequency(frequency);
		saw.setFrequency(frequency);
	}
}

void VCOBase::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void VCOBase::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	_oversampleThreshold = 0.06f * sampleRate;

	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange(sampleRate);
	}
}

json_t* VCOBase::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, POLY_INPUT, json_integer(_polyInputID));
	return root;
}

void VCOBase::dataFromJson(json_t* root) {
	json_t* p = json_object_get(root, POLY_INPUT);
	if (p) {
		_polyInputID = json_integer_value(p);
	}
}

int VCOBase::channels() {
	return _polyInputID == _fmInputID ? inputs[_fmInputID].getChannels() : inputs[_pitchInputID].getChannels();
}

void VCOBase::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->sampleRateChange(APP->engine->getSampleRate());
	if (c > 0) {
		_engines[c]->phasor.syncPhase(_engines[0]->phasor);
	}
}

void VCOBase::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void VCOBase::modulateChannel(int c) {
	Engine& e = *_engines[c];

	e.baseVOct = params[_frequencyParamID].getValue();
	if (_fineFrequencyParamID >= 0) {
		e.baseVOct += params[_fineFrequencyParamID].getValue() / 12.0f;
	}
	if (inputs[_pitchInputID].isConnected()) {
		e.baseVOct += clamp(inputs[_pitchInputID].getVoltage(c), -5.0f, 5.0f);
	}
	if (_linearMode) {
		if (_slowMode) {
			e.baseHz = e.baseVOct;
		}
		else {
			e.baseHz = e.baseVOct * 1000.0f;
		}
	}
	else {
		if (_slowMode) {
			e.baseVOct += slowModeOffset;
		}
		e.baseHz = cvToFrequency(e.baseVOct);
	}
}

void VCOBase::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	if (e.syncTrigger.next(inputs[_syncInputID].getPolyVoltage(c))) {
		e.phasor.resetPhase();
	}

	float frequency = e.baseHz;
	Phasor::phase_delta_t phaseOffset = 0;
	if (_fmInputID >= 0 && inputs[_fmInputID].isConnected() && _fmDepth > 0.01f) {
		float fm = inputs[_fmInputID].getPolyVoltage(c) * _fmDepth;
		if (_fmLinearMode) {
			phaseOffset = Phasor::radiansToPhase(2.0f * fm);
		}
		else {
			frequency = cvToFrequency(e.baseVOct + fm);
		}
	}
	e.setFrequency(frequency);

	const float oversampleWidth = 100.0f;
	float mix, oMix;
	if (frequency > _oversampleThreshold) {
		if (frequency > _oversampleThreshold + oversampleWidth) {
			mix = 0.0f;
			oMix = 1.0f;
		}
		else {
			oMix = (frequency - _oversampleThreshold) / oversampleWidth;
			mix = 1.0f - oMix;
		}
	}
	else {
		mix = 1.0f;
		oMix = 0.0f;
	}

	e.squareOut = 0.0f;
	e.sawOut = 0.0f;
	e.triangleOut = 0.0f;
	if (oMix > 0.0f) {
		for (int i = 0; i < Engine::oversample; ++i) {
			e.phasor.advancePhase();
			if (e.squareActive) {
				e.squareBuffer[i] = e.square.nextFromPhasor(e.phasor, phaseOffset + e.additionalPhaseOffset);
			}
			if (e.sawActive) {
				e.sawBuffer[i] = e.saw.nextFromPhasor(e.phasor, phaseOffset + e.additionalPhaseOffset);
			}
			if (e.triangleActive) {
				e.triangleBuffer[i] = e.triangle.nextFromPhasor(e.phasor, phaseOffset + e.additionalPhaseOffset);
			}
		}
		if (e.squareActive) {
			e.squareOut += oMix * amplitude * e.squareDecimator.next(e.squareBuffer);
		}
		if (e.sawActive) {
			e.sawOut += oMix * amplitude * e.sawDecimator.next(e.sawBuffer);
		}
		if (e.triangleActive) {
			e.triangleOut += oMix * amplitude * e.triangleDecimator.next(e.triangleBuffer);
		}
	}
	else {
		e.phasor.advancePhase(Engine::oversample);
	}
	if (mix > 0.0f) {
		if (e.squareActive) {
			e.squareOut += mix * amplitude * e.square.nextFromPhasor(e.phasor, phaseOffset + e.additionalPhaseOffset);
		}
		if (e.sawActive) {
			e.sawOut += mix * amplitude * e.saw.nextFromPhasor(e.phasor, phaseOffset + e.additionalPhaseOffset);
		}
		if (e.triangleActive) {
			e.triangleOut += mix * amplitude * e.triangle.nextFromPhasor(e.phasor, phaseOffset + e.additionalPhaseOffset);
		}
	}

	e.sineOut = e.sineActive ? (amplitude * e.sine.nextFromPhasor(e.phasor, phaseOffset + e.additionalPhaseOffset)) : 0.0f;
}
