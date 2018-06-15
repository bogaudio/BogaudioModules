
#include "Switch.hpp"

void bogaudio::Switch::onReset() {
	_trigger.reset();
}

void bogaudio::Switch::step() {
	_trigger.process(params[GATE_PARAM].value + inputs[GATE_INPUT].value);
	if (_trigger.isHigh()) {
		outputs[OUT1_OUTPUT].value = inputs[HIGH1_INPUT].value;
		outputs[OUT2_OUTPUT].value = inputs[HIGH2_INPUT].value;
	}
	else {
		outputs[OUT1_OUTPUT].value = inputs[LOW1_INPUT].value;
		outputs[OUT2_OUTPUT].value = inputs[LOW2_INPUT].value;
	}
}

struct SwitchWidget : ModuleWidget {
	static constexpr int hp = 3;

	SwitchWidget(bogaudio::Switch* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/Switch.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto gateParamPosition = Vec(13.5, 22.0);

		auto gateInputPosition = Vec(10.5, 44.0);
		auto high1InputPosition = Vec(10.5, 90.0);
		auto low1InputPosition = Vec(10.5, 126.0);
		auto high2InputPosition = Vec(10.5, 209.0);
		auto low2InputPosition = Vec(10.5, 245.0);

		auto out1OutputPosition = Vec(10.5, 164.0);
		auto out2OutputPosition = Vec(10.5, 283.0);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Button18>(gateParamPosition, module, bogaudio::Switch::GATE_PARAM, 0.0, 1.0, 0.0));

		addInput(Port::create<Port24>(gateInputPosition, Port::INPUT, module, bogaudio::Switch::GATE_INPUT));
		addInput(Port::create<Port24>(high1InputPosition, Port::INPUT, module, bogaudio::Switch::HIGH1_INPUT));
		addInput(Port::create<Port24>(low1InputPosition, Port::INPUT, module, bogaudio::Switch::LOW1_INPUT));
		addInput(Port::create<Port24>(high2InputPosition, Port::INPUT, module, bogaudio::Switch::HIGH2_INPUT));
		addInput(Port::create<Port24>(low2InputPosition, Port::INPUT, module, bogaudio::Switch::LOW2_INPUT));

		addOutput(Port::create<Port24>(out1OutputPosition, Port::OUTPUT, module, bogaudio::Switch::OUT1_OUTPUT));
		addOutput(Port::create<Port24>(out2OutputPosition, Port::OUTPUT, module, bogaudio::Switch::OUT2_OUTPUT));
	}
};

Model* modelSwitch = createModel<Switch, SwitchWidget>("Bogaudio-Switch", "Switch",  "signal switch", SWITCH_TAG);
