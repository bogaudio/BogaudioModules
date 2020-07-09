
#include "Test.hpp"

void Test::reset() {
}

void Test::processChannel(const ProcessArgs& args, int _c) {
	if (!(outputs[OUT_OUTPUT].isConnected() || outputs[OUT2_OUTPUT].isConnected())) {
		return;
	}

#ifdef LPF
	if (!inputs[IN_INPUT].isConnected()) {
		return;
	}
	float sr = APP->engine->getSampleRate();
	_lpf.setParams(
		sr,
		0.49f * sr * clamp(params[PARAM1_PARAM].getValue(), 0.0f, 1.0f),
		std::max(10.0 * clamp(params[PARAM2_PARAM].getValue(), 0.0f, 1.0f), 0.1)
	);
	outputs[OUT_OUTPUT].setVoltage(_lpf.next(inputs[IN_INPUT].getVoltage()));

#elif LPFNOISE
	_lpf.setParams(
		APP->engine->getSampleRate(),
		22000.0 * clamp(params[PARAM1_PARAM].getValue(), 0.0f, 1.0f),
		0.717f
	);
	float noise = _noise.next();
	outputs[OUT_OUTPUT].setVoltage(_lpf.next(noise) * 10.0);;
	outputs[OUT2_OUTPUT].setVoltage(noise * 10.0);;

#elif SINE
	_sine.setSampleRate(APP->engine->getSampleRate());
	_sine.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].setVoltage(_sine.next() * 5.0f);

	_sine2.setSampleRate(APP->engine->getSampleRate());
	_sine2.setFrequency(oscillatorPitch());
	outputs[OUT2_OUTPUT].setVoltage(_sine2.next() * 5.0f);

#elif SQUARE
	_square.setSampleRate(APP->engine->getSampleRate());
	_square.setFrequency(oscillatorPitch());
	float pw = params[PARAM2_PARAM].getValue();
	if (inputs[CV2_INPUT].isConnected()) {
		pw += clamp(inputs[CV2_INPUT].getVoltage(), -5.0f, 5.0f) / 10.0f;
	}
	_square.setPulseWidth(pw);
	outputs[OUT_OUTPUT].setVoltage(_square.next() * 5.0f);

	_square2.setSampleRate(APP->engine->getSampleRate());
	_square2.setFrequency(oscillatorPitch());
	_square2.setPulseWidth(pw);
	_square2.setQuality(params[PARAM3_PARAM].getValue() * 200);
	outputs[OUT2_OUTPUT].setVoltage(_square2.next() * 5.0f);

#elif SAW
	_saw.setSampleRate(APP->engine->getSampleRate());
	_saw.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].setVoltage(_saw.next() * 5.0f);

	_saw2.setSampleRate(APP->engine->getSampleRate());
	_saw2.setFrequency(oscillatorPitch());
	_saw2.setQuality(params[PARAM2_PARAM].getValue() * 200);
	outputs[OUT2_OUTPUT].setVoltage(_saw2.next() * 5.0f);

