
#include "bogaudio.hpp"

struct TwentyFiveHP : Module {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	TwentyFiveHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void TwentyFiveHP::onReset() {
}

void TwentyFiveHP::step() {
}


struct TwentyFiveHPWidget : ModuleWidget {
	TwentyFiveHPWidget(TwentyFiveHP* module);
};

TwentyFiveHPWidget::TwentyFiveHPWidget(TwentyFiveHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 25, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/TwentyFiveHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));
}


Model* modelTwentyFiveHP = Model::create<TwentyFiveHP, TwentyFiveHPWidget>("Bogaudio", "Bogaudio-TwentyFiveHP", "25HP");
