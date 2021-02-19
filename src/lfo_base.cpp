
#include "lfo_base.hpp"
#include "dsp/pitch.hpp"

void LFOBase::Smoother::setParams(float sampleRate, float frequency, float amount) {
	assert(sampleRate > 0.0f);
	assert(frequency > 0.0f);
	assert(amount >= 0.0f && amount <= 1.0f);
	if (_sampleRate != sampleRate || _frequency != frequency || _amount != amount) {
		_sampleRate = sampleRate;
		_frequency = frequency;
		_amount = amount;

		float millis = 1.0f / frequency;
		millis /= 2.0f;
		millis *= 1000.0f;
		millis *= amount*amount * 10.0f;
		_slewLimiter.setParams(_sampleRate, millis, 0.5f);
	}
}

float LFOBase::Smoother::next(float sample) {
	return _slewLimiter.next(sample);
}

float LFOBase::LFOFrequencyParamQuantity::offset() {
	auto lfo = dynamic_cast<LFOBase*>(module);
	return lfo->getPitchOffset();
}

float LFOBase::getPitchOffset() {
	float offset = -3.0f;
	if (_slowMode) {
		offset -= 8.0f;
	}
	else {
		offset -= 4.0f;
	}
	return offset;
}

void LFOBase::setFrequency(Param& frequency, Input& pitch, Phasor& phasor, int c) {
	float f = frequency.getValue();
	if (pitch.isConnected()) {
		f += pitch.getVoltage(c);
	}
	f += getPitchOffset();

	f = cvToFrequency(f);
	if (f > 2000.0f) {
		f = 2000.0f;
	}
	phasor.setFrequency(f);
}
