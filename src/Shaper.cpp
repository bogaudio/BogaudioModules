
#include "Shaper.hpp"

void Shaper::reset() {
	for (int c = 0; c < _channels; ++c) {
		_core[c]->reset();
	}
}

int Shaper::channels() {
	return std::max(1, std::max(inputs[SIGNAL_INPUT].getChannels(), inputs[TRIGGER_INPUT].getChannels()));
}

void Shaper::addEngine(int c) {
	_core[c] = new ShaperCore(
		params[ATTACK_PARAM],
		params[ON_PARAM],
		params[DECAY_PARAM],
		params[OFF_PARAM],
		params[ENV_PARAM],
		params[SIGNAL_PARAM],
		params[TRIGGER_PARAM],
		params[SPEED_PARAM],
		params[LOOP_PARAM],

		inputs[SIGNAL_INPUT],
		inputs[TRIGGER_INPUT],
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,

		outputs[SIGNAL_OUTPUT],
		outputs[ENV_OUTPUT],
		outputs[INV_OUTPUT],
		outputs[TRIGGER_OUTPUT],
		NULL,
		NULL,
		NULL,
		NULL,

		_attackLights,
		_onLights,
		_decayLights,
		_offLights,

		_triggerOnLoad,
		_shouldTriggerOnLoad
	);
}

void Shaper::removeEngine(int c) {
	delete _core[c];
	_core[c] = NULL;
}

void Shaper::processChannel(const ProcessArgs& args, int c) {
	_core[c]->step(c, _channels);
}

void Shaper::postProcess(const ProcessArgs& args) {
	float attackSum = 0.0f;
	float onSum = 0.0f;
	float decaySum = 0.0f;
	float offSum = 0.0f;
	for (int c = 0; c < _channels; ++c) {
		attackSum += _attackLights[c];
		onSum += _onLights[c];
		decaySum += _decayLights[c];
		offSum += _offLights[c];
	}
	lights[ATTACK_LIGHT].value = attackSum / (float)_channels;
	lights[ON_LIGHT].value = onSum / (float)_channels;
	lights[DECAY_LIGHT].value = decaySum / (float)_channels;
	lights[OFF_LIGHT].value = offSum / (float)_channels;
}

bool Shaper::shouldTriggerOnNextLoad() {
	for (int c = 0; c < _channels; ++c) {
		if (_core[c]->_stage != _core[c]->STOPPED_STAGE) {
			return true;
		}
	}
	return false;
}

struct ShaperWidget : TriggerOnLoadModuleWidget {
	static constexpr int hp = 10;

	ShaperWidget(Shaper* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Shaper.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto attackParamPosition = Vec(29.08, 33.08);
		auto triggerParamPosition = Vec(89.04, 43.04);
		auto onParamPosition = Vec(29.08, 89.08);
		auto speedParamPosition = Vec(119.0, 96.0);
		auto decayParamPosition = Vec(29.08, 145.08);
		auto loopParamPosition = Vec(119.0, 152.0);
		auto offParamPosition = Vec(29.08, 201.08);
		auto envParamPosition = Vec(82.38, 257.08);
		auto signalParamPosition = Vec(82.38, 313.08);

		auto triggerInputPosition = Vec(114.0, 40.0);
		auto signalInputPosition = Vec(11.5, 320.0);

		auto triggerOutputPosition = Vec(114.0, 208.0);
		auto envOutputPosition = Vec(11.5, 264.0);
		auto invOutputPosition = Vec(40.5, 264.0);
		auto signalOutputPosition = Vec(40.5, 320.0);

		auto attackLightPosition = Vec(12.0, 80.0);
		auto onLightPosition = Vec(12.0, 121.0);
		auto decayLightPosition = Vec(12.0, 189.0);
		auto offLightPosition = Vec(12.0, 237.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(attackParamPosition, module, Shaper::ATTACK_PARAM));
		addParam(createParam<Knob38>(onParamPosition, module, Shaper::ON_PARAM));
		addParam(createParam<Knob38>(decayParamPosition, module, Shaper::DECAY_PARAM));
		addParam(createParam<Knob38>(offParamPosition, module, Shaper::OFF_PARAM));
		addParam(createParam<Knob38>(envParamPosition, module, Shaper::ENV_PARAM));
		addParam(createParam<Knob38>(signalParamPosition, module, Shaper::SIGNAL_PARAM));

		addParam(createParam<Button18>(triggerParamPosition, module, Shaper::TRIGGER_PARAM));
		addInput(createInput<Port24>(triggerInputPosition, module, Shaper::TRIGGER_INPUT));

		addParam(createParam<SliderSwitch2State14>(speedParamPosition, module, Shaper::SPEED_PARAM));
		addParam(createParam<SliderSwitch2State14>(loopParamPosition, module, Shaper::LOOP_PARAM));
		addOutput(createOutput<Port24>(triggerOutputPosition, module, Shaper::TRIGGER_OUTPUT));

		addOutput(createOutput<Port24>(envOutputPosition, module, Shaper::ENV_OUTPUT));
		addOutput(createOutput<Port24>(invOutputPosition, module, Shaper::INV_OUTPUT));

		addInput(createInput<Port24>(signalInputPosition, module, Shaper::SIGNAL_INPUT));
		addOutput(createOutput<Port24>(signalOutputPosition, module, Shaper::SIGNAL_OUTPUT));

		addChild(createLight<TinyLight<GreenLight>>(attackLightPosition, module, Shaper::ATTACK_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(onLightPosition, module, Shaper::ON_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(decayLightPosition, module, Shaper::DECAY_LIGHT));
		addChild(createLight<TinyLight<GreenLight>>(offLightPosition, module, Shaper::OFF_LIGHT));
	}
};

Model* modelShaper = bogaudio::createModel<Shaper, ShaperWidget>("Bogaudio-Shaper", "SHAPER", "Retro-inspired envelope generator and amplifier", "Envelope generator", "VCA", "Polyphonic");
