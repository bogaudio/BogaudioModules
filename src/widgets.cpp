
#include "widgets.hpp"

using namespace bogaudio;

Button18::Button18() {
	addFrame(SVG::load(assetPlugin(plugin, "res/button_18px_0.svg")));
	addFrame(SVG::load(assetPlugin(plugin, "res/button_18px_1.svg")));
	box.size = Vec(18, 18);
}


BGKnob::BGKnob(const char* svg, int dim) {
	setSVG(SVG::load(assetPlugin(plugin, svg)));
	box.size = Vec(dim, dim);
	shadow->blurRadius = 2.0;
	// k->shadow->opacity = 0.15;
	shadow->box.pos = Vec(0.0, 3.0);
}


Knob16::Knob16() : BGKnob("res/knob_16px.svg", 16) {
	shadow->box.pos = Vec(0.0, 2.5);
}


Knob26::Knob26() : BGKnob("res/knob_26px.svg", 26) {
}


Knob29::Knob29() : BGKnob("res/knob_29px.svg", 29) {
}


Knob38::Knob38() : BGKnob("res/knob_38px.svg", 38) {
}


Knob68::Knob68() : BGKnob("res/knob_68px.svg", 68) {
	shadow->box.pos = Vec(0.0, 4.0);
}


Port24::Port24() {
	setSVG(SVG::load(assetPlugin(plugin, "res/port.svg")));
	box.size = Vec(24, 24);
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 1.5);
}


SliderSwitch::SliderSwitch() {
	shadow = new CircularShadow();
	addChild(shadow);
	shadow->box.size = Vec();
}


SliderSwitch2State14::SliderSwitch2State14() {
	addFrame(SVG::load(assetPlugin(plugin, "res/slider_switch_2_14px_0.svg")));
	addFrame(SVG::load(assetPlugin(plugin, "res/slider_switch_2_14px_1.svg")));
	shadow->box.size = Vec(14.0, 24.0);
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 7.0);
}


StatefulButton::StatefulButton(const char* offSVGPath, const char* onSVGPath) {
	shadow = new CircularShadow();
	addChild(shadow);

	_svgWidget = new SVGWidget();
	addChild(_svgWidget);

	auto svg = SVG::load(assetPlugin(plugin, offSVGPath));
	_frames.push_back(svg);
	_frames.push_back(SVG::load(assetPlugin(plugin, onSVGPath)));

	_svgWidget->setSVG(svg);
	box.size = _svgWidget->box.size;
	shadow->box.size = _svgWidget->box.size;
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 1.0);
}

void StatefulButton::step() {
	FramebufferWidget::step();
}

void StatefulButton::onDragStart(EventDragStart& e) {
	_svgWidget->setSVG(_frames[1]);
	dirty = true;

	if (value >= maxValue) {
		setValue(minValue);
	}
	else {
		setValue(value + 1.0);
	}
}

void StatefulButton::onDragEnd(EventDragEnd& e) {
	_svgWidget->setSVG(_frames[0]);
	dirty = true;
}


StatefulButton9::StatefulButton9() : StatefulButton("res/button_9px_0.svg", "res/button_9px_1.svg") {
}
