
#include "AD.hpp"

void AD::onReset() {
	_trigger.reset();
	_eocPulseGen.process(10.0);
	_envelope.reset();
	_modulationStep = modulationSteps;
	_on = false;
}

void AD::onSampleRateChange() {
	float sr = engineGetSampleRate();
	_envelope.setSampleRate(sr);
	_attackSL.setParams(sr / (float)modulationSteps);
	_decaySL.setParams(sr / (float)modulationSteps);
	_modulationStep = modulationSteps;
}

void AD::step() {
	lights[LOOP_LIGHT].value = _loopMode = params[LOOP_PARAM].getValue() > 0.5f;
	lights[LINEAR_LIGHT].value = _linearMode = params[LINEAR_PARAM].getValue() > 0.5f;
	if (!(outputs[ENV_OUTPUT].isConnected() || outputs[EOC_OUTPUT].isConnected() || inputs[TRIGGER_INPUT].isConnected())) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		float attack = powf(params[ATTACK_PARAM].getValue(), 2.0f);
		if (inputs[ATTACK_INPUT].isConnected()) {
			attack *= clamp(inputs[ATTACK_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		_envelope.setAttack(_attackSL.next(attack * 10.f));

		float decay = powf(params[DECAY_PARAM].getValue(), 2.0f);
		if (inputs[DECAY_INPUT].isConnected()) {
			decay *= clamp(inputs[DECAY_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		_envelope.setDecay(_decaySL.next(decay * 10.f));

		_envelope.setLinearShape(_linearMode);
	}

	_trigger.process(inputs[TRIGGER_INPUT].getVoltage());
	if (!_on && (_trigger.isHigh() || (_loopMode && _envelope.isStage(ADSR::STOPPED_STAGE)))) {
		_on = true;
	}
	_envelope.setGate(_on);
	outputs[ENV_OUTPUT].setVoltage(_envelope.next() * 10.0f);
	if (_on && _envelope.isStage(ADSR::SUSTAIN_STAGE)) {
		_envelope.reset();
		_on = false;
		_eocPulseGen.trigger(0.001f);
	}
	outputs[EOC_OUTPUT].setVoltage(_eocPulseGen.process(engineGetSampleTime()) ? 5.0f : 0.0f);

	lights[ATTACK_LIGHT].value = _envelope.isStage(ADSR::ATTACK_STAGE);
	lights[DECAY_LIGHT].value = _envelope.isStage(ADSR::DECAY_STAGE);
}

struct ADWidget : ModuleWidget {
	static constexpr int hp = 3;

	ADWidget(AD* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/AD.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto attackParamPosition = Vec(8.0, 33.0);
		auto decayParamPosition = Vec(8.0, 90.0);
		auto loopParamPosition = Vec(33.5, 131.2);
		auto linearParamPosition = Vec(33.5, 145.7);

		auto triggerInputPosition = Vec(10.5, 163.5);
		auto attackInputPosition = Vec(10.5, 198.5);
		auto decayInputPosition = Vec(10.5, 233.5);

		auto envOutputPosition = Vec(10.5, 271.5);
		auto eocOutputPosition = Vec(10.5, 306.5);

		auto attackLightPosition = Vec(20.8, 65.0);
		auto decayLightPosition = Vec(20.8, 122.0);
		auto loopLightPosition = Vec(2.5, 132.5);
		auto linearLightPosition = Vec(2.5, 147.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob29>(attackParamPosition, module, AD::ATTACK_PARAM));
		addParam(createParam<Knob29>(decayParamPosition, module, AD::DECAY_PARAM));
		addParam(createParam<StatefulButton9>(loopParamPosition, module, AD::LOOP_PARAM));
		addParam(createParam<StatefulButton9>(linearParamPosition, module, AD::LINEAR_PARAM));

		addInput(createInput<Port24>(triggerInputPosition, module, AD::TRIGGER_INPUT));
		addInput(createInput<Port24>(attackInputPosition, module, AD::ATTACK_INPUT));
		addInput(createInput<Port24>(decayInputPosition, module, AD::DECAY_INPUT));

		addOutput(createOutput<Port24>(envOutputPosition, module, AD::ENV_OUTPUT));
		addOutput(createOutput<Port24>(eocOutputPosition, module, AD::EOC_OUTPUT));

		addChild(createLight<TinyLight<GreenLight>>(attackLightPosition, module, AD::ATTACK_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(decayLightPosition, module, AD::DECAY_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(loopLightPosition, module, AD::LOOP_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(linearLightPosition, module, AD::LINEAR_LIGHT));
	}
};

Model* modelAD = createModel<AD, ADWidget>("Bogaudio-AD");
