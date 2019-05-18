
#include "widgets.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio;
using namespace bogaudio::dsp;

Button18::Button18() {
	addFrame(SVG::load(asset::plugin(pluginInstance, "res/button_18px_0.svg")));
	addFrame(SVG::load(asset::plugin(pluginInstance, "res/button_18px_1.svg")));
	box.size = Vec(18, 18);
}


BGKnob::BGKnob(const char* svg, int dim) {
	setSVG(SVG::load(asset::plugin(pluginInstance, svg)));
	box.size = Vec(dim, dim);
	shadow->blurRadius = 2.0;
	// k->shadow->opacity = 0.15;
	shadow->box.pos = Vec(0.0, 3.0);
}


Knob16::Knob16() : BGKnob("res/knob_16px.svg", 16) {
	shadow->box.pos = Vec(0.0, 2.5);
}


Knob19::Knob19() : BGKnob("res/knob_19px.svg", 19) {
	shadow->box.pos = Vec(0.0, 2.5);
}


Knob26::Knob26() : BGKnob("res/knob_26px.svg", 26) {
}


Knob29::Knob29() : BGKnob("res/knob_29px.svg", 29) {
}


Knob38::Knob38() : BGKnob("res/knob_38px.svg", 38) {
}


Knob45::Knob45() : BGKnob("res/knob_45px.svg", 45) {
}


Knob68::Knob68() : BGKnob("res/knob_68px.svg", 68) {
	shadow->box.pos = Vec(0.0, 4.0);
}


Port24::Port24() {
	setSVG(SVG::load(asset::plugin(pluginInstance, "res/port.svg")));
	box.size = Vec(24, 24);
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 1.5);
}


BlankPort24::BlankPort24() {
	setSVG(NULL);
	box.size = Vec(24, 24);
}


SliderSwitch::SliderSwitch() {
	shadow = new CircularShadow();
	addChild(shadow);
	shadow->box.size = Vec();
}


SliderSwitch2State14::SliderSwitch2State14() {
	addFrame(SVG::load(asset::plugin(pluginInstance, "res/slider_switch_2_14px_0.svg")));
	addFrame(SVG::load(asset::plugin(pluginInstance, "res/slider_switch_2_14px_1.svg")));
	shadow->box.size = Vec(14.0, 24.0);
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 7.0);
}


StatefulButton::StatefulButton(const char* offSVGPath, const char* onSVGPath) {
	shadow = new CircularShadow();
	addChild(shadow);

	_svgWidget = new SVGWidget();
	addChild(_svgWidget);

	auto svg = SVG::load(asset::plugin(pluginInstance, offSVGPath));
	_frames.push_back(svg);
	_frames.push_back(SVG::load(asset::plugin(pluginInstance, onSVGPath)));

	_svgWidget->setSVG(svg);
	box.size = _svgWidget->box.size;
	shadow->box.size = _svgWidget->box.size;
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 1.0);
}

void StatefulButton::step() {
	// FIXME.v1 FramebufferWidget::step();
}

void StatefulButton::onDragStart(const event::DragStart& e) {
	_svgWidget->setSVG(_frames[1]);
	// FIXME.v1
	// dirty = true;
	//
	// if (value >= maxValue) {
	// 	setValue(minValue);
	// }
	// else {
	// 	setValue(value + 1.0);
	// }
}

void StatefulButton::onDragEnd(const event::DragEnd& e) {
	_svgWidget->setSVG(_frames[0]);
	// FIXME.v1 dirty = true;
}


StatefulButton9::StatefulButton9() : StatefulButton("res/button_9px_0.svg", "res/button_9px_1.svg") {
}


StatefulButton18::StatefulButton18() : StatefulButton("res/button_18px_0.svg", "res/button_18px_1.svg") {
}


ToggleButton18::ToggleButton18() {
	addFrame(SVG::load(asset::plugin(pluginInstance, "res/button_18px_0.svg")));
	addFrame(SVG::load(asset::plugin(pluginInstance, "res/button_18px_1.svg")));
}


NVGcolor bogaudio::decibelsToColor(float db) {
	if (db < -80.0f) {
		return nvgRGBA(0x00, 0x00, 0x00, 0x00);
	}
	if (db < -24.0f) {
		return nvgRGBA(0x55, 0xff, 0x00, (1.0f - (db + 24.0f) / -56.0f) * (float)0xff);
	}
	if (db < 0.0f) {
		return nvgRGBA((1.0f - db / -24.0f) * 0xff, 0xff, 0x00, 0xff);
	}
	return nvgRGBA(0xff, (1.0f - std::min(db, 9.0f) / 9.0f) * 0xff, 0x00, 0xff);
}


void VUSlider::draw(const DrawArgs& args) {
	nvgSave(args.vg);
	{
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 6, 3, 6, box.size.y - 6, 2);
		nvgFillColor(args.vg, nvgRGBA(0x22, 0x22, 0x22, 0xff));
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(0x88, 0x88, 0x88, 0xff));
		nvgStroke(args.vg);
	}
	nvgRestore(args.vg);

	nvgSave(args.vg);
	{
		// FIXME.v1 nvgTranslate(args.vg, 0, (box.size.y - 13.0f) * (1.0f - value));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0, 0, 18, 13, 1.5);
		nvgFillColor(args.vg, nvgRGBA(0x77, 0x77, 0x77, 0xff));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 2, 18, 9);
		nvgFillColor(args.vg, nvgRGBA(0x44, 0x44, 0x44, 0xff));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 6, 18, 1);
		nvgFillColor(args.vg, nvgRGBA(0xfa, 0xfa, 0xfa, 0xff));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 2, 4, 14, 5, 1.0);
		nvgFillColor(args.vg, nvgRGBA(0xaa, 0xaa, 0xaa, 0xff));
		nvgFill(args.vg);

		float db = _vuLevel ? *_vuLevel : 0.0f;
		if (db > 0.0f) {
			db = amplitudeToDecibels(db);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 2, 4, 14, 5, 1.0);
			nvgFillColor(args.vg, decibelsToColor(db));
			nvgFill(args.vg);
		}
	}
	nvgRestore(args.vg);
}
