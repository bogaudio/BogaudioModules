
#include "bogaudio.hpp"

struct SixHP : Module {
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

	SixHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void SixHP::onReset() {
}

void SixHP::step() {
}


struct SixHPWidget : ModuleWidget {
	SixHPWidget(SixHP* module);
};

SixHPWidget::SixHPWidget(SixHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 6, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/SixHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));
}


Model* modelSixHP = Model::create<SixHP, SixHPWidget>("Bogaudio", "Bogaudio-SixHP", "6HP");
