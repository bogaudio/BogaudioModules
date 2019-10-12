
#include "EightFO.hpp"

const Phasor::phase_delta_t basePhase7Offset = Phasor::radiansToPhase(1.75f * M_PI);
const Phasor::phase_delta_t basePhase6Offset = Phasor::radiansToPhase(1.5f * M_PI);
const Phasor::phase_delta_t basePhase5Offset = Phasor::radiansToPhase(1.25f * M_PI);
const Phasor::phase_delta_t basePhase4Offset = Phasor::radiansToPhase(M_PI);
const Phasor::phase_delta_t basePhase3Offset = Phasor::radiansToPhase(0.75f * M_PI);
const Phasor::phase_delta_t basePhase2Offset = Phasor::radiansToPhase(0.5f * M_PI);
const Phasor::phase_delta_t basePhase1Offset = Phasor::radiansToPhase(0.25f * M_PI);
const Phasor::phase_delta_t basePhase0Offset = Phasor::radiansToPhase(0.0f);

void EightFO::Engine::reset() {
	resetTrigger.reset();
	sampleStep = phasor._sampleRate;
}

void EightFO::Engine::sampleRateChange() {
	phasor.setSampleRate(APP->engine->getSampleRate());
	sampleStep = phasor._sampleRate;
}

void EightFO::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void EightFO::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

bool EightFO::active() {
	return (
		outputs[PHASE7_OUTPUT].isConnected() ||
		outputs[PHASE6_OUTPUT].isConnected() ||
		outputs[PHASE5_OUTPUT].isConnected() ||
		outputs[PHASE4_OUTPUT].isConnected() ||
		outputs[PHASE3_OUTPUT].isConnected() ||
		outputs[PHASE2_OUTPUT].isConnected() ||
		outputs[PHASE1_OUTPUT].isConnected() ||
		outputs[PHASE0_OUTPUT].isConnected()
	);
}

int EightFO::channels() {
	return std::max(1, inputs[PITCH_INPUT].getChannels());
}

void EightFO::addEngine(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->sampleRateChange();
	if (c > 0) {
		_engines[c]->phasor.syncPhase(_engines[0]->phasor);
	}
}

