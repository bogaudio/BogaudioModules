
#include "FourFO.hpp"

const Phasor::phase_delta_t basePhase3Offset = Phasor::radiansToPhase(1.5f * M_PI);
const Phasor::phase_delta_t basePhase2Offset = Phasor::radiansToPhase(M_PI);
const Phasor::phase_delta_t basePhase1Offset = Phasor::radiansToPhase(0.5f * M_PI);
const Phasor::phase_delta_t basePhase0Offset = Phasor::radiansToPhase(0.0f);

void FourFO::Engine::reset() {
	resetTrigger.reset();
	sampleStep = phasor._sampleRate;
}

void FourFO::Engine::sampleRateChange() {
	phasor.setSampleRate(APP->engine->getSampleRate());
	sampleStep = phasor._sampleRate;
}

void FourFO::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void FourFO::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

bool FourFO::active() {
	return (
		outputs[PHASE3_OUTPUT].isConnected() ||
		outputs[PHASE2_OUTPUT].isConnected() ||
		outputs[PHASE1_OUTPUT].isConnected() ||
		outputs[PHASE0_OUTPUT].isConnected()
	);
}

int FourFO::channels() {
	return inputs[PITCH_INPUT].getChannels();
}

void FourFO::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->sampleRateChange();
	if (c > 0) {
		_engines[c]->phasor.syncPhase(_engines[0]->phasor);
	}
}

void FourFO::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void FourFO::modulate() {
	_wave = (Wave)roundf(params[WAVE_PARAM].getValue());
	_slowMode = params[SLOW_PARAM].getValue() > 0.5f;
}

