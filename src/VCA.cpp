
#include "VCA.hpp"

void VCA::step() {
	channelStep(inputs[IN1_INPUT], outputs[OUT1_OUTPUT], params[LEVEL1_PARAM], inputs[CV1_INPUT], _amplifier1);
	channelStep(inputs[IN2_INPUT], outputs[OUT2_OUTPUT], params[LEVEL2_PARAM], inputs[CV2_INPUT], _amplifier2);
}

void VCA::channelStep(Input& input, Output& output, Param& knob, Input& cv, Amplifier& amplifier) {
	if (input.active && output.active) {
		float level = knob.value;
		if (cv.active) {
			level *= clamp(cv.value, 0.0f, 10.0f) / 10.0f;
		}
		level = 1.0f - level;
		level *= Amplifier::minDecibels;
		amplifier.setLevel(level);
		output.value = amplifier.next(input.value);
	}
}

struct VCAWidget : ModuleWidget {
	VCAWidget(VCA* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/VCA.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto level1ParamPosition = Vec(9.5, 28.5);
		auto level2ParamPosition = Vec(9.5, 189.5);

		auto cv1InputPosition = Vec(10.5, 66.0);
		auto in1InputPosition = Vec(10.5, 101.0);
		auto cv2InputPosition = Vec(10.5, 227.0);
		auto in2InputPosition = Vec(10.5, 262.0);

		auto out1OutputPosition = Vec(10.5, 139.0);
		auto out2OutputPosition = Vec(10.5, 300.0);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob26>(level1ParamPosition, module, VCA::LEVEL1_PARAM, 0.0, 1.0, 0.5));
		addParam(ParamWidget::create<Knob26>(level2ParamPosition, module, VCA::LEVEL2_PARAM, 0.0, 1.0, 0.5));

		addInput(Port::create<Port24>(cv1InputPosition, Port::INPUT, module, VCA::CV1_INPUT));
		addInput(Port::create<Port24>(in1InputPosition, Port::INPUT, module, VCA::IN1_INPUT));
		addInput(Port::create<Port24>(cv2InputPosition, Port::INPUT, module, VCA::CV2_INPUT));
		addInput(Port::create<Port24>(in2InputPosition, Port::INPUT, module, VCA::IN2_INPUT));

		addOutput(Port::create<Port24>(out1OutputPosition, Port::OUTPUT, module, VCA::OUT1_OUTPUT));
		addOutput(Port::create<Port24>(out2OutputPosition, Port::OUTPUT, module, VCA::OUT2_OUTPUT));
	}
};

Model* modelVCA = Model::create<VCA, VCAWidget>("Bogaudio", "Bogaudio-VCA", "VCA", AMPLIFIER_TAG, ATTENUATOR_TAG, DUAL_TAG, UTILITY_TAG);
