
#include "bogaudio.hpp"

struct FifteenHP : Module {
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

	FifteenHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void FifteenHP::onReset() {
}

void FifteenHP::step() {
}


struct FifteenHPWidget : ModuleWidget {
	FifteenHPWidget(FifteenHP* module);
};

FifteenHPWidget::FifteenHPWidget(FifteenHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 15, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/FifteenHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));
}


Model* modelFifteenHP = Model::create<FifteenHP, FifteenHPWidget>("Bogaudio", "Bogaudio-FifteenHP", "15HP");
