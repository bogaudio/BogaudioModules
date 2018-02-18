
#include "bogaudio.hpp"

struct EighteenHP : Module {
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

	EighteenHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void EighteenHP::onReset() {
}

void EighteenHP::step() {
}


struct EighteenHPWidget : ModuleWidget {
	EighteenHPWidget(EighteenHP* module);
};

EighteenHPWidget::EighteenHPWidget(EighteenHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 18, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/EighteenHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));
}


Model* modelEighteenHP = Model::create<EighteenHP, EighteenHPWidget>("Bogaudio", "Bogaudio-EighteenHP", "18HP");
