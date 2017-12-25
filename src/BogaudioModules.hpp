#include "rack.hpp"

using namespace rack;

extern Plugin *plugin;

struct ShaperWidget : ModuleWidget {
	ShaperWidget();
};

struct ShaperPlusWidget : ModuleWidget {
	ShaperPlusWidget();
};

struct DADSRHWidget : ModuleWidget {
	DADSRHWidget();
};

struct DADSRHPlusWidget : ModuleWidget {
	DADSRHPlusWidget();
};

struct AnalyzerWidget : ModuleWidget {
	AnalyzerWidget();
};


struct OffsetWidget : ModuleWidget {
	OffsetWidget();
};

struct SampleHoldWidget : ModuleWidget {
	SampleHoldWidget();
};

struct ManualWidget : ModuleWidget {
	ManualWidget();
};

struct NoiseWidget : ModuleWidget {
	NoiseWidget();
};


struct Knob29 : RoundKnob {
	Knob29() {
		setSVG(SVG::load(assetPlugin(plugin, "res/knob_29px.svg")));
		box.size = Vec(29, 29);
	}
};

struct Knob38 : RoundKnob {
	Knob38() {
		setSVG(SVG::load(assetPlugin(plugin, "res/knob_38px.svg")));
		box.size = Vec(38, 38);
	}
};

struct Button18 : SVGSwitch, MomentarySwitch {
	Button18() {
		addFrame(SVG::load(assetPlugin(plugin, "res/button_18px.svg")));
		box.size = Vec(18, 18);
	}
};

struct Button9Toggle2 : SVGSwitch, ToggleSwitch {
	Button9Toggle2() {
		addFrame(SVG::load(assetPlugin(plugin, "res/button_9px.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/button_9px.svg")));
		box.size = Vec(9, 9);
	}
};

struct Button9Toggle3 : SVGSwitch, ToggleSwitch {
	Button9Toggle3() {
		addFrame(SVG::load(assetPlugin(plugin, "res/button_9px.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/button_9px.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/button_9px.svg")));
		box.size = Vec(9, 9);
	}
};

struct Port24 : SVGPort {
	Port24() {
		background->svg = SVG::load(assetPlugin(plugin, "res/port.svg"));
		background->wrap();
		box.size = Vec(24, 24);
	}
};

struct SliderSwitch2State14 : SVGSwitch, ToggleSwitch {
	SliderSwitch2State14() {
		addFrame(SVG::load(assetPlugin(plugin, "res/slider_switch_2_14px_0.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/slider_switch_2_14px_1.svg")));
	}
};
