
#include "Sums.hpp"

void Sums::step() {
	float a = inputs[A_INPUT].getVoltage();
	float b = inputs[B_INPUT].getVoltage();
	if (_disableOutputLimit) {
		outputs[SUM_OUTPUT].value = a + b,
		outputs[DIFFERENCE_OUTPUT].setVoltage(a - b);
		outputs[MAX_OUTPUT].setVoltage(std::max(a, b));
		outputs[MIN_OUTPUT].setVoltage(std::min(a, b));

		if (inputs[NEGATE_INPUT].isConnected()) {
			outputs[NEGATE_OUTPUT].setVoltage(-inputs[NEGATE_INPUT].getVoltage());
		}
		else {
			outputs[NEGATE_OUTPUT].setVoltage(0.0f);
		}
	}
	else {
		outputs[SUM_OUTPUT].setVoltage(clamp(a + b, -12.0f, 12.0f));
		outputs[DIFFERENCE_OUTPUT].setVoltage(clamp(a - b, -12.0f, 12.0f));
		outputs[MAX_OUTPUT].setVoltage(clamp(std::max(a, b), -12.0f, 12.0f));
		outputs[MIN_OUTPUT].setVoltage(clamp(std::min(a, b), -12.0f, 12.0f));

		if (inputs[NEGATE_INPUT].isConnected()) {
			outputs[NEGATE_OUTPUT].setVoltage(clamp(-inputs[NEGATE_INPUT].getVoltage(), -12.0f, 12.0f));
		}
		else {
			outputs[NEGATE_OUTPUT].setVoltage(0.0f);
		}
	}
}

struct SumsWidget : DisableOutputLimitModuleWidget {
	static constexpr int hp = 3;

	SumsWidget(Sums* module) : DisableOutputLimitModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/Sums.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto aInputPosition = Vec(10.5, 23.0);
		auto bInputPosition = Vec(10.5, 53.0);
		auto negateInputPosition = Vec(10.5, 262.0);

		auto sumOutputPosition = Vec(10.5, 86.0);
		auto differenceOutputPosition = Vec(10.5, 126.0);
		auto maxOutputPosition = Vec(10.5, 166.0);
		auto minOutputPosition = Vec(10.5, 206.0);
		auto negateOutputPosition = Vec(10.5, 295.0);
		// end generated by svg_widgets.rb

		addInput(createInput<Port24>(aInputPosition, module, Sums::A_INPUT));
		addInput(createInput<Port24>(bInputPosition, module, Sums::B_INPUT));
		addInput(createInput<Port24>(negateInputPosition, module, Sums::NEGATE_INPUT));

		addOutput(createOutput<Port24>(sumOutputPosition, module, Sums::SUM_OUTPUT));
		addOutput(createOutput<Port24>(differenceOutputPosition, module, Sums::DIFFERENCE_OUTPUT));
		addOutput(createOutput<Port24>(maxOutputPosition, module, Sums::MAX_OUTPUT));
		addOutput(createOutput<Port24>(minOutputPosition, module, Sums::MIN_OUTPUT));
		addOutput(createOutput<Port24>(negateOutputPosition, module, Sums::NEGATE_OUTPUT));
	}
};

Model* modelSums = createModel<Sums, SumsWidget>("Bogaudio-Sums", "Sums",  "arithmetic logic", LOGIC_TAG);
