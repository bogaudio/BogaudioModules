
#include "VCO.hpp"
#include "dsp/pitch.hpp"

float VCO::VCOFrequencyParamQuantity::offset() {
	VCO* vco = dynamic_cast<VCO*>(module);
	return vco->_slowMode ? vco->slowModeOffset : 0.0f;
}

float VCO::VCOFrequencyParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	VCO* vco = dynamic_cast<VCO*>(module);
	return vco->_linearMode ? (vco->_slowMode ? v : v * 1000.0f) : FrequencyParamQuantity::getDisplayValue();
}

void VCO::VCOFrequencyParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}

	VCO* vco = dynamic_cast<VCO*>(module);
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

void VCO::Engine::reset() {
	syncTrigger.reset();
}

void VCO::Engine::sampleRateChange(float sampleRate) {
	phasor.setSampleRate(sampleRate);
	square.setSampleRate(sampleRate);
	saw.setSampleRate(sampleRate);
	squareDecimator.setParams(sampleRate, oversample);
	sawDecimator.setParams(sampleRate, oversample);
	triangleDecimator.setParams(sampleRate, oversample);
	squarePulseWidthSL.setParams(sampleRate, 0.1f, 2.0f);
}

void VCO::Engine::setFrequency(float f) {
	if (frequency != f && f < 0.475f * phasor._sampleRate) {
		frequency = f;
		phasor.setFrequency(frequency / (float)oversample);
		square.setFrequency(frequency);
		saw.setFrequency(frequency);
	}
}

void VCO::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void VCO::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	_oversampleThreshold = 0.06f * sampleRate;

	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange(sampleRate);
	}
}

bool VCO::active() {
	return (
		outputs[SQUARE_OUTPUT].isConnected() ||
		outputs[SAW_OUTPUT].isConnected() ||
		outputs[TRIANGLE_OUTPUT].isConnected() ||
		outputs[SINE_OUTPUT].isConnected()
	);
}

int VCO::channels() {
	return std::max(1, std::max(inputs[PITCH_INPUT].getChannels(), inputs[FM_INPUT].getChannels()));
}

void VCO::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->sampleRateChange(APP->engine->getSampleRate());
	if (c > 0) {
		_engines[c]->phasor.syncPhase(_engines[0]->phasor);
	}
}

void VCO::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void VCO::modulate() {
	_fmLinearMode = params[FM_TYPE_PARAM].getValue() < 0.5f;
	_fmDepth = params[FM_PARAM].getValue();
}

