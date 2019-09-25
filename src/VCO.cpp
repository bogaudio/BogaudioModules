
#include "VCO.hpp"
#include "dsp/pitch.hpp"

float VCO::VCOFrequencyParamQuantity::offset() {
	VCO* vco = dynamic_cast<VCO*>(module);
	return vco->_slowMode ? vco->slowModeOffset : 0.0f;
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
	for (int c = 0; c < maxChannels; ++c) {
		if (_engines[c]) {
			_engines[c]->reset();
		}
	}
}

void VCO::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	_oversampleThreshold = 0.06f * sampleRate;

	for (int c = 0; c < maxChannels; ++c) {
		if (_engines[c]) {
			_engines[c]->sampleRateChange(sampleRate);
		}
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
	return std::max(1, inputs[PITCH_INPUT].getChannels());
}

void VCO::addEngine(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->sampleRateChange(APP->engine->getSampleRate());
}

void VCO::removeEngine(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void VCO::modulate() {
	_fmLinearMode = params[FM_TYPE_PARAM].getValue() < 0.5f;
	_fmDepth = params[FM_PARAM].getValue();
}

void VCO::modulateChannel(int c) {
	_engines[c]->baseVOct = params[FREQUENCY_PARAM].getValue();
	_engines[c]->baseVOct += params[FINE_PARAM].getValue() / 12.0f;
	if (inputs[PITCH_INPUT].isConnected()) {
		_engines[c]->baseVOct += clamp(inputs[PITCH_INPUT].getVoltage(c), -5.0f, 5.0f);
	}
	if (_slowMode) {
		_engines[c]->baseVOct += slowModeOffset;
	}
	_engines[c]->baseHz = cvToFrequency(_engines[c]->baseVOct);

	float pw = params[PW_PARAM].getValue();
	if (inputs[PW_INPUT].isConnected()) {
		pw *= clamp(inputs[PW_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	pw *= 1.0f - 2.0f * _engines[c]->square.minPulseWidth;
	pw *= 0.5f;
	pw += 0.5f;
	_engines[c]->square.setPulseWidth(_engines[c]->squarePulseWidthSL.next(pw));
}

void VCO::always(const ProcessArgs& args) {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].getValue() > 0.5f;
}

void VCO::processChannel(const ProcessArgs& args, int c) {
	if (_engines[c]->syncTrigger.next(inputs[SYNC_INPUT].getPolyVoltage(c))) {
		_engines[c]->phasor.resetPhase();
	}

	float frequency = _engines[c]->baseHz;
	Phasor::phase_delta_t phaseOffset = 0;
	if (inputs[FM_INPUT].isConnected() && _fmDepth > 0.01f) {
		float fm = inputs[FM_INPUT].getPolyVoltage(c) * _fmDepth;
		if (_fmLinearMode) {
			phaseOffset = Phasor::radiansToPhase(2.0f * fm);
		}
		else {
			frequency = cvToFrequency(_engines[c]->baseVOct + fm);
		}
	}
	_engines[c]->setFrequency(frequency);

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
			_engines[c]->phasor.advancePhase();
			if (outputs[SQUARE_OUTPUT].isConnected()) {
				_engines[c]->squareBuffer[i] = _engines[c]->square.nextFromPhasor(_engines[c]->phasor, phaseOffset);
			}
			if (outputs[SAW_OUTPUT].isConnected()) {
				_engines[c]->sawBuffer[i] = _engines[c]->saw.nextFromPhasor(_engines[c]->phasor, phaseOffset);
			}
			if (outputs[TRIANGLE_OUTPUT].isConnected()) {
				_engines[c]->triangleBuffer[i] = _engines[c]->triangle.nextFromPhasor(_engines[c]->phasor, phaseOffset);
			}
		}
		if (outputs[SQUARE_OUTPUT].isConnected()) {
			squareOut += oMix * amplitude * _engines[c]->squareDecimator.next(_engines[c]->squareBuffer);
		}
		if (outputs[SAW_OUTPUT].isConnected()) {
			sawOut += oMix * amplitude * _engines[c]->sawDecimator.next(_engines[c]->sawBuffer);
		}
		if (outputs[TRIANGLE_OUTPUT].isConnected()) {
			triangleOut += oMix * amplitude * _engines[c]->triangleDecimator.next(_engines[c]->triangleBuffer);
		}
	}
	else {
		_engines[c]->phasor.advancePhase(Engine::oversample);
	}
	if (mix > 0.0f) {
		if (outputs[SQUARE_OUTPUT].isConnected()) {
			squareOut += mix * amplitude * _engines[c]->square.nextFromPhasor(_engines[c]->phasor, phaseOffset);
		}
		if (outputs[SAW_OUTPUT].isConnected()) {
			sawOut += mix * amplitude * _engines[c]->saw.nextFromPhasor(_engines[c]->phasor, phaseOffset);
		}
		if (outputs[TRIANGLE_OUTPUT].isConnected()) {
			triangleOut += mix * amplitude * _engines[c]->triangle.nextFromPhasor(_engines[c]->phasor, phaseOffset);
		}
	}

	outputs[SQUARE_OUTPUT].setChannels(_channels);
	outputs[SQUARE_OUTPUT].setVoltage(squareOut, c);
	outputs[SAW_OUTPUT].setChannels(_channels);
	outputs[SAW_OUTPUT].setVoltage(sawOut, c);
	outputs[TRIANGLE_OUTPUT].setChannels(_channels);
	outputs[TRIANGLE_OUTPUT].setVoltage(triangleOut, c);
	outputs[SINE_OUTPUT].setChannels(_channels);
	outputs[SINE_OUTPUT].setVoltage(outputs[SINE_OUTPUT].isConnected() ? (amplitude * _engines[c]->sine.nextFromPhasor(_engines[c]->phasor, phaseOffset)) : 0.0f, c);
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
		auto slowParamPosition = Vec(122.0, 157.2);
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

		auto slowLightPosition = Vec(82.0, 158.5);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob68>(frequencyParamPosition, module, VCO::FREQUENCY_PARAM));
		addParam(createParam<Knob16>(fineParamPosition, module, VCO::FINE_PARAM));
		addParam(createParam<StatefulButton9>(slowParamPosition, module, VCO::SLOW_PARAM));
		addParam(createParam<Knob26>(pwParamPosition, module, VCO::PW_PARAM));
		addParam(createParam<Knob26>(fmParamPosition, module, VCO::FM_PARAM));
		addParam(createParam<SliderSwitch2State14>(fmTypeParamPosition, module, VCO::FM_TYPE_PARAM));

		addInput(createInput<Port24>(pitchInputPosition, module, VCO::PITCH_INPUT));
		addInput(createInput<Port24>(syncInputPosition, module, VCO::SYNC_INPUT));
		addInput(createInput<Port24>(pwInputPosition, module, VCO::PW_INPUT));
		addInput(createInput<Port24>(fmInputPosition, module, VCO::FM_INPUT));

		addOutput(createOutput<Port24>(squareOutputPosition, module, VCO::SQUARE_OUTPUT));
		addOutput(createOutput<Port24>(sawOutputPosition, module, VCO::SAW_OUTPUT));
		addOutput(createOutput<Port24>(triangleOutputPosition, module, VCO::TRIANGLE_OUTPUT));
		addOutput(createOutput<Port24>(sineOutputPosition, module, VCO::SINE_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(slowLightPosition, module, VCO::SLOW_LIGHT));
	}
};

Model* modelVCO = bogaudio::createModel<VCO, VCOWidget>("Bogaudio-VCO", "VCO",  "oscillator", "Oscillator");