#elif SATSAW
	float saturation = params[PARAM2_PARAM].getValue() * 10.0f;
	if (inputs[CV2_INPUT].isConnected()) {
		saturation *= clamp(inputs[CV2_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	_saw.setSampleRate(APP->engine->getSampleRate());
	_saw.setFrequency(oscillatorPitch());
	_saw.setSaturation(saturation);
	outputs[OUT_OUTPUT].setVoltage(_saw.next() * 5.0f);

	_saw2.setSampleRate(APP->engine->getSampleRate());
	_saw2.setFrequency(oscillatorPitch());
	_saw2.setSaturation(saturation);
	_saw2.setQuality(params[PARAM3_PARAM].getValue() * 200);
	outputs[OUT2_OUTPUT].setVoltage(_saw2.next() * 5.0f);

#elif TRIANGLE
	_triangle.setSampleRate(APP->engine->getSampleRate());
	_triangle.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].setVoltage(_triangle.next() * 5.0f);

#elif SAMPLED_TRIANGLE
	float sample = params[PARAM2_PARAM].getValue() * Phasor::maxSampleWidth;
	if (inputs[CV2_INPUT].isConnected()) {
		sample *= clamp(inputs[CV2_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	_triangle.setSampleRate(APP->engine->getSampleRate());
	_triangle.setFrequency(oscillatorPitch());
	_triangle.setSampleWidth(sample);
	outputs[OUT_OUTPUT].setVoltage(_triangle.next() * 5.0f);

	_triangle2.setSampleRate(APP->engine->getSampleRate());
	_triangle2.setFrequency(oscillatorPitch());
	float maxSampleSteps = (_triangle2._sampleRate / _triangle2._frequency) / 4.0f;
	_sampleSteps = clamp((int)((4.0f * sample) * maxSampleSteps), 1, (int)maxSampleSteps);
	++_sampleStep;
	if (_sampleStep >= _sampleSteps) {
		_sampleStep = 0;
		_sample = _triangle2.next() * 5.0f;
	}
	else {
		_triangle2.advancePhase();
	}
	outputs[OUT2_OUTPUT].setVoltage(_sample);

#elif SINEBANK
	_sineBank.setSampleRate(APP->engine->getSampleRate());
	_sineBank.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].setVoltage(_sineBank.next());

#elif OVERSAMPLING
	_saw1.setSampleRate(APP->engine->getSampleRate());
	_saw1.setFrequency(oscillatorPitch() / (float)OVERSAMPLEN);
	float buf[OVERSAMPLEN];
	for (int i = 0; i < OVERSAMPLEN; ++i) {
		buf[i] = _saw1.next();
	}
	outputs[OUT_OUTPUT].setVoltage(_rackDecimator.process(buf) * 5.0f);

	_saw2.setSampleRate(APP->engine->getSampleRate());
	_saw2.setFrequency(oscillatorPitch() / (float)OVERSAMPLEN);
	_lpf.setParams(
		APP->engine->getSampleRate(),
		APP->engine->getSampleRate() / 4.0f,
		0.03
	);
	_lpf2.setParams(
		APP->engine->getSampleRate(),
		APP->engine->getSampleRate() / 4.0f,
		0.03
	);
	float s = 0.0f;
	for (int i = 0; i < OVERSAMPLEN; ++i) {
		// s = _lpf2.next(_lpf.next(_saw2.next()));
		s = _lpf.next(_saw2.next());
		// s = _saw2.next();
	}
	outputs[OUT2_OUTPUT].setVoltage(s * 5.0);

#elif OVERSAMPLED_BL
	int quality = params[PARAM2_PARAM].getValue() * 100;
	const int maxOversample = 16;
	int oversample = params[PARAM3_PARAM].getValue() * maxOversample;

	_saw1.setSampleRate(APP->engine->getSampleRate());
	_saw1.setFrequency(oscillatorPitch());
	_saw1.setQuality(quality);
	outputs[OUT_OUTPUT].setVoltage(_saw1.next() * 5.0f);

	_saw2.setSampleRate(APP->engine->getSampleRate());
	_saw2.setQuality(quality);
	if (oversample < 2) {
		_saw2.setFrequency(oscillatorPitch());
		outputs[OUT2_OUTPUT].setVoltage(_saw2.next() * 5.0f);
	}
	else {
		_saw2.setFrequency(oscillatorPitch() / (float)oversample);
		_lpf.setParams(
			oversample * APP->engine->getSampleRate(),
			0.95f * APP->engine->getSampleRate(),
			0.03
		);
		float s = 0.0f;
		for (int i = 0; i < oversample; ++i) {
			s = _lpf.next(_saw2.next());
		}
		outputs[OUT2_OUTPUT].setVoltage(s * 5.0f);
	}

#elif ANTIALIASING
	const int quality = 12;
	const float oversampleThreshold = 0.06f;
	const float oversampleMixWidth = 100.0f;
	float sampleRate = APP->engine->getSampleRate();
	float frequency = oscillatorPitch(15000.0);

	float otf = oversampleThreshold * sampleRate;
	float mix, oMix;
	if (frequency > otf) {
		if (frequency > otf + oversampleMixWidth) {
			mix = 0.0f;
			oMix = 1.0f;
		}
		else {
			oMix = (frequency - otf) / oversampleMixWidth;
			mix = 1.0f - oMix;
		}
	}
	else {
		mix = 1.0f;
		oMix = 0.0f;
	}
	assert(mix >= 0.0f);
	assert(mix <= 1.0f);
	assert(oMix >= 0.0f);
	assert(oMix <= 1.0f);

	_phasor.setSampleRate(sampleRate);
	_phasor.setFrequency(frequency);
	_oversampledPhasor.setSampleRate(sampleRate);
	_oversampledPhasor.setFrequency(frequency / (float)OVERSAMPLEN);
	_saw.setSampleRate(sampleRate);
	_saw.setQuality(quality);
	_sawDecimator.setParams(sampleRate, OVERSAMPLEN);
	_square.setSampleRate(sampleRate);
	_square.setQuality(quality);
	_squareDecimator.setParams(sampleRate, OVERSAMPLEN);

	float out = 0.0f;
	float out2 = 0.0f;
	_phasor.advancePhase();
	if (mix > 0.0f) {
		_saw.setFrequency(frequency);
		_square.setFrequency(frequency);
		out += _saw.nextFromPhasor(_phasor) * mix;
		out2 += _square.nextFromPhasor(_phasor) * mix;
	}

	if (oMix > 0.0f) {
		float sawBuf[OVERSAMPLEN] {};
		float squareBuf[OVERSAMPLEN] {};
		_saw.setFrequency(frequency / (float)OVERSAMPLEN);
		_square.setFrequency(frequency / (float)OVERSAMPLEN);

		for (int i = 0; i < OVERSAMPLEN; ++i) {
			_oversampledPhasor.advancePhase();
			sawBuf[i] = _saw.nextFromPhasor(_oversampledPhasor);
			squareBuf[i] = _square.nextFromPhasor(_oversampledPhasor);
		}

		out += _sawDecimator.next(sawBuf) * oMix;
		// out += _sawRackDecimator.process(sawBuf) * oMix;

		out2 += _squareDecimator.next(squareBuf) * oMix;
		// out2 += _squareRackDecimator.process(squareBuf) * oMix;
	}
	else {
		for (int i = 0; i < OVERSAMPLEN; ++i) {
			_oversampledPhasor.advancePhase();
		}
	}

	outputs[OUT_OUTPUT].setVoltage(out * 5.0f);
	outputs[OUT2_OUTPUT].setVoltage(out2 * 5.0f);

#elif DECIMATORS
	const int quality = 12;
	float sampleRate = APP->engine->getSampleRate();
	float frequency = oscillatorPitch(15000.0);
	_saw.setSampleRate(sampleRate);
	_saw.setFrequency(frequency / (float)OVERSAMPLEN);
	_saw.setQuality(quality);
	_cicDecimator.setParams(sampleRate, OVERSAMPLEN);
	_lpfDecimator.setParams(sampleRate, OVERSAMPLEN);

	float buf[OVERSAMPLEN] {};
	for (int i = 0; i < OVERSAMPLEN; ++i) {
		buf[i] = _saw.next();
	}
	outputs[OUT_OUTPUT].setVoltage(_cicDecimator.next(buf) * 5.0f);
	// outputs[OUT2_OUTPUT].setVoltage(_lpfDecimator.next(buf) * 5.0f);
	outputs[OUT2_OUTPUT].setVoltage(_rackDecimator.process(buf) * 5.0f);

#elif INTERPOLATOR
	const int quality = 12;
	float sampleRate = APP->engine->getSampleRate();
	float frequency = oscillatorPitch();
	_saw.setSampleRate(sampleRate);
	_saw.setFrequency(frequency);
	_saw.setQuality(quality);
	_decimator.setParams(sampleRate, FACTOR);
	_interpolator.setParams(sampleRate, FACTOR);

	if (_steps >= FACTOR) {
		_steps = 0;
		for (int i = 0; i < FACTOR; ++i) {
			_rawSamples[i] = _saw.next();
		}
		_interpolator.next(_decimator.next(_rawSamples), _processedSamples);
	}
	outputs[OUT_OUTPUT].setVoltage(_processedSamples[_steps] * 5.0f);
	outputs[OUT2_OUTPUT].setVoltage(_rawSamples[_steps] * 5.0f);
	++_steps;

#elif FM
	const float amplitude = 5.0f;
	float baseHz = oscillatorPitch();
	float ratio = ratio2();
	float index = index3();
	float sampleRate = APP->engine->getSampleRate();
	if (_baseHz != baseHz || _ratio != ratio || _index != index || _sampleRate != sampleRate) {
		_baseHz = baseHz;
		_ratio = ratio;
		_index = index;
		_sampleRate = sampleRate;
		float modHz = _ratio * _baseHz;
		// printf("baseHz=%f ratio=%f modHz=%f index=%f\n", _baseHz, _ratio, modHz, _index);

		_modulator.setFrequency(modHz);
		_modulator.setSampleRate(_sampleRate);
		_carrier.setSampleRate(_sampleRate);

		_carrier2.setSampleRate(APP->engine->getSampleRate());
		_carrier2.setFrequency(baseHz);
		_modulator2.setSampleRate(APP->engine->getSampleRate());
		_modulator2.setFrequency(modHz);
	}

	// linear FM.
	float modHz = _ratio * _baseHz;
	_carrier.setFrequency(_baseHz + _index * _modulator.next() * modHz); // linear FM requires knowing the modulator's frequency.
	outputs[OUT_OUTPUT].setVoltage(_carrier.next() * amplitude);

	// PM for comparison - identical output.
	_carrier2.advancePhase();
	outputs[OUT2_OUTPUT].setVoltage(_carrier2.nextFromPhasor(_carrier2, Phasor::radiansToPhase(_index * _modulator2.next())) * amplitude);

#elif PM
	const float amplitude = 5.0f;
	float baseHz = oscillatorPitch();
	float modHz = ratio2() * baseHz;
	_carrier.setSampleRate(APP->engine->getSampleRate());
	_carrier.setFrequency(baseHz);
	_modulator.setSampleRate(APP->engine->getSampleRate());
	_modulator.setFrequency(modHz);
	_carrier.advancePhase();
	outputs[OUT_OUTPUT].setVoltage(_carrier.nextFromPhasor(_carrier, Phasor::radiansToPhase(index3() * _modulator.next())) * amplitude);

#elif FEEDBACK_PM
	_carrier.setSampleRate(APP->engine->getSampleRate());
	_carrier.setFrequency(oscillatorPitch());
	float feedback = params[PARAM2_PARAM].getValue();
	if (inputs[CV2_INPUT].isConnected()) {
		feedback *= clamp(inputs[CV2_INPUT].getVoltage(), 0.0f, 10.0f) / 10.0f;
	}
	_carrier.advancePhase();
	outputs[OUT_OUTPUT].setVoltage(_feedbackSample = _carrier.nextFromPhasor(_carrier, Phasor::radiansToPhase(feedback * _feedbackSample)) * 5.0f);

#elif EG
	_envelope.setSampleRate(APP->engine->getSampleRate());
	_envelope.setAttack(params[PARAM1_PARAM].getValue());
	_envelope.setDecay(params[PARAM2_PARAM].getValue());
	_envelope.setSustain(params[PARAM3_PARAM].getValue());
	_envelope.setRelease(params[PARAM2_PARAM].getValue());
	_envelope.setGate(inputs[CV1_INPUT].getVoltage() > 0.1f);
	outputs[OUT_OUTPUT].setVoltage(_envelope.next() * 10.0f);

#elif TABLES
	_sine.setSampleRate(APP->engine->getSampleRate());
	_sine.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].setVoltage(_sine.next() * 5.0f);

	_table.setSampleRate(APP->engine->getSampleRate());
	_table.setFrequency(oscillatorPitch());
	outputs[OUT2_OUTPUT].setVoltage(_table.next() * 5.0f);

#elif SLEW
	float ms = params[PARAM1_PARAM].getValue();
	if (inputs[CV1_INPUT].isConnected()) {
		ms *= clamp(inputs[CV2_INPUT].getVoltage(), 0.0f, 10.0f) / 10.0f;
	}
	ms = powf(ms, 2.0f);
	ms *= 10000.0f;
	_slew.setParams(APP->engine->getSampleRate(), ms);
	outputs[OUT_OUTPUT].setVoltage(_slew.next(inputs[IN_INPUT].getVoltage()));

	float shape = params[PARAM2_PARAM].getValue();
	if (inputs[CV2_INPUT].isConnected()) {
		shape *= clamp(inputs[CV2_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
	}
	if (shape < 0.5) {
		shape /= 0.5;
		shape = _slew2.minShape + shape * (1.0f - _slew2.minShape);
	}
	else {
		shape -= 0.5f;
		shape /= 0.5f;
		shape *= (_slew2.maxShape - 1.0f);
		shape += 1.0f;
	}
	_slew2.setParams(APP->engine->getSampleRate(), ms, shape);
	outputs[OUT2_OUTPUT].setVoltage(_slew2.next(inputs[IN_INPUT].getVoltage()));

#elif RMS
	float sensitivity = params[PARAM2_PARAM].getValue();
	if (inputs[CV2_INPUT].isConnected()) {
		sensitivity *= clamp(inputs[CV2_INPUT].getVoltage(), 0.0f, 10.0f) / 10.0f;
	}
	_rms.setSampleRate(APP->engine->getSampleRate());
	_rms.setSensitivity(sensitivity);
	outputs[OUT_OUTPUT].setVoltage(_rms.next(inputs[IN_INPUT].getVoltage()));
	_pef.setParams(APP->engine->getSampleRate(), sensitivity);
	outputs[OUT2_OUTPUT].setVoltage(_pef.next(inputs[IN_INPUT].getVoltage()));

#elif FASTRMS
	float sensitivity = params[PARAM2_PARAM].getValue();
	if (inputs[CV2_INPUT].isConnected()) {
		sensitivity *= clamp(inputs[CV2_INPUT].getVoltage(), 0.0f, 10.0f) / 10.0f;
	}
	_pure.setSampleRate(APP->engine->getSampleRate());
	_pure.setSensitivity(sensitivity);
	outputs[OUT_OUTPUT].setVoltage(_pure.next(inputs[IN_INPUT].getVoltage()));
	_fast.setSampleRate(APP->engine->getSampleRate());
	_fast.setSensitivity(sensitivity);
	outputs[OUT2_OUTPUT].setVoltage(_fast.next(inputs[IN_INPUT].getVoltage()));

#elif RAVG
	if (_reset.process(inputs[CV1_INPUT].getVoltage())) {
		_average.reset();
	}
	outputs[OUT_OUTPUT].setVoltage(_average.next(inputs[IN_INPUT].getVoltage()));

#elif SATURATOR
	float in = inputs[IN_INPUT].getVoltage();
	outputs[OUT_OUTPUT].setVoltage(_saturator.next(in));
	outputs[OUT2_OUTPUT].setVoltage(clamp(in, -Saturator::limit, Saturator::limit));

#elif BROWNIAN
	const float maxDiv = 1000.0f;
	float change = clamp(1.0f - params[PARAM1_PARAM].getValue(), 0.01f, 1.0f);
	float smooth = clamp(params[PARAM2_PARAM].getValue(), 0.01f, 1.0f);
	smooth *= smooth;
	_filter1.setParams(APP->engine->getSampleRate(), smooth * APP->engine->getSampleRate() * 0.49f);
	_filter2.setParams(APP->engine->getSampleRate(), smooth * APP->engine->getSampleRate() * 0.49f);

	_last1 = _last1 + _noise1.next() / (change * maxDiv);
	outputs[OUT_OUTPUT].setVoltage(_filter1.next(_last1));
	if (_last1 > 5.0f || _last1 < -5.0f) {
		_last1 = 0.0f;
	}

	_last2 = _last2 + _noise1.next() / (change * maxDiv);
	outputs[OUT2_OUTPUT].setVoltage(_filter2.next(_last2));
	if (_last2 > 5.0f || _last2 < -5.0f) {
		_last2 = 0.0f;
	}

	// // "leaky integrator"
	// float alpha = params[PARAM1_PARAM].getValue();
	// alpha = clamp(1.0f - alpha*alpha, 0.00001f, 1.0f);
	// float sample = 5.0f * _noise1.next();
	// _last1 = alpha*_last1 + (1.0f - alpha)*sample;
	// outputs[OUT_OUTPUT].setVoltage(_last1);

#elif RANDOMWALK
	float change = params[PARAM1_PARAM].getValue();
	change *= change;
	change *= change;
	_walk1.setParams(APP->engine->getSampleRate(), change);
	_walk2.setParams(APP->engine->getSampleRate(), change);
	outputs[OUT_OUTPUT].setVoltage(_walk1.next());
	outputs[OUT2_OUTPUT].setVoltage(_walk2.next());

#elif DCBLOCKER
	float in = inputs[IN_INPUT].getVoltage();
	outputs[OUT_OUTPUT].setVoltage(_filter.next(in));
	outputs[OUT2_OUTPUT].setVoltage(in);

#endif
}

float Test::oscillatorPitch(float max) {
	if (inputs[CV1_INPUT].isConnected()) {
		return cvToFrequency(inputs[CV1_INPUT].getVoltage());
	}
	return max * powf(params[PARAM1_PARAM].getValue(), 2.0);
}

float Test::oscillatorPitch2(float max) {
	if (inputs[CV2_INPUT].isConnected()) {
		return cvToFrequency(inputs[CV2_INPUT].getVoltage());
	}
	return max * powf(params[PARAM2_PARAM].getValue(), 2.0);
}

float Test::ratio2() {
	float ratio = (params[PARAM2_PARAM].getValue() * 2.0f) - 1.0f;
	if (inputs[CV2_INPUT].isConnected()) {
		ratio *= clamp(inputs[CV2_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
	}
	if (ratio < 0.0f) {
		return 1.0f + ratio;
	}
	return 1.0f + 9.0f*ratio;
}

float Test::index3() {
	float index = params[PARAM3_PARAM].getValue();
	if (inputs[CV3_INPUT].isConnected()) {
		index *= clamp(inputs[CV3_INPUT].getVoltage(), 0.0f, 10.0f) / 10.0f;
	}
	return index * 10.0f;
}


struct TestWidget : ModuleWidget {
	TestWidget(Test* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Test.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto param1ParamPosition = Vec(9.5, 13.5);
		auto param2ParamPosition = Vec(9.5, 98.5);
		auto param3ParamPosition = Vec(9.5, 183.5);

		auto cv1InputPosition = Vec(10.5, 53.0);
		auto cv2InputPosition = Vec(10.5, 138.0);
		auto cv3InputPosition = Vec(10.5, 223.0);
		auto inInputPosition = Vec(10.5, 268.0);

		auto outOutputPosition = Vec(10.5, 306.0);
		auto out2OutputPosition = Vec(20.5, 316.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(param1ParamPosition, module, Test::PARAM1_PARAM));
		addParam(createParam<Knob26>(param2ParamPosition, module, Test::PARAM2_PARAM));
		addParam(createParam<Knob26>(param3ParamPosition, module, Test::PARAM3_PARAM));

		addInput(createInput<Port24>(cv1InputPosition, module, Test::CV1_INPUT));
		addInput(createInput<Port24>(cv2InputPosition, module, Test::CV2_INPUT));
		addInput(createInput<Port24>(cv3InputPosition, module, Test::CV3_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, Test::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Test::OUT_OUTPUT));
		addOutput(createOutput<Port24>(out2OutputPosition, module, Test::OUT2_OUTPUT));
	}
};

Model* modelTest = bogaudio::createModel<Test, TestWidget>("Bogaudio-Test", "TEST", "test");
