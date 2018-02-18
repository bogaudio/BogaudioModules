
#include "bogaudio.hpp"

struct TwentyTwoHP : Module {
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

	TwentyTwoHP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onReset();
	}

	virtual void onReset() override;
	virtual void step() override;
};

void TwentyTwoHP::onReset() {
}

void TwentyTwoHP::step() {
}


struct TwentyTwoHPWidget : ModuleWidget {
	TwentyTwoHPWidget(TwentyTwoHP* module);
};

TwentyTwoHPWidget::TwentyTwoHPWidget(TwentyTwoHP* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 22, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/TwentyTwoHP.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));
}


Model* modelTwentyTwoHP = Model::create<TwentyTwoHP, TwentyTwoHPWidget>("Bogaudio", "Bogaudio-TwentyTwoHP", "22HP");
