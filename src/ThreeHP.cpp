
#include "bogaudio.hpp"

struct ThreeHP : Module {
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

	ThreeHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void ThreeHP::onReset() {
}

void ThreeHP::step() {
}


struct ThreeHPWidget : ModuleWidget {
	ThreeHPWidget(ThreeHP* module);
};

ThreeHPWidget::ThreeHPWidget(ThreeHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/ThreeHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));
}


Model* modelThreeHP = Model::create<ThreeHP, ThreeHPWidget>("Bogaudio", "Bogaudio-ThreeHP", "3HP");