void VCO::modulateChannel(int c) {
	Engine& e = *_engines[c];

	e.baseVOct = params[FREQUENCY_PARAM].getValue();
	e.baseVOct += params[FINE_PARAM].getValue() / 12.0f;
	if (inputs[PITCH_INPUT].isConnected()) {
		e.baseVOct += clamp(inputs[PITCH_INPUT].getVoltage(c), -5.0f, 5.0f);
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

	float pw = params[PW_PARAM].getValue();
	if (inputs[PW_INPUT].isConnected()) {
		pw *= clamp(inputs[PW_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	pw *= 1.0f - 2.0f * e.square.minPulseWidth;
	pw *= 0.5f;
	pw += 0.5f;
	e.square.setPulseWidth(e.squarePulseWidthSL.next(pw));
}

void VCO::always(const ProcessArgs& args) {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].getValue() > 0.5f;
	lights[LINEAR_LIGHT].value = _linearMode = params[LINEAR_PARAM].getValue() > 0.5f;
}

void VCO::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	if (e.syncTrigger.next(inputs[SYNC_INPUT].getPolyVoltage(c))) {
		e.phasor.resetPhase();
	}

	float frequency = e.baseHz;
	Phasor::phase_delta_t phaseOffset = 0;
	if (inputs[FM_INPUT].isConnected() && _fmDepth > 0.01f) {
		float fm = inputs[FM_INPUT].getPolyVoltage(c) * _fmDepth;
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

	float squareOut = 0.0f;
	float sawOut = 0.0f;
	float triangleOut = 0.0f;
	if (oMix > 0.0f) {
		for (int i = 0; i < Engine::oversample; ++i) {
			e.phasor.advancePhase();
			if (outputs[SQUARE_OUTPUT].isConnected()) {
				e.squareBuffer[i] = e.square.nextFromPhasor(e.phasor, phaseOffset);
			}
			if (outputs[SAW_OUTPUT].isConnected()) {
				e.sawBuffer[i] = e.saw.nextFromPhasor(e.phasor, phaseOffset);
			}
			if (outputs[TRIANGLE_OUTPUT].isConnected()) {
				e.triangleBuffer[i] = e.triangle.nextFromPhasor(e.phasor, phaseOffset);
			}
		}
		if (outputs[SQUARE_OUTPUT].isConnected()) {
			squareOut += oMix * amplitude * e.squareDecimator.next(e.squareBuffer);
		}
		if (outputs[SAW_OUTPUT].isConnected()) {
			sawOut += oMix * amplitude * e.sawDecimator.next(e.sawBuffer);
		}
		if (outputs[TRIANGLE_OUTPUT].isConnected()) {
			triangleOut += oMix * amplitude * e.triangleDecimator.next(e.triangleBuffer);
		}
	}
	else {
		e.phasor.advancePhase(Engine::oversample);
	}
	if (mix > 0.0f) {
		if (outputs[SQUARE_OUTPUT].isConnected()) {
			squareOut += mix * amplitude * e.square.nextFromPhasor(e.phasor, phaseOffset);
		}
		if (outputs[SAW_OUTPUT].isConnected()) {
			sawOut += mix * amplitude * e.saw.nextFromPhasor(e.phasor, phaseOffset);
		}
		if (outputs[TRIANGLE_OUTPUT].isConnected()) {
			triangleOut += mix * amplitude * e.triangle.nextFromPhasor(e.phasor, phaseOffset);
		}
	}

	outputs[SQUARE_OUTPUT].setChannels(_channels);
	outputs[SQUARE_OUTPUT].setVoltage(squareOut, c);
	outputs[SAW_OUTPUT].setChannels(_channels);
	outputs[SAW_OUTPUT].setVoltage(sawOut, c);
	outputs[TRIANGLE_OUTPUT].setChannels(_channels);
	outputs[TRIANGLE_OUTPUT].setVoltage(triangleOut, c);
	outputs[SINE_OUTPUT].setChannels(_channels);
	outputs[SINE_OUTPUT].setVoltage(outputs[SINE_OUTPUT].isConnected() ? (amplitude * e.sine.nextFromPhasor(e.phasor, phaseOffset)) : 0.0f, c);
}

struct VCOWidget : ModuleWidget {
	static constexpr int hp = 10;

	VCOWidget(VCO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCO.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(41.0, 45.0);
		auto fineParamPosition = Vec(48.0, 153.0);
		auto slowParamPosition = Vec(123.0, 149.7);
		auto linearParamPosition = Vec(123.0, 162.7);
		auto pwParamPosition = Vec(62.0, 188.0);
		auto fmParamPosition = Vec(62.0, 230.0);
		auto fmTypeParamPosition = Vec(100.5, 231.5);

		auto pwInputPosition = Vec(15.0, 274.0);
		auto fmInputPosition = Vec(47.0, 274.0);
		auto pitchInputPosition = Vec(15.0, 318.0);
		auto syncInputPosition = Vec(47.0, 318.0);

		auto squareOutputPosition = Vec(79.0, 274.0);
		auto sawOutputPosition = Vec(111.0, 274.0);
		auto triangleOutputPosition = Vec(79.0, 318.0);
		auto sineOutputPosition = Vec(111.0, 318.0);

		auto slowLightPosition = Vec(82.0, 151.0);
		auto linearLightPosition = Vec(82.0, 164.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob68>(frequencyParamPosition, module, VCO::FREQUENCY_PARAM));
		addParam(createParam<Knob16>(fineParamPosition, module, VCO::FINE_PARAM));
		addParam(createParam<StatefulButton9>(slowParamPosition, module, VCO::SLOW_PARAM));
		addParam(createParam<Knob26>(pwParamPosition, module, VCO::PW_PARAM));
		addParam(createParam<Knob26>(fmParamPosition, module, VCO::FM_PARAM));
		addParam(createParam<SliderSwitch2State14>(fmTypeParamPosition, module, VCO::FM_TYPE_PARAM));
		addParam(createParam<StatefulButton9>(linearParamPosition, module, VCO::LINEAR_PARAM));

		addInput(createInput<Port24>(pitchInputPosition, module, VCO::PITCH_INPUT));
		addInput(createInput<Port24>(syncInputPosition, module, VCO::SYNC_INPUT));
		addInput(createInput<Port24>(pwInputPosition, module, VCO::PW_INPUT));
		addInput(createInput<Port24>(fmInputPosition, module, VCO::FM_INPUT));

		addOutput(createOutput<Port24>(squareOutputPosition, module, VCO::SQUARE_OUTPUT));
		addOutput(createOutput<Port24>(sawOutputPosition, module, VCO::SAW_OUTPUT));
		addOutput(createOutput<Port24>(triangleOutputPosition, module, VCO::TRIANGLE_OUTPUT));
		addOutput(createOutput<Port24>(sineOutputPosition, module, VCO::SINE_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(slowLightPosition, module, VCO::SLOW_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(linearLightPosition, module, VCO::LINEAR_LIGHT));
	}
};

Model* modelVCO = bogaudio::createModel<VCO, VCOWidget>("Bogaudio-VCO", "VCO", "Oscillator with 4 waveforms, PWM, FM, hard sync", "Oscillator", "Polyphonic");
