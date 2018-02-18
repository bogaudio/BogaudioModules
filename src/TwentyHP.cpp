
#include "bogaudio.hpp"

struct TwentyHP : Module {
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

	TwentyHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void TwentyHP::onReset() {
}

void TwentyHP::step() {
}


struct TwentyHPWidget : ModuleWidget {
	TwentyHPWidget(TwentyHP* module);
};

TwentyHPWidget::TwentyHPWidget(TwentyHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 20, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/TwentyHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));
}


Model* modelTwentyHP = Model::create<TwentyHP, TwentyHPWidget>("Bogaudio", "Bogaudio-TwentyHP", "20HP");
