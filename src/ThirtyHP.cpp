
#include "bogaudio.hpp"

struct ThirtyHP : Module {
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

	ThirtyHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void ThirtyHP::onReset() {
}

void ThirtyHP::step() {
}


struct ThirtyHPWidget : ModuleWidget {
	ThirtyHPWidget(ThirtyHP* module);
};

ThirtyHPWidget::ThirtyHPWidget(ThirtyHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 30, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/ThirtyHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));
}


Model* modelThirtyHP = Model::create<ThirtyHP, ThirtyHPWidget>("Bogaudio", "Bogaudio-ThirtyHP", "30HP");
