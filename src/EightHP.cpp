
#include "bogaudio.hpp"

struct EightHP : Module {
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

	EightHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void EightHP::onReset() {
}

void EightHP::step() {
}


struct EightHPWidget : ModuleWidget {
	EightHPWidget(EightHP* module);
};

EightHPWidget::EightHPWidget(EightHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/EightHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));
}


Model* modelEightHP = Model::create<EightHP, EightHPWidget>("Bogaudio", "Bogaudio-EightHP", "8HP");
