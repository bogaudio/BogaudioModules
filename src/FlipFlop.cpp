
#include "FlipFlop.hpp"

void FlipFlop::onReset() {
	_flipped1 = false;
	_flipped2 = false;
	_trigger1.reset();
	_resetTrigger1.reset();
	_trigger2.reset();
	_resetTrigger2.reset();
}

void FlipFlop::step() {
	channelStep(
		inputs[IN1_INPUT],
		inputs[RESET1_INPUT],
		outputs[A1_OUTPUT],
		outputs[B1_OUTPUT],
		_trigger1,
		_resetTrigger1,
		_flipped1
	);
	channelStep(
		inputs[IN2_INPUT],
		inputs[RESET2_INPUT],
		outputs[A2_OUTPUT],
		outputs[B2_OUTPUT],
		_trigger2,
		_resetTrigger2,
		_flipped2
	);
}

void FlipFlop::channelStep(
	Input& triggerInput,
	Input& resetInput,
	Output& aOutput,
	Output& bOutput,
	PositiveZeroCrossing& trigger,
	SchmittTrigger& resetTrigger,
	bool& flipped
) {
	bool triggered = trigger.next(triggerInput.value);
	resetTrigger.process(resetInput.value);
	if (resetTrigger.isHigh()) {
		flipped = false;
	}
	else if (triggered) {
		flipped = !flipped;
	}

	if (flipped) {
		aOutput.value = 0.0f;
		bOutput.value = 5.0f;
	}
	else {
		aOutput.value = 5.0f;
		bOutput.value = 0.0f;
	}
}

struct FlipFlopWidget : ModuleWidget {
	static constexpr int hp = 3;

	FlipFlopWidget(FlipFlop* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/FlipFlop.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto in1InputPosition = Vec(10.5, 21.0);
		auto reset1InputPosition = Vec(10.5, 56.0);
		auto in2InputPosition = Vec(10.5, 172.0);
		auto reset2InputPosition = Vec(10.5, 207.0);

		auto a1OutputPosition = Vec(10.5, 94.0);
		auto b1OutputPosition = Vec(10.5, 129.0);
		auto a2OutputPosition = Vec(10.5, 245.0);
		auto b2OutputPosition = Vec(10.5, 280.0);
		// end generated by svg_widgets.rb

		addInput(Port::create<Port24>(in1InputPosition, Port::INPUT, module, FlipFlop::IN1_INPUT));
		addInput(Port::create<Port24>(reset1InputPosition, Port::INPUT, module, FlipFlop::RESET1_INPUT));
		addInput(Port::create<Port24>(in2InputPosition, Port::INPUT, module, FlipFlop::IN2_INPUT));
		addInput(Port::create<Port24>(reset2InputPosition, Port::INPUT, module, FlipFlop::RESET2_INPUT));

		addOutput(Port::create<Port24>(a1OutputPosition, Port::OUTPUT, module, FlipFlop::A1_OUTPUT));
		addOutput(Port::create<Port24>(b1OutputPosition, Port::OUTPUT, module, FlipFlop::B1_OUTPUT));
		addOutput(Port::create<Port24>(a2OutputPosition, Port::OUTPUT, module, FlipFlop::A2_OUTPUT));
		addOutput(Port::create<Port24>(b2OutputPosition, Port::OUTPUT, module, FlipFlop::B2_OUTPUT));
	}
};

Model* modelFlipFlop = createModel<FlipFlop, FlipFlopWidget>("Bogaudio-FlipFlop", "FlipFlop",  "dual stateful logic", LOGIC_TAG, DUAL_TAG);
