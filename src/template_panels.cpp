
#include "template_panels.hpp"

struct ThreeHPWidget : ModuleWidget {
  ThreeHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ThreeHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(0, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
  }
};

Model* modelThreeHP = rack::createModel<Module, ThreeHPWidget>("Bogaudio-ThreeHP");


struct SixHPWidget : ModuleWidget {
  SixHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 6, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SixHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(0, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
  }
};

Model* modelSixHP = rack::createModel<Module, SixHPWidget>("Bogaudio-SixHP");


struct EightHPWidget : ModuleWidget {
  EightHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EightHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(0, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
  }
};

Model* modelEightHP = rack::createModel<Module, EightHPWidget>("Bogaudio-EightHP");


struct TenHPWidget : ModuleWidget {
	TenHPWidget(Module* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * 10, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TenHP.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(0, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
	}
};

Model* modelTenHP = rack::createModel<Module, TenHPWidget>("Bogaudio-TenHP");


struct TwelveHPWidget : ModuleWidget {
  TwelveHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 12, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TwelveHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(0, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(0, 365)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
  }
};

Model* modelTwelveHP = rack::createModel<Module, TwelveHPWidget>("Bogaudio-TwelveHP");

struct ThirteenHPWidget : ModuleWidget {
  ThirteenHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 13, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ThirteenHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(0, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(0, 365)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
  }
};

Model* modelThirteenHP = rack::createModel<Module, ThirteenHPWidget>("Bogaudio-ThirteenHP");


struct FifteenHPWidget : ModuleWidget {
  FifteenHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 15, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/FifteenHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));
  }
};

Model* modelFifteenHP = rack::createModel<Module, FifteenHPWidget>("Bogaudio-FifteenHP");


struct EighteenHPWidget : ModuleWidget {
  EighteenHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 18, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EighteenHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));
  }
};

Model* modelEighteenHP = rack::createModel<Module, EighteenHPWidget>("Bogaudio-EighteenHP");


struct TwentyHPWidget : ModuleWidget {
  TwentyHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 20, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TwentyHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));
  }
};

Model* modelTwentyHP = rack::createModel<Module, TwentyHPWidget>("Bogaudio-TwentyHP");


struct TwentyTwoHPWidget : ModuleWidget {
  TwentyTwoHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 22, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TwentyTwoHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));
  }
};

Model* modelTwentyTwoHP = rack::createModel<Module, TwentyTwoHPWidget>("Bogaudio-TwentyTwoHP");


struct TwentyFiveHPWidget : ModuleWidget {
  TwentyFiveHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 25, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/TwentyFiveHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));
  }
};

Model* modelTwentyFiveHP = rack::createModel<Module, TwentyFiveHPWidget>("Bogaudio-TwentyFiveHP");


struct ThirtyHPWidget : ModuleWidget {
  ThirtyHPWidget(Module* module) : ModuleWidget(module) {
  	box.size = Vec(RACK_GRID_WIDTH * 30, RACK_GRID_HEIGHT);

  	{
  		SVGPanel *panel = new SVGPanel();
  		panel->box.size = box.size;
  		panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ThirtyHP.svg")));
  		addChild(panel);
  	}

  	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
  	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
  	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));
  }
};

Model* modelThirtyHP = rack::createModel<Module, ThirtyHPWidget>("Bogaudio-ThirtyHP");
