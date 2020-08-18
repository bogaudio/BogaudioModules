
#include <math.h>
#include <algorithm>

#include "Test2.hpp"
#include "pitch.hpp"

void Test2::reset() {
}

void Test2::processChannel(const ProcessArgs& args, int _c) {
	if (!outputs[OUT_OUTPUT].isConnected()) {
		return;
	}

#ifdef COMPLEX_BIQUAD
	_complexBiquad.setComplexParams(
		params[PARAM1B_PARAM].getValue(),
		params[PARAM2A_PARAM].getValue(),
		params[PARAM2B_PARAM].getValue() * M_PI,
		std::min(params[PARAM3A_PARAM].getValue(), 0.9f),
		params[PARAM3B_PARAM].getValue() * M_PI
	);
	float in = 0.0f;
	if (inputs[IN_INPUT].isConnected()) {
		in = inputs[IN_INPUT].getVoltage();
	}
	outputs[OUT_OUTPUT].setVoltage(_complexBiquad.next(in));

#elif MULTIPOLE
	++_steps;
	if (_steps >= maxSteps) {
		_steps = 0;

		_filter.setParams(
			params[PARAM2A_PARAM].getValue() <= 0.5f ? MultipoleFilter::LP_TYPE : MultipoleFilter::HP_TYPE,
			2 * clamp((int)(params[PARAM1B_PARAM].getValue() * (MultipoleFilter::maxPoles / 2)), 1, MultipoleFilter::maxPoles / 2),
			APP->engine->getSampleRate(),
			params[PARAM1A_PARAM].getValue() * APP->engine->getSampleRate() / 2.0f,
			params[PARAM2B_PARAM].getValue() * MultipoleFilter::maxRipple
		);
		// _filter.setParams(
		// 	MultipoleFilter::HP_TYPE,
		// 	4,
		// 	APP->engine->getSampleRate(),
		// 	0.1f * APP->engine->getSampleRate(),
		// 	0.1f
		// );
	}
	float in = 0.0f;
	if (inputs[IN_INPUT].isConnected()) {
		in = inputs[IN_INPUT].getVoltage();
	}
	outputs[OUT_OUTPUT].setVoltage(_filter.next(in));

#elif ADSR_ENVELOPE
  if (outputs[OUT_OUTPUT].isConnected()) {
		_adsr.setSampleRate(APP->engine->getSampleRate());
		if (inputs[IN_INPUT].isConnected()) {
			_trigger.process(inputs[IN_INPUT].getVoltage());
		}
		_adsr.setGate(_trigger.isHigh());
		_adsr.setAttack(powf(params[PARAM1A_PARAM].getValue(), 2.0f) * 10.0f);
		_adsr.setDecay(powf(params[PARAM1B_PARAM].getValue(), 2.0f) * 10.0f);
		_adsr.setSustain(params[PARAM2A_PARAM].getValue());
		_adsr.setRelease(powf(params[PARAM2B_PARAM].getValue(), 2.0f) * 10.0f);
		float attackShape = params[PARAM3A_PARAM].getValue();
		if (attackShape < 0.5f) {
			attackShape += 0.5f;
		}
		else {
			attackShape -= 0.5;
			attackShape *= 2.0f;
			attackShape += 1.0f;
		}
		float decayShape = params[PARAM3B_PARAM].getValue();
		if (decayShape < 0.5f) {
			decayShape += 0.5f;
		}
		else {
			decayShape -= 0.5;
			decayShape *= 2.0f;
			decayShape += 1.0f;
		}
		_adsr.setShapes(attackShape, decayShape, decayShape);
		outputs[OUT_OUTPUT].setVoltage(_adsr.next() * 10.0f);
	}

#elif LIMITER
	float shape = params[PARAM1A_PARAM].getValue() * 5.0f;
	float knee = params[PARAM2A_PARAM].getValue() * 10.0f;
	float limit = params[PARAM2B_PARAM].getValue() * 15.0f;
	float scale = params[PARAM1B_PARAM].getValue() * 2.0f + 1.0f;
	_limiter.setParams(shape, knee, limit, scale);
	outputs[OUT_OUTPUT].setVoltage(_limiter.next(inputs[IN_INPUT].getVoltage()));

#elif CHIRP
	float sr = APP->engine->getSampleRate();
	_phasor.setSampleRate(sr);

	float f1 = params[PARAM1A_PARAM].getValue();
	f1 *= f1;
	f1 *= sr * 0.5f * std::min(f1, 0.99f);
	f1 = std::max(10.0f, f1);
	float f2 = params[PARAM1B_PARAM].getValue();
	f2 *= f2;
	f2 *= sr * 0.5f * std::min(f2, 0.99f);
	f2 = std::max(10.0f, f2);
	float T = std::max(0.001f, params[PARAM2A_PARAM].getValue()); // seconds
	bool linear = params[PARAM2B_PARAM].getValue() < 0.5f;

	_time = _time * (float)(_time < T);

	// formulas from https://en.wikipedia.org/wiki/Chirp
	float out = 0.0f;
	if (linear) {
		float c = (f2 - f1) / T;
		float phase = 2.0f * M_PI * (0.5f * c * _time * _time + f1 * _time);
		// out = sinf(phase);
		out = _phasor.nextForPhase(Phasor::radiansToPhase(phase));
	}
	else {
		float k = powf(f2 / f1, 1.0f / T);
		float phase = 2.0f * M_PI * f1 * ((powf(k, _time) - 1.0f) / logf(k));
		// out = sinf(phase);
		out = _phasor.nextForPhase(Phasor::radiansToPhase(phase));
	}
	outputs[OUT_OUTPUT].setVoltage(out * 5.0f);

	_time += 1.0f / sr;

#elif CHIRP2
	float sr = APP->engine->getSampleRate();
	_chirp.setSampleRate(sr);

	float f1 = params[PARAM1A_PARAM].getValue();
	f1 *= f1;
	f1 *= sr * 0.5f * std::min(f1, 0.99f);
	f1 = std::max(10.0f, f1);
	float f2 = params[PARAM1B_PARAM].getValue();
	f2 *= f2;
	f2 *= sr * 0.5f * std::min(f2, 0.99f);
	f2 = std::max(10.0f, f2);
	float T = ChirpOscillator::minTimeSeconds + params[PARAM2A_PARAM].getValue() * (10.0f - ChirpOscillator::minTimeSeconds);
	bool linear = params[PARAM2B_PARAM].getValue() < 0.5f;
	_chirp.setParams(f1, f2, T, linear);
	outputs[OUT_OUTPUT].setVoltage(_chirp.next() * 5.0f);

	if (_chirp.isCycleComplete()) {
		_pulse.trigger(0.001f);
	}
	outputs[OUT2_OUTPUT].setVoltage(_pulse.process(1.0f / sr) * 5.0f);
#endif
}

