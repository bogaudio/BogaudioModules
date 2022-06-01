
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

#define OFFSET_SCALE "offset_scale"

json_t* LFOBase::saveToJson(json_t* root) {
	json_object_set_new(root, OFFSET_SCALE, json_real(_offsetScale));
	return root;
}

void LFOBase::loadFromJson(json_t* root) {
	json_t* os = json_object_get(root, OFFSET_SCALE);
	if (os) {
		_offsetScale = clamp(json_real_value(os), 1.0f, 2.0f);
	}
}


void LFOBaseModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<LFOBase*>(module);
	assert(m);

	OptionsMenuItem* o = new OptionsMenuItem("Offset range");
	o->addItem(OptionMenuItem("+/-5V", [m]() { return (int)m->_offsetScale == 1; }, [m]() { m->_offsetScale = 1.0f; }));
	o->addItem(OptionMenuItem("+/-10V", [m]() { return (int)m->_offsetScale == 2; }, [m]() { m->_offsetScale = 2.0f; }));
	OptionsMenuItem::addToMenu(o, menu);
}
