
#include "MegaGate.hpp"
#include "dsp/pitch.hpp"

void MegaGate::Engine::reset() {
	trigger.reset();
	gatePulseGen.process(10.0);
}

void MegaGate::Engine::setSampleRate(float sr) {
	velocitySL.setParams(sr, 5.0f, 1.0f);
	tiltSL.setParams(sr, 10.0f, 2.0f);
	leftVcaSL.setParams(sr, 5.0f, 1.0f);
	rightVcaSL.setParams(sr, 5.0f, 1.0f);
	leftFinalHP.setParams(sr, MultimodeFilter::BUTTERWORTH_TYPE, 2, MultimodeFilter::HIGHPASS_MODE, 80.0f, MultimodeFilter::minQbw, MultimodeFilter::LINEAR_BANDWIDTH_MODE, MultimodeFilter::MINIMUM_DELAY_MODE);
	rightFinalHP.setParams(sr, MultimodeFilter::BUTTERWORTH_TYPE, 2, MultimodeFilter::HIGHPASS_MODE, 80.0f, MultimodeFilter::minQbw, MultimodeFilter::LINEAR_BANDWIDTH_MODE, MultimodeFilter::MINIMUM_DELAY_MODE);
}

void MegaGate::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void MegaGate::sampleRateChange() {
	_sampleRate = APP->engine->getSampleRate();
	_sampleTime = APP->engine->getSampleTime();
	for (int i = 0; i < _channels; ++i) {
		_engines[i]->setSampleRate(_sampleRate);
	}
}

bool MegaGate::active() {
	return outputs[LEFT_OUTPUT].isConnected() || outputs[RIGHT_OUTPUT].isConnected() || outputs[ENV_OUTPUT].isConnected();
}

int MegaGate::channels() {
	return inputs[GATE_INPUT].getChannels();
}

void MegaGate::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->reset();
	_engines[c]->setSampleRate(_sampleRate);
}

void MegaGate::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void MegaGate::modulateChannel(int c) {
	_engines[c]->slew.modulate(
		_sampleRate,
		params[RISE_PARAM],
		inputs[RISE_INPUT],
		300.0f * _timeScale,
		params[RISE_SHAPE_PARAM],
		params[FALL_PARAM],
		inputs[FALL_INPUT],
		1000.0f * _timeScale,
		params[FALL_SHAPE_PARAM],
		c
	);
}

