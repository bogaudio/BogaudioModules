
#include "template_panels.hpp"

struct ThreeHPWidget : ModuleWidget {
	ThreeHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ThreeHP.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
	}
};

Model* modelThreeHP = bogaudio::createModel<Module, ThreeHPWidget>("Bogaudio-ThreeHP", "THREEHP", "Template/blank", "Blank");


struct SixHPWidget : ModuleWidget {
	SixHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 6, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SixHP.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
	}
};

Model* modelSixHP = bogaudio::createModel<Module, SixHPWidget>("Bogaudio-SixHP", "SIXHP", "Template/blank", "Blank");


struct EightHPWidget : ModuleWidget {
	EightHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 8, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EightHP.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));
	}
};

Model* modelEightHP = bogaudio::createModel<Module, EightHPWidget>("Bogaudio-EightHP", "EIGHTHP", "Template/blank", "Blank");


struct TenHPWidget : ModuleWidget {
	TenHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 10, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelTenHP = bogaudio::createModel<Module, TenHPWidget>("Bogaudio-TenHP", "TENHP", "Template/blank", "Blank");


struct TwelveHPWidget : ModuleWidget {
	TwelveHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 12, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelTwelveHP = bogaudio::createModel<Module, TwelveHPWidget>("Bogaudio-TwelveHP", "TWELVEHP", "Template/blank", "Blank");


struct ThirteenHPWidget : ModuleWidget {
	ThirteenHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 13, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelThirteenHP = bogaudio::createModel<Module, ThirteenHPWidget>("Bogaudio-ThirteenHP", "THIRTEENHP", "Template/blank", "Blank");


struct FifteenHPWidget : ModuleWidget {
	FifteenHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 15, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelFifteenHP = bogaudio::createModel<Module, FifteenHPWidget>("Bogaudio-FifteenHP", "FIFTEENHP", "Template/blank", "Blank");


struct SixteenHPWidget : ModuleWidget {
	SixteenHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 16, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SixteenHP.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));
	}
};

Model* modelSixteenHP = bogaudio::createModel<Module, SixteenHPWidget>("Bogaudio-SixteenHP", "SIXTEENHP", "Template/blank", "Blank");


struct EighteenHPWidget : ModuleWidget {
	EighteenHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 18, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelEighteenHP = bogaudio::createModel<Module, EighteenHPWidget>("Bogaudio-EighteenHP", "EIGHTEENHP", "Template/blank", "Blank");


struct TwentyHPWidget : ModuleWidget {
	TwentyHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 20, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelTwentyHP = bogaudio::createModel<Module, TwentyHPWidget>("Bogaudio-TwentyHP", "TWENTYHP", "Template/blank", "Blank");


struct TwentyTwoHPWidget : ModuleWidget {
	TwentyTwoHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 22, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelTwentyTwoHP = bogaudio::createModel<Module, TwentyTwoHPWidget>("Bogaudio-TwentyTwoHP", "TWENTYTWOHP", "Template/blank", "Blank");


struct TwentyFiveHPWidget : ModuleWidget {
	TwentyFiveHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 25, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelTwentyFiveHP = bogaudio::createModel<Module, TwentyFiveHPWidget>("Bogaudio-TwentyFiveHP", "TWENTYFIVEHP", "Template/blank", "Blank");


struct ThirtyHPWidget : ModuleWidget {
	ThirtyHPWidget(Module* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * 30, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
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

Model* modelThirtyHP = bogaudio::createModel<Module, ThirtyHPWidget>("Bogaudio-ThirtyHP", "THIRTYHP", "Template/blank", "Blank");