void FourFO::modulateChannel(int c) {
	Engine& e = *_engines[c];

	setFrequency(params[FREQUENCY_PARAM], inputs[PITCH_INPUT], e.phasor, c);

	switch (_wave) {
		case SQUARE_WAVE: {
			float pw = params[SAMPLE_PWM_PARAM].getValue();
			if (inputs[SAMPLE_PWM_INPUT].isConnected()) {
				pw *= clamp(inputs[SAMPLE_PWM_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
			}
			pw *= 1.0f - 2.0f * e.square.minPulseWidth;
			pw *= 0.5f;
			pw += 0.5f;
			e.square.setPulseWidth(pw);
			e.sampleSteps = 1;
			break;
		}
		case STEPPED_WAVE: {
			e.sampleSteps = 1;
			break;
		}
		default: {
			float sample = fabsf(params[SAMPLE_PWM_PARAM].getValue());
			if (inputs[SAMPLE_PWM_INPUT].isConnected()) {
				sample *= clamp(fabsf(inputs[SAMPLE_PWM_INPUT].getPolyVoltage(c)) / 5.0f, 0.0f, 1.0f);
			}
			float maxSampleSteps = (e.phasor._sampleRate / e.phasor._frequency) / 4.0f;
			e.sampleSteps = clamp((int)(sample * maxSampleSteps), 1, (int)maxSampleSteps);
		}
	}

	float smooth = params[SMOOTH_PARAM].getValue();
	if (inputs[SMOOTH_INPUT].isConnected()) {
		smooth *= clamp(inputs[SMOOTH_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	float sr = APP->engine->getSampleRate();
	e.phase3Smoother.setParams(sr, e.phasor._frequency, smooth);
	e.phase2Smoother.setParams(sr, e.phasor._frequency, smooth);
	e.phase1Smoother.setParams(sr, e.phasor._frequency, smooth);
	e.phase0Smoother.setParams(sr, e.phasor._frequency, smooth);

	e.offset = params[OFFSET_PARAM].getValue();
	if (inputs[OFFSET_INPUT].isConnected()) {
		e.offset *= clamp(inputs[OFFSET_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	e.offset *= _offsetScale;
	e.offset *= 5.0f;
	e.scale = params[SCALE_PARAM].getValue();
	if (inputs[SCALE_INPUT].isConnected()) {
		e.scale *= clamp(inputs[SCALE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}

	e.phase3Offset = phaseOffset(c, params[PHASE3_PARAM], inputs[PHASE3_INPUT], basePhase3Offset);
	e.phase2Offset = phaseOffset(c, params[PHASE2_PARAM], inputs[PHASE2_INPUT], basePhase2Offset);
	e.phase1Offset = phaseOffset(c, params[PHASE1_PARAM], inputs[PHASE1_INPUT], basePhase1Offset);
	e.phase0Offset = phaseOffset(c, params[PHASE0_PARAM], inputs[PHASE0_INPUT], basePhase0Offset);
}

void FourFO::processChannel(const ProcessArgs& args, int c) {
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
	updateOutput(c, useSample, outputs[PHASE3_OUTPUT], e.phase3Offset, e.phase3Sample, e.phase3Active, e.phase3Smoother);
	updateOutput(c, useSample, outputs[PHASE2_OUTPUT], e.phase2Offset, e.phase2Sample, e.phase2Active, e.phase2Smoother);
	updateOutput(c, useSample, outputs[PHASE1_OUTPUT], e.phase1Offset, e.phase1Sample, e.phase1Active, e.phase1Smoother);
	updateOutput(c, useSample, outputs[PHASE0_OUTPUT], e.phase0Offset, e.phase0Sample, e.phase0Active, e.phase0Smoother);
}

Phasor::phase_delta_t FourFO::phaseOffset(int c, Param& p, Input& i, Phasor::phase_delta_t baseOffset) {
	float o = p.getValue() * Phasor::cyclePhase / 2.0f;
	if (i.isConnected()) {
		o *= clamp(i.getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	return baseOffset - o;
}

void FourFO::updateOutput(int c, bool useSample, Output& output, Phasor::phase_delta_t& offset, float& sample, bool& active, Smoother& smoother) {
	if (output.isConnected()) {
		output.setChannels(_channels);
		if (!useSample || !active) {
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
				case STEPPED_WAVE: {
					v = _engines[c]->stepped.nextFromPhasor(_engines[c]->phasor, offset);
					break;
				}
			}
			sample = amplitude * _engines[c]->scale * v + _engines[c]->offset;
		}
		output.setVoltage(clamp(smoother.next(sample), -12.0f, 12.0f), c);
		active = true;
	}
	else {
		active = false;
	}
}

struct FourFOWidget : LFOBaseModuleWidget {
	static constexpr int hp = 10;

	FourFOWidget(FourFO* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "FourFO");
		createScrews();

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(23.0, 42.0);
		auto waveParamPosition = Vec(95.0, 53.0);
		auto slowParamPosition = Vec(49.0, 108.7);
		auto samplePwmParamPosition = Vec(13.0, 149.0);
		auto smoothParamPosition = Vec(48.0, 149.0);
		auto offsetParamPosition = Vec(13.0, 198.0);
		auto scaleParamPosition = Vec(48.0, 198.0);
		auto phase0ParamPosition = Vec(121.0, 193.0);
		auto phase1ParamPosition = Vec(84.0, 193.0);
		auto phase2ParamPosition = Vec(84.0, 251.0);
		auto phase3ParamPosition = Vec(121.0, 251.0);

		auto samplePwmInputPosition = Vec(11.0, 230.0);
		auto smoothInputPosition = Vec(43.0, 230.0);
		auto offsetInputPosition = Vec(11.0, 274.0);
		auto scaleInputPosition = Vec(43.0, 274.0);
		auto pitchInputPosition = Vec(11.0, 318.0);
		auto resetInputPosition = Vec(43.0, 318.0);
		auto phase0InputPosition = Vec(117.0, 104.0);
		auto phase1InputPosition = Vec(80.0, 104.0);
		auto phase2InputPosition = Vec(80.0, 322.0);
		auto phase3InputPosition = Vec(117.0, 322.0);

		auto phase0OutputPosition = Vec(117.0, 146.0);
		auto phase1OutputPosition = Vec(80.0, 146.0);
		auto phase2OutputPosition = Vec(80.0, 280.0);
		auto phase3OutputPosition = Vec(117.0, 280.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(frequencyParamPosition, module, FourFO::FREQUENCY_PARAM));
		{
			auto w = createParam<Knob16>(waveParamPosition, module, FourFO::WAVE_PARAM);
			auto k = dynamic_cast<SvgKnob*>(w);
			k->minAngle = 0.0;
			k->maxAngle = M_PI;
			k->speed = 3.0;
			addParam(w);
		}
		addParam(createParam<IndicatorButtonGreen9>(slowParamPosition, module, FourFO::SLOW_PARAM));
		addParam(createParam<Knob16>(samplePwmParamPosition, module, FourFO::SAMPLE_PWM_PARAM));
		addParam(createParam<Knob16>(smoothParamPosition, module, FourFO::SMOOTH_PARAM));
		addParam(createParam<Knob16>(offsetParamPosition, module, FourFO::OFFSET_PARAM));
		addParam(createParam<Knob16>(scaleParamPosition, module, FourFO::SCALE_PARAM));
		addPhaseParam(phase3ParamPosition, module, FourFO::PHASE3_PARAM, Phasor::phaseToRadians(basePhase3Offset));
		addPhaseParam(phase2ParamPosition, module, FourFO::PHASE2_PARAM, Phasor::phaseToRadians(basePhase2Offset));
		addPhaseParam(phase1ParamPosition, module, FourFO::PHASE1_PARAM, Phasor::phaseToRadians(basePhase1Offset));
		addPhaseParam(phase0ParamPosition, module, FourFO::PHASE0_PARAM, Phasor::phaseToRadians(basePhase0Offset));

		addInput(createInput<Port24>(samplePwmInputPosition, module, FourFO::SAMPLE_PWM_INPUT));
		addInput(createInput<Port24>(smoothInputPosition, module, FourFO::SMOOTH_INPUT));
		addInput(createInput<Port24>(offsetInputPosition, module, FourFO::OFFSET_INPUT));
		addInput(createInput<Port24>(scaleInputPosition, module, FourFO::SCALE_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, FourFO::PITCH_INPUT));
		addInput(createInput<Port24>(resetInputPosition, module, FourFO::RESET_INPUT));
		addInput(createInput<Port24>(phase0InputPosition, module, FourFO::PHASE0_INPUT));
		addInput(createInput<Port24>(phase1InputPosition, module, FourFO::PHASE1_INPUT));
		addInput(createInput<Port24>(phase2InputPosition, module, FourFO::PHASE2_INPUT));
		addInput(createInput<Port24>(phase3InputPosition, module, FourFO::PHASE3_INPUT));

		addOutput(createOutput<Port24>(phase0OutputPosition, module, FourFO::PHASE0_OUTPUT));
		addOutput(createOutput<Port24>(phase1OutputPosition, module, FourFO::PHASE1_OUTPUT));
		addOutput(createOutput<Port24>(phase2OutputPosition, module, FourFO::PHASE2_OUTPUT));
		addOutput(createOutput<Port24>(phase3OutputPosition, module, FourFO::PHASE3_OUTPUT));
	}

	void addPhaseParam(const Vec& position, Module* module, FourFO::ParamsIds paramId, float rotation) {
		auto w = createParam<Knob16>(position, module, paramId);
		auto k = dynamic_cast<SvgKnob*>(w);
		k->minAngle += 0.5 * M_PI - rotation;
		k->maxAngle += 0.5 * M_PI - rotation;
		addParam(w);
	}
};

Model* modelFourFO = createModel<FourFO, FourFOWidget>("Bogaudio-FourFO", "4FO", "Quadrature LFO", "LFO", "Random", "Polyphonic");