void MegaGate::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	float in = inputs[GATE_INPUT].getPolyVoltage(c);
	if (e.trigger.process(in)) {
		float time = clamp(params[MINIMUM_GATE_PARAM].getValue(), 0.0f, 1.0f);
		if (inputs[MINIMUM_GATE_INPUT].isConnected()) {
			time *= clamp(inputs[MINIMUM_GATE_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		time *= time;
		time *= _timeScale;
		e.gateSeconds = time;

		e.gateElapsedSeconds = 0.0f;
		if (_gateToTrigger) {
			e.gateSeconds = std::max(0.01f, time);
		}
		else {
			e.gateSeconds = time;
		}
	}
	else {
		e.gateElapsedSeconds += _sampleTime;
	}

	float gate = 0.0f;
	if (e.gateElapsedSeconds < e.gateSeconds) {
		gate = 10.0f;
	}
	else if (!_gateToTrigger) {
		gate = in;
	}

	float velocity = 1.0f;
	if (inputs[VELOCITY_INPUT].isConnected()) {
		velocity = clamp(inputs[VELOCITY_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	velocity = e.velocitySL.next(velocity);
	e.velocityAmp.setLevel(_minVelocityDb + velocity * (_maxVelocityDb - _minVelocityDb));
	float env = e.velocityAmp.next(e.slew.next(gate));
	env /= 10.0f;

	float tilt = clamp(params[TILT_PARAM].getValue(), -1.0f, 1.0f);
	if (inputs[TILT_INPUT].isConnected()) {
		tilt *= clamp(inputs[TILT_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
	}
	tilt = e.tiltSL.next(tilt);

	float leftEnv = env;
	float rightEnv = env;
	if (tilt < 0.0f) {
		rightEnv *= 1.0f + tilt;
	}
	else {
		leftEnv *= 1.0f - tilt;
	}

	float lpfEnv = clamp(params[LPF_ENV_PARAM].getValue(), -1.0f, 1.0f);
	if (inputs[LPF_ENV_INPUT].isConnected()) {
		float cv = clamp(params[LPF_ENV_ATTENUATOR_PARAM].getValue(), -1.0f, 1.0f);
		cv *= clamp(inputs[LPF_ENV_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		lpfEnv = clamp(lpfEnv + cv, -1.0f, 1.0f);
	}
	float lpfBias = clamp(params[LPF_BIAS_PARAM].getValue(), -1.0f, 1.0f);
	if (inputs[LPF_BIAS_INPUT].isConnected()) {
		float cv = clamp(params[LPF_BIAS_ATTENUATOR_PARAM].getValue(), -1.0f, 1.0f);
		cv *= clamp(inputs[LPF_BIAS_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		lpfBias = clamp(lpfBias + cv, -1.0f, 1.0f);
	}
	lpfBias *= lpfBias;
	int lpfPoles = 1 + roundf(clamp(params[LPF_POLES_PARAM].getValue(), 0.0f, 3.0f));

	float hpfEnv = clamp(params[HPF_ENV_PARAM].getValue(), -1.0f, 1.0f);
	if (inputs[HPF_ENV_INPUT].isConnected()) {
		float cv = clamp(params[HPF_ENV_ATTENUATOR_PARAM].getValue(), -1.0f, 1.0f);
		cv *= clamp(inputs[HPF_ENV_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		hpfEnv = clamp(hpfEnv + cv, -1.0f, 1.0f);
	}
	float hpfBias = clamp(params[HPF_BIAS_PARAM].getValue(), -1.0f, 1.0f);
	if (inputs[HPF_BIAS_INPUT].isConnected()) {
		float cv = clamp(params[HPF_BIAS_ATTENUATOR_PARAM].getValue(), -1.0f, 1.0f);
		cv *= clamp(inputs[HPF_BIAS_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		hpfBias = clamp(hpfBias + cv, -1.0f, 1.0f);
	}
	hpfBias *= hpfBias;
	int hpfPoles = 1 + roundf(clamp(params[HPF_POLES_PARAM].getValue(), 0.0f, 3.0f));

	bool serial = params[FILTERS_SERIAL_PARAM].getValue() > 0.5f;
	bool linear = params[LINEAR_VCA_PARAM].getValue() > 0.5f;

	float vcaEnv = clamp(params[VCA_ENV_PARAM].getValue(), -1.0f, 1.0f);
	if (inputs[VCA_ENV_INPUT].isConnected()) {
		float cv = clamp(params[VCA_ENV_ATTENUATOR_PARAM].getValue(), -1.0f, 1.0f);
		cv *= clamp(inputs[VCA_ENV_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		vcaEnv = clamp(vcaEnv + cv, -1.0f, 1.0f);
	}
	float vcaBias = clamp(params[VCA_BIAS_PARAM].getValue(), 0.0f, 1.0f);
	if (inputs[VCA_ENV_INPUT].isConnected()) {
		float cv = clamp(params[VCA_BIAS_ATTENUATOR_PARAM].getValue(), -1.0f, 1.0f);
		cv *= clamp(inputs[VCA_BIAS_INPUT].getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
		vcaBias = clamp(vcaBias + cv, 0.0f, 1.0f);
	}

	{
		float f = clamp(lpfBias + leftEnv * lpfEnv, 0.0f, 1.0f);
		f *= maxFilterCutoff;
		f = std::max(f, MultimodeFilter4::minFrequency);
		e.leftLpf.setParams(
			_sampleRate,
			MultimodeFilter::BUTTERWORTH_TYPE,
			lpfPoles,
			MultimodeFilter::LOWPASS_MODE,
			f,
			0.0f
		);
	}
	{
		float f = clamp(hpfBias - leftEnv * hpfEnv, 0.0f, 1.0f);
		f *= maxFilterCutoff;
		f = std::max(f, MultimodeFilter4::minFrequency);
		e.leftHpf.setParams(
			_sampleRate,
			MultimodeFilter::BUTTERWORTH_TYPE,
			hpfPoles,
			MultimodeFilter::HIGHPASS_MODE,
			f,
			0.0f
		);
	}
	float level = clamp(vcaBias + leftEnv * vcaEnv, 0.0f, 1.0f);
	level = e.leftVcaSL.next(level);

	float leftIn = inputs[LEFT_INPUT].getPolyVoltage(c);
	float leftOut = 0.0f;
	if (serial) {
		leftOut = e.leftHpf.next(e.leftLpf.next(leftIn));
	}
	else {
		leftOut = e.leftLpf.next(leftIn) + e.leftHpf.next(leftIn);
	}
	leftOut = e.leftFinalHP.next(leftOut);
	if (linear) {
		leftOut *= level;
	}
	else {
		e.leftVca.setLevel(Amplifier::minDecibels * (1.0f - level));
		leftOut = e.leftVca.next(leftOut);
	}

	float rightOut = 0.0f;
	if (outputs[RIGHT_OUTPUT].isConnected()) {
		float rightIn = leftIn;
		if (inputs[RIGHT_INPUT].isConnected()) {
			rightIn = inputs[RIGHT_INPUT].getPolyVoltage(c);
		}

		{
			float f = clamp(lpfBias + rightEnv * lpfEnv, 0.0f, 1.0f);
			f *= maxFilterCutoff;
			f = std::max(f, MultimodeFilter4::minFrequency);
			e.rightLpf.setParams(
				_sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				lpfPoles,
				MultimodeFilter::LOWPASS_MODE,
				f,
				0.0f
			);
		}
		{
			float f = clamp(hpfBias - rightEnv * hpfEnv, 0.0f, 1.0f);
			f *= maxFilterCutoff;
			f = std::max(f, MultimodeFilter4::minFrequency);
			e.rightHpf.setParams(
				_sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				hpfPoles,
				MultimodeFilter::HIGHPASS_MODE,
				f,
				0.0f
			);
		}
		float level = clamp(vcaBias + rightEnv * vcaEnv, 0.0f, 1.0f);
		level = e.rightVcaSL.next(level);

		if (serial) {
			rightOut = e.rightHpf.next(e.rightLpf.next(rightIn));
		}
		else {
			rightOut = e.rightLpf.next(rightIn) + e.rightHpf.next(rightIn);
		}
		rightOut = e.rightFinalHP.next(rightOut);
		if (linear) {
			rightOut *= level;
		}
		else {
			e.rightVca.setLevel(Amplifier::minDecibels * (1.0f - level));
			rightOut = e.rightVca.next(rightOut);
		}
	}

	outputs[ENV_OUTPUT].setChannels(_channels);
	outputs[ENV_OUTPUT].setVoltage(env * 10.0f, c);
	outputs[LEFT_OUTPUT].setChannels(_channels);
	outputs[LEFT_OUTPUT].setVoltage(leftOut, c);
	outputs[RIGHT_OUTPUT].setChannels(_channels);
	outputs[RIGHT_OUTPUT].setVoltage(rightOut, c);
}

void MegaGate::processAlways(const ProcessArgs& args) {
	{
		int poles = params[LPF_POLES_PARAM].getValue();
		lights[LPF_POLES_1_LIGHT].value = poles == 0;
		lights[LPF_POLES_2_LIGHT].value = poles == 1;
		lights[LPF_POLES_3_LIGHT].value = poles == 2;
		lights[LPF_POLES_4_LIGHT].value = poles == 3;
	}
	{
		int poles = params[HPF_POLES_PARAM].getValue();
		lights[HPF_POLES_1_LIGHT].value = poles == 0;
		lights[HPF_POLES_2_LIGHT].value = poles == 1;
		lights[HPF_POLES_3_LIGHT].value = poles == 2;
		lights[HPF_POLES_4_LIGHT].value = poles == 3;
	}
}

struct MegaGateWidget : LPGEnvBaseWidget {
	static constexpr int hp = 18;

	MegaGateWidget(MegaGate* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "MegaGate");
		createScrews();

		// generated by svg_widgets.rb
		auto riseParamPosition = Vec(17.5, 41.5);
		auto riseShapeParamPosition = Vec(24.0, 89.0);
		auto fallParamPosition = Vec(72.5, 41.5);
		auto fallShapeParamPosition = Vec(79.0, 89.0);
		auto minimumGateParamPosition = Vec(17.5, 176.0);
		auto tiltParamPosition = Vec(72.5, 176.0);
		auto gateToTriggerParamPosition = Vec(20.0, 255.0);
		auto times10xParamPosition = Vec(20.0, 269.0);
		auto filtersSerialParamPosition = Vec(90.0, 255.0);
		auto linearVcaParamPosition = Vec(90.0, 269.0);
		auto lpfEnvParamPosition = Vec(136.5, 42.0);
		auto lpfEnvAttenuatorParamPosition = Vec(143.0, 81.0);
		auto lpfBiasParamPosition = Vec(186.5, 42.0);
		auto lpfBiasAttenuatorParamPosition = Vec(193.0, 81.0);
		auto lpfPolesParamPosition = Vec(241.0, 87.0);
		auto hpfEnvParamPosition = Vec(136.5, 157.0);
		auto hpfEnvAttenuatorParamPosition = Vec(143.0, 196.0);
		auto hpfBiasParamPosition = Vec(186.5, 157.0);
		auto hpfBiasAttenuatorParamPosition = Vec(193.0, 196.0);
		auto hpfPolesParamPosition = Vec(239.0, 202.0);
		auto vcaEnvParamPosition = Vec(163.5, 271.0);
		auto vcaEnvAttenuatorParamPosition = Vec(170.0, 310.0);
		auto vcaBiasParamPosition = Vec(213.5, 271.0);
		auto vcaBiasAttenuatorParamPosition = Vec(220.0, 310.0);

		auto riseInputPosition = Vec(20.5, 118.0);
		auto fallInputPosition = Vec(75.5, 118.0);
		auto minimumGateInputPosition = Vec(20.5, 220.0);
		auto tiltInputPosition = Vec(75.5, 220.0);
		auto gateInputPosition = Vec(8.5, 287.0);
		auto velocityInputPosition = Vec(38.5, 287.0);
		auto leftInputPosition = Vec(68.5, 287.0);
		auto rightInputPosition = Vec(98.5, 287.0);
		auto lpfEnvInputPosition = Vec(139.0, 106.0);
		auto lpfBiasInputPosition = Vec(189.0, 106.0);
		auto hpfEnvInputPosition = Vec(139.0, 221.0);
		auto hpfBiasInputPosition = Vec(189.0, 221.0);
		auto vcaEnvInputPosition = Vec(166.0, 335.0);
		auto vcaBiasInputPosition = Vec(216.0, 335.0);

		auto envOutputPosition = Vec(38.5, 324.0);
		auto leftOutputPosition = Vec(68.5, 324.0);
		auto rightOutputPosition = Vec(98.5, 324.0);

		auto lpfPoles1LightPosition = Vec(239.0, 40.0);
		auto lpfPoles2LightPosition = Vec(239.0, 52.0);
		auto lpfPoles3LightPosition = Vec(239.0, 64.0);
		auto lpfPoles4LightPosition = Vec(239.0, 76.0);
		auto hpfPoles1LightPosition = Vec(237.0, 155.0);
		auto hpfPoles2LightPosition = Vec(237.0, 167.0);
		auto hpfPoles3LightPosition = Vec(237.0, 179.0);
		auto hpfPoles4LightPosition = Vec(237.0, 191.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob29>(riseParamPosition, module, MegaGate::RISE_PARAM));
		addParam(createParam<Knob16>(riseShapeParamPosition, module, MegaGate::RISE_SHAPE_PARAM));
		addParam(createParam<Knob29>(fallParamPosition, module, MegaGate::FALL_PARAM));
		addParam(createParam<Knob16>(fallShapeParamPosition, module, MegaGate::FALL_SHAPE_PARAM));
		addParam(createParam<Knob29>(minimumGateParamPosition, module, MegaGate::MINIMUM_GATE_PARAM));
		addParam(createParam<Knob29>(tiltParamPosition, module, MegaGate::TILT_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(gateToTriggerParamPosition, module, MegaGate::GATE_TO_TRIGGER_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(times10xParamPosition, module, MegaGate::TIMES_10X_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(filtersSerialParamPosition, module, MegaGate::FILTERS_SERIAL_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(linearVcaParamPosition, module, MegaGate::LINEAR_VCA_PARAM));
		addParam(createParam<Knob29>(lpfEnvParamPosition, module, MegaGate::LPF_ENV_PARAM));
		addParam(createParam<Knob16>(lpfEnvAttenuatorParamPosition, module, MegaGate::LPF_ENV_ATTENUATOR_PARAM));
		addParam(createParam<Knob29>(lpfBiasParamPosition, module, MegaGate::LPF_BIAS_PARAM));
		addParam(createParam<Knob16>(lpfBiasAttenuatorParamPosition, module, MegaGate::LPF_BIAS_ATTENUATOR_PARAM));
		addParam(createParam<StatefulButton9>(lpfPolesParamPosition, module, MegaGate::LPF_POLES_PARAM));
		addParam(createParam<Knob29>(hpfEnvParamPosition, module, MegaGate::HPF_ENV_PARAM));
		addParam(createParam<Knob16>(hpfEnvAttenuatorParamPosition, module, MegaGate::HPF_ENV_ATTENUATOR_PARAM));
		addParam(createParam<Knob29>(hpfBiasParamPosition, module, MegaGate::HPF_BIAS_PARAM));
		addParam(createParam<Knob16>(hpfBiasAttenuatorParamPosition, module, MegaGate::HPF_BIAS_ATTENUATOR_PARAM));
		addParam(createParam<StatefulButton9>(hpfPolesParamPosition, module, MegaGate::HPF_POLES_PARAM));
		addParam(createParam<Knob29>(vcaEnvParamPosition, module, MegaGate::VCA_ENV_PARAM));
		addParam(createParam<Knob16>(vcaEnvAttenuatorParamPosition, module, MegaGate::VCA_ENV_ATTENUATOR_PARAM));
		addParam(createParam<Knob29>(vcaBiasParamPosition, module, MegaGate::VCA_BIAS_PARAM));
		addParam(createParam<Knob16>(vcaBiasAttenuatorParamPosition, module, MegaGate::VCA_BIAS_ATTENUATOR_PARAM));

		addInput(createInput<Port24>(riseInputPosition, module, MegaGate::RISE_INPUT));
		addInput(createInput<Port24>(fallInputPosition, module, MegaGate::FALL_INPUT));
		addInput(createInput<Port24>(minimumGateInputPosition, module, MegaGate::MINIMUM_GATE_INPUT));
		addInput(createInput<Port24>(tiltInputPosition, module, MegaGate::TILT_INPUT));
		addInput(createInput<Port24>(gateInputPosition, module, MegaGate::GATE_INPUT));
		addInput(createInput<Port24>(velocityInputPosition, module, MegaGate::VELOCITY_INPUT));
		addInput(createInput<Port24>(leftInputPosition, module, MegaGate::LEFT_INPUT));
		addInput(createInput<Port24>(rightInputPosition, module, MegaGate::RIGHT_INPUT));
		addInput(createInput<Port24>(lpfEnvInputPosition, module, MegaGate::LPF_ENV_INPUT));
		addInput(createInput<Port24>(lpfBiasInputPosition, module, MegaGate::LPF_BIAS_INPUT));
		addInput(createInput<Port24>(hpfEnvInputPosition, module, MegaGate::HPF_ENV_INPUT));
		addInput(createInput<Port24>(hpfBiasInputPosition, module, MegaGate::HPF_BIAS_INPUT));
		addInput(createInput<Port24>(vcaEnvInputPosition, module, MegaGate::VCA_ENV_INPUT));
		addInput(createInput<Port24>(vcaBiasInputPosition, module, MegaGate::VCA_BIAS_INPUT));

		addOutput(createOutput<Port24>(envOutputPosition, module, MegaGate::ENV_OUTPUT));
		addOutput(createOutput<Port24>(leftOutputPosition, module, MegaGate::LEFT_OUTPUT));
		addOutput(createOutput<Port24>(rightOutputPosition, module, MegaGate::RIGHT_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(lpfPoles1LightPosition, module, MegaGate::LPF_POLES_1_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(lpfPoles2LightPosition, module, MegaGate::LPF_POLES_2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(lpfPoles3LightPosition, module, MegaGate::LPF_POLES_3_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(lpfPoles4LightPosition, module, MegaGate::LPF_POLES_4_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(hpfPoles1LightPosition, module, MegaGate::HPF_POLES_1_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(hpfPoles2LightPosition, module, MegaGate::HPF_POLES_2_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(hpfPoles3LightPosition, module, MegaGate::HPF_POLES_3_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(hpfPoles4LightPosition, module, MegaGate::HPF_POLES_4_LIGHT));
	}
};

Model* modelMegaGate = createModel<MegaGate, MegaGateWidget>("Bogaudio-MegaGate", "MEGAGATE", "Low- and high-pass gate", "Low-pass gate", "Dual", "Polyphonic");