void EightFO::removeEngine(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void EightFO::modulate() {
	_wave = (Wave)roundf(params[WAVE_PARAM].getValue());
}

void EightFO::modulateChannel(int c) {
	Engine& e = *_engines[c];

	setFrequency(params[FREQUENCY_PARAM], inputs[PITCH_INPUT], e.phasor, c);

	if (_wave == SQUARE_WAVE) {
		float pw = params[SAMPLE_PWM_PARAM].getValue();
		if (inputs[SAMPLE_PWM_INPUT].isConnected()) {
			pw *= clamp(inputs[SAMPLE_PWM_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		}
		pw *= 1.0f - 2.0f * e.square.minPulseWidth;
		pw *= 0.5f;
		pw += 0.5f;
		e.square.setPulseWidth(pw);
		e.sampleSteps = 1;
	}
	else {
		float sample = fabsf(params[SAMPLE_PWM_PARAM].getValue());
		if (inputs[SAMPLE_PWM_INPUT].isConnected()) {
			sample *= clamp(fabsf(inputs[SAMPLE_PWM_INPUT].getPolyVoltage(c)) / 5.0f, 0.0f, 1.0f);
		}
		float maxSampleSteps = (e.phasor._sampleRate / e.phasor._frequency) / 4.0f;
		e.sampleSteps = clamp((int)(sample * maxSampleSteps), 1, (int)maxSampleSteps);
		e.square.setPulseWidth(SquareOscillator::defaultPulseWidth);
	}

	e.offset = params[OFFSET_PARAM].getValue();
	if (inputs[OFFSET_INPUT].isConnected()) {
		e.offset *= clamp(inputs[OFFSET_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	e.offset *= 5.0f;
	e.scale = params[SCALE_PARAM].getValue();
	if (inputs[SCALE_INPUT].isConnected()) {
		e.scale *= clamp(inputs[SCALE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}

	e.phase7Offset = phaseOffset(c, params[PHASE7_PARAM], inputs[PHASE7_INPUT], basePhase7Offset);
	e.phase6Offset = phaseOffset(c, params[PHASE6_PARAM], inputs[PHASE6_INPUT], basePhase6Offset);
	e.phase5Offset = phaseOffset(c, params[PHASE5_PARAM], inputs[PHASE5_INPUT], basePhase5Offset);
	e.phase4Offset = phaseOffset(c, params[PHASE4_PARAM], inputs[PHASE4_INPUT], basePhase4Offset);
	e.phase3Offset = phaseOffset(c, params[PHASE3_PARAM], inputs[PHASE3_INPUT], basePhase3Offset);
	e.phase2Offset = phaseOffset(c, params[PHASE2_PARAM], inputs[PHASE2_INPUT], basePhase2Offset);
	e.phase1Offset = phaseOffset(c, params[PHASE1_PARAM], inputs[PHASE1_INPUT], basePhase1Offset);
	e.phase0Offset = phaseOffset(c, params[PHASE0_PARAM], inputs[PHASE0_INPUT], basePhase0Offset);
}

void EightFO::always(const ProcessArgs& args) {
	lights[SLOW_LIGHT].value = _slowMode = params[SLOW_PARAM].getValue() > 0.5f;
}

void EightFO::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	if (e.resetTrigger.next(inputs[RESET_INPUT].getPolyVoltage(c))) {
		e.phasor.resetPhase();
	}

	e.phasor.advancePhase();
	bool useSample = false;
	if (e.sampleSteps > 1) {
		++e.sampleStep;
		if (e.sampleStep >= e.sampleSteps) {
			e.sampleStep = 0;
		}
		else {
			useSample = true;
		}
	}
	updateOutput(c, useSample, outputs[PHASE7_OUTPUT], e.phase7Offset, e.phase7Sample, e.phase7Active);
	updateOutput(c, useSample, outputs[PHASE6_OUTPUT], e.phase6Offset, e.phase6Sample, e.phase6Active);
	updateOutput(c, useSample, outputs[PHASE5_OUTPUT], e.phase5Offset, e.phase5Sample, e.phase5Active);
	updateOutput(c, useSample, outputs[PHASE4_OUTPUT], e.phase4Offset, e.phase4Sample, e.phase4Active);
	updateOutput(c, useSample, outputs[PHASE3_OUTPUT], e.phase3Offset, e.phase3Sample, e.phase3Active);
	updateOutput(c, useSample, outputs[PHASE2_OUTPUT], e.phase2Offset, e.phase2Sample, e.phase2Active);
	updateOutput(c, useSample, outputs[PHASE1_OUTPUT], e.phase1Offset, e.phase1Sample, e.phase1Active);
	updateOutput(c, useSample, outputs[PHASE0_OUTPUT], e.phase0Offset, e.phase0Sample, e.phase0Active);
}

Phasor::phase_delta_t EightFO::phaseOffset(int c, Param& p, Input& i, Phasor::phase_delta_t baseOffset) {
	float o = p.getValue() * Phasor::maxPhase / 2.0f;
	if (i.isConnected()) {
		o *= clamp(i.getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	return baseOffset - o;
}

void EightFO::updateOutput(int c, bool useSample, Output& output, Phasor::phase_delta_t& offset, float& sample, bool& active) {
	if (output.isConnected()) {
		output.setChannels(_channels);
		if (useSample && active) {
			output.setVoltage(sample, c);
		}
		else {
			float v = 0.0f;
			switch (_wave) {
				case NO_WAVE: {
					assert(false);
				}
				case SINE_WAVE: {
					v = _engines[c]->sine.nextFromPhasor(_engines[c]->phasor, offset);
					break;
				}
				case TRIANGLE_WAVE: {
					v = _engines[c]->triangle.nextFromPhasor(_engines[c]->phasor, offset);
					break;
				}
				case RAMP_UP_WAVE: {
					v = _engines[c]->ramp.nextFromPhasor(_engines[c]->phasor, offset);
					break;
				}
				case RAMP_DOWN_WAVE: {
					v = -_engines[c]->ramp.nextFromPhasor(_engines[c]->phasor, offset);
					break;
				}
				case SQUARE_WAVE: {
					v = _engines[c]->square.nextFromPhasor(_engines[c]->phasor, offset);
					break;
				}
			}
			output.setVoltage(sample = amplitude * _engines[c]->scale * v + _engines[c]->offset, c);
		}
		active = true;
	}
	else {
		active = false;
	}
}

struct EightFOWidget : ModuleWidget {
	static constexpr int hp = 17;

	EightFOWidget(EightFO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EightFO.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(40.0, 45.0);
		auto slowParamPosition = Vec(126.0, 333.7);
		auto waveParamPosition = Vec(20.0, 168.0);
		auto samplePwmParamPosition = Vec(100.0, 163.0);
		auto offsetParamPosition = Vec(40.0, 226.0);
		auto scaleParamPosition = Vec(100.0, 226.0);
		auto phase7ParamPosition = Vec(144.0, 40.0);
		auto phase6ParamPosition = Vec(144.0, 80.0);
		auto phase5ParamPosition = Vec(144.0, 120.0);
		auto phase4ParamPosition = Vec(144.0, 160.0);
		auto phase3ParamPosition = Vec(144.0, 200.0);
		auto phase2ParamPosition = Vec(144.0, 240.0);
		auto phase1ParamPosition = Vec(144.0, 280.0);
		auto phase0ParamPosition = Vec(144.0, 320.0);

		auto phase7InputPosition = Vec(179.0, 36.0);
		auto phase6InputPosition = Vec(179.0, 76.0);
		auto phase5InputPosition = Vec(179.0, 116.0);
		auto phase4InputPosition = Vec(179.0, 156.0);
		auto phase3InputPosition = Vec(179.0, 196.0);
		auto phase2InputPosition = Vec(179.0, 236.0);
		auto phase1InputPosition = Vec(179.0, 276.0);
		auto phase0InputPosition = Vec(179.0, 316.0);
		auto samplePwmInputPosition = Vec(14.0, 277.0);
		auto offsetInputPosition = Vec(52.0, 277.0);
		auto scaleInputPosition = Vec(90.0, 277.0);
		auto pitchInputPosition = Vec(14.0, 318.0);
		auto resetInputPosition = Vec(52.0, 318.0);

		auto phase7OutputPosition = Vec(218.0, 36.0);
		auto phase6OutputPosition = Vec(218.0, 76.0);
		auto phase5OutputPosition = Vec(218.0, 116.0);
		auto phase4OutputPosition = Vec(218.0, 156.0);
		auto phase3OutputPosition = Vec(218.0, 196.0);
		auto phase2OutputPosition = Vec(218.0, 236.0);
		auto phase1OutputPosition = Vec(218.0, 276.0);
		auto phase0OutputPosition = Vec(218.0, 316.0);

		auto slowLightPosition = Vec(86.0, 335.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob68>(frequencyParamPosition, module, EightFO::FREQUENCY_PARAM));
		{
			auto w = createParam<Knob16>(waveParamPosition, module, EightFO::WAVE_PARAM);
			auto k = dynamic_cast<SvgKnob*>(w);
			k->snap = true;
			k->minAngle = 0.0;
			k->maxAngle = M_PI;
			k->speed = 3.0;
			addParam(w);
		}
		addParam(createParam<StatefulButton9>(slowParamPosition, module, EightFO::SLOW_PARAM));
		addParam(createParam<Knob26>(samplePwmParamPosition, module, EightFO::SAMPLE_PWM_PARAM));
		addParam(createParam<Knob26>(offsetParamPosition, module, EightFO::OFFSET_PARAM));
		addParam(createParam<Knob26>(scaleParamPosition, module, EightFO::SCALE_PARAM));

		addPhaseParam(phase7ParamPosition, module, EightFO::PHASE7_PARAM, Phasor::phaseToRadians(basePhase7Offset));
		addPhaseParam(phase6ParamPosition, module, EightFO::PHASE6_PARAM, Phasor::phaseToRadians(basePhase6Offset));
		addPhaseParam(phase5ParamPosition, module, EightFO::PHASE5_PARAM, Phasor::phaseToRadians(basePhase5Offset));
		addPhaseParam(phase4ParamPosition, module, EightFO::PHASE4_PARAM, Phasor::phaseToRadians(basePhase4Offset));
		addPhaseParam(phase3ParamPosition, module, EightFO::PHASE3_PARAM, Phasor::phaseToRadians(basePhase3Offset));
		addPhaseParam(phase2ParamPosition, module, EightFO::PHASE2_PARAM, Phasor::phaseToRadians(basePhase2Offset));
		addPhaseParam(phase1ParamPosition, module, EightFO::PHASE1_PARAM, Phasor::phaseToRadians(basePhase1Offset));
		addPhaseParam(phase0ParamPosition, module, EightFO::PHASE0_PARAM, Phasor::phaseToRadians(basePhase0Offset));

		addInput(createInput<Port24>(samplePwmInputPosition, module, EightFO::SAMPLE_PWM_INPUT));
		addInput(createInput<Port24>(offsetInputPosition, module, EightFO::OFFSET_INPUT));
		addInput(createInput<Port24>(scaleInputPosition, module, EightFO::SCALE_INPUT));
		addInput(createInput<Port24>(phase7InputPosition, module, EightFO::PHASE7_INPUT));
		addInput(createInput<Port24>(phase6InputPosition, module, EightFO::PHASE6_INPUT));
		addInput(createInput<Port24>(phase5InputPosition, module, EightFO::PHASE5_INPUT));
		addInput(createInput<Port24>(phase4InputPosition, module, EightFO::PHASE4_INPUT));
		addInput(createInput<Port24>(phase3InputPosition, module, EightFO::PHASE3_INPUT));
		addInput(createInput<Port24>(phase2InputPosition, module, EightFO::PHASE2_INPUT));
		addInput(createInput<Port24>(phase1InputPosition, module, EightFO::PHASE1_INPUT));
		addInput(createInput<Port24>(phase0InputPosition, module, EightFO::PHASE0_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, EightFO::PITCH_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, EightFO::RESET_INPUT));

		addOutput(createOutput<Port24>(phase7OutputPosition, module, EightFO::PHASE7_OUTPUT));
		addOutput(createOutput<Port24>(phase6OutputPosition, module, EightFO::PHASE6_OUTPUT));
		addOutput(createOutput<Port24>(phase5OutputPosition, module, EightFO::PHASE5_OUTPUT));
		addOutput(createOutput<Port24>(phase4OutputPosition, module, EightFO::PHASE4_OUTPUT));
		addOutput(createOutput<Port24>(phase3OutputPosition, module, EightFO::PHASE3_OUTPUT));
		addOutput(createOutput<Port24>(phase2OutputPosition, module, EightFO::PHASE2_OUTPUT));
		addOutput(createOutput<Port24>(phase1OutputPosition, module, EightFO::PHASE1_OUTPUT));
		addOutput(createOutput<Port24>(phase0OutputPosition, module, EightFO::PHASE0_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(slowLightPosition, module, EightFO::SLOW_LIGHT));
	}

	void addPhaseParam(const Vec& position, Module* module, EightFO::ParamsIds paramId, float rotation) {
		auto w = createParam<Knob16>(position, module, paramId);
		auto k = dynamic_cast<SvgKnob*>(w);
		k->minAngle += 0.5 * M_PI - rotation;
		k->maxAngle += 0.5 * M_PI - rotation;
		addParam(w);
	}
};

Model* modelEightFO = bogaudio::createModel<EightFO, EightFOWidget>("Bogaudio-EightFO", "8FO", "LFO with outputs at 8 different phases", "LFO", "Polyphonic");
