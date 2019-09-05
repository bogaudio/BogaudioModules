
#include "VCO.hpp"
#include "dsp/pitch.hpp"

float VCO::VCOFrequencyParamQuantity::offset() {
	VCO* vco = dynamic_cast<VCO*>(module);
	return vco->_slowMode ? vco->_slowModeOffset : 0.0f;
}

void VCO::onReset() {
	_syncTrigger.reset();
	_modulationStep = modulationSteps;
}

void VCO::onSampleRateChange() {
	setSampleRate(APP->engine->getSampleRate());
	_modulationStep = modulationSteps;
}

void VCO::process(const ProcessArgs& args) {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].getValue() > 0.5f;
	_fmLinearMode = params[FM_TYPE_PARAM].getValue() < 0.5f;

	if (!(
		outputs[SQUARE_OUTPUT].isConnected() ||
		outputs[SAW_OUTPUT].isConnected() ||
		outputs[TRIANGLE_OUTPUT].isConnected() ||
		outputs[SINE_OUTPUT].isConnected()
	)) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		_baseVOct = params[FREQUENCY_PARAM].getValue();
		_baseVOct += params[FINE_PARAM].getValue() / 12.0f;
		if (inputs[PITCH_INPUT].isConnected()) {
			_baseVOct += clamp(inputs[PITCH_INPUT].getVoltage(), -5.0f, 5.0f);
		}
		if (_slowMode) {
			_baseVOct += _slowModeOffset;
		}
		_baseHz = cvToFrequency(_baseVOct);

		float pw = params[PW_PARAM].getValue();
		if (inputs[PW_INPUT].isConnected()) {
			pw *= clamp(inputs[PW_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
		}
		pw *= 1.0f - 2.0f * _square.minPulseWidth;
		pw *= 0.5f;
		pw += 0.5f;
		_square.setPulseWidth(_squarePulseWidthSL.next(pw));

		_fmDepth = params[FM_PARAM].getValue();
	}

	if (_syncTrigger.next(inputs[SYNC_INPUT].getVoltage())) {
		_phasor.resetPhase();
	}

	float frequency = _baseHz;
	Phasor::phase_delta_t phaseOffset = 0;
	if (inputs[FM_INPUT].isConnected() && _fmDepth > 0.01f) {
		float fm = inputs[FM_INPUT].getVoltageSum() * _fmDepth;
		if (_fmLinearMode) {
			phaseOffset = Phasor::radiansToPhase(2.0f * fm);
		}
		else {
			frequency = cvToFrequency(_baseVOct + fm);
		}
	}
	setFrequency(frequency);

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
		for (int i = 0; i < oversample; ++i) {
			_phasor.advancePhase();
			if (outputs[SQUARE_OUTPUT].isConnected()) {
				_squareBuffer[i] = _square.nextFromPhasor(_phasor, phaseOffset);
			}
			if (outputs[SAW_OUTPUT].isConnected()) {
				_sawBuffer[i] = _saw.nextFromPhasor(_phasor, phaseOffset);
			}
			if (outputs[TRIANGLE_OUTPUT].isConnected()) {
				_triangleBuffer[i] = _triangle.nextFromPhasor(_phasor, phaseOffset);
			}
		}
		if (outputs[SQUARE_OUTPUT].isConnected()) {
			squareOut += oMix * amplitude * _squareDecimator.next(_squareBuffer);
		}
		if (outputs[SAW_OUTPUT].isConnected()) {
			sawOut += oMix * amplitude * _sawDecimator.next(_sawBuffer);
		}
		if (outputs[TRIANGLE_OUTPUT].isConnected()) {
			triangleOut += oMix * amplitude * _triangleDecimator.next(_triangleBuffer);
		}
	}
	else {
		_phasor.advancePhase(oversample);
	}
	if (mix > 0.0f) {
		if (outputs[SQUARE_OUTPUT].isConnected()) {
			squareOut += mix * amplitude * _square.nextFromPhasor(_phasor, phaseOffset);
		}
		if (outputs[SAW_OUTPUT].isConnected()) {
			sawOut += mix * amplitude * _saw.nextFromPhasor(_phasor, phaseOffset);
		}
		if (outputs[TRIANGLE_OUTPUT].isConnected()) {
			triangleOut += mix * amplitude * _triangle.nextFromPhasor(_phasor, phaseOffset);
		}
	}

	outputs[SQUARE_OUTPUT].setVoltage(squareOut);
	outputs[SAW_OUTPUT].setVoltage(sawOut);
	outputs[TRIANGLE_OUTPUT].setVoltage(triangleOut);
	outputs[SINE_OUTPUT].setVoltage(outputs[SINE_OUTPUT].isConnected() ? (amplitude * _sine.nextFromPhasor(_phasor, phaseOffset)) : 0.0f);
}

void VCO::setSampleRate(float sampleRate) {
	_oversampleThreshold = 0.06f * sampleRate;
	_phasor.setSampleRate(sampleRate);
	_square.setSampleRate(sampleRate);
	_saw.setSampleRate(sampleRate);
	_squareDecimator.setParams(sampleRate, oversample);
	_sawDecimator.setParams(sampleRate, oversample);
	_triangleDecimator.setParams(sampleRate, oversample);
	_squarePulseWidthSL.setParams(sampleRate, 0.1f, 2.0f);
}

void VCO::setFrequency(float frequency) {
	if (_frequency != frequency && frequency < 0.475f * _phasor._sampleRate) {
		_frequency = frequency;
		_phasor.setFrequency(_frequency / (float)oversample);
		_square.setFrequency(_frequency);
		_saw.setFrequency(_frequency);
	}
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