// float Test2::oscillatorPitch1A() {
// 	if (inputs[CV1A_INPUT].isConnected()) {
// 		return cvToFrequency(inputs[CV1A_INPUT].getVoltage());
// 	}
// 	return 10000.0 * powf(params[PARAM1_PARAM].getValue(), 2.0);
// }


struct Test2Widget : BGModuleWidget {
	static constexpr int hp = 6;

	Test2Widget(Test2* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Test2");
		createScrews();

		// generated by svg_widgets.rb
		auto param1aParamPosition = Vec(9.5, 38.5);
		auto param2aParamPosition = Vec(9.5, 138.5);
		auto param3aParamPosition = Vec(9.5, 238.5);
		auto param1bParamPosition = Vec(54.5, 38.5);
		auto param2bParamPosition = Vec(54.5, 138.5);
		auto param3bParamPosition = Vec(54.5, 238.5);

		auto cv1aInputPosition = Vec(10.5, 78.0);
		auto cv2aInputPosition = Vec(10.5, 178.0);
		auto cv3aInputPosition = Vec(10.5, 278.0);
		auto cv1bInputPosition = Vec(55.5, 78.0);
		auto cv2bInputPosition = Vec(55.5, 178.0);
		auto cv3bInputPosition = Vec(55.5, 278.0);
		auto inInputPosition = Vec(10.5, 323.0);

		auto outOutputPosition = Vec(55.5, 323.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(param1aParamPosition, module, Test2::PARAM1A_PARAM));
		addParam(createParam<Knob26>(param2aParamPosition, module, Test2::PARAM2A_PARAM));
		addParam(createParam<Knob26>(param3aParamPosition, module, Test2::PARAM3A_PARAM));
		addParam(createParam<Knob26>(param1bParamPosition, module, Test2::PARAM1B_PARAM));
		addParam(createParam<Knob26>(param2bParamPosition, module, Test2::PARAM2B_PARAM));
		addParam(createParam<Knob26>(param3bParamPosition, module, Test2::PARAM3B_PARAM));

		addInput(createInput<Port24>(cv1aInputPosition, module, Test2::CV1A_INPUT));
		addInput(createInput<Port24>(cv2aInputPosition, module, Test2::CV2A_INPUT));
		addInput(createInput<Port24>(cv3aInputPosition, module, Test2::CV3A_INPUT));
		addInput(createInput<Port24>(cv1bInputPosition, module, Test2::CV1B_INPUT));
		addInput(createInput<Port24>(cv2bInputPosition, module, Test2::CV2B_INPUT));
		addInput(createInput<Port24>(cv3bInputPosition, module, Test2::CV3B_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, Test2::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Test2::OUT_OUTPUT));
		addOutput(createOutput<Port24>(Vec(outOutputPosition.x, outOutputPosition.y + 10), module, Test2::OUT2_OUTPUT));
	}
};

Model* modelTest2 = bogaudio::createModel<Test2, Test2Widget>("Bogaudio-Test2", "TEST2", "test2");
