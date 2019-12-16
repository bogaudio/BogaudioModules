
#include "widgets.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio;
using namespace bogaudio::dsp;

Button18::Button18() {
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1.svg")));
	box.size = Vec(18, 18);
	momentary = true;
}


BGKnob::BGKnob(const char* svg, int dim) {
	setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, svg)));
	box.size = Vec(dim, dim);
	shadow->blurRadius = 2.0;
	// k->shadow->opacity = 0.15;
	shadow->box.pos = Vec(0.0, 3.0);
}

void BGKnob::redraw() {
	event::Change c;
	onChange(c);
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


void IndicatorKnob::IKWidget::setAngle(float a) {
	assert(a >= -1.0f && a <= 1.0f);

	_angle = a * 0.83f * M_PI;
	if (a < 0.0f) {
		_color.r = 1.0f; // 0xff
		_color.g = 0.6f; // 0x99
		_color.b = 0.0f; // 0x00
		_color.a = -a;
	}
	else {
		_color.r = 0.333f; // 0x55
		_color.g = 1.0f; // 0xff
		_color.b = 0.333f; // 0x55
		_color.a = a;
	}
}

void IndicatorKnob::IKWidget::draw(const DrawArgs& args) {
	nvgSave(args.vg);

	float c = box.size.x * 0.5f;
	nvgTranslate(args.vg, c, c);
	nvgRotate(args.vg, _angle);
	nvgTranslate(args.vg, -c, -c);

	float r = c - 0.2f; // FIXME: need to scale everything if there is ever a dim != 19.
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgFillColor(args.vg, nvgRGBA(0x33, 0x33, 0x33, 0xff));
	nvgFill(args.vg);

	r -= 2.0f;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgFillColor(args.vg, nvgRGBA(0xee, 0xee, 0xee, 0xff));
	nvgFill(args.vg);
	if (!_drawColorsCB || _drawColorsCB()) {
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c, c, r);
		nvgFillColor(args.vg, _color);
		nvgFill(args.vg);

		r -= 0.15f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c, c, r);
		nvgStrokeColor(args.vg, nvgRGBA(0x66, 0x66, 0x66, 0x7f));
		nvgStrokeWidth(args.vg, 0.3f);
		nvgStroke(args.vg);

		r -= 0.3f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c, c, r);
		nvgStrokeColor(args.vg, nvgRGBA(0x77, 0x77, 0x77, 0x7f));
		nvgStrokeWidth(args.vg, 0.3f);
		nvgStroke(args.vg);

		r -= 0.3f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c, c, r);
		nvgStrokeColor(args.vg, nvgRGBA(0x88, 0x88, 0x88, 0x7f));
		nvgStrokeWidth(args.vg, 0.3f);
		nvgStroke(args.vg);

		r -= 0.3f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c, c, r);
		nvgStrokeColor(args.vg, nvgRGBA(0x99, 0x99, 0x99, 0x7f));
		nvgStrokeWidth(args.vg, 0.3f);
		nvgStroke(args.vg);

		r -= 0.15f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c, c, r);
		nvgFillColor(args.vg, nvgRGBA(0xaa, 0xaa, 0xaa, 0x7f));
		nvgFill(args.vg);
	}

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, 1.6f, 1.3f);
	nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, 1.9f, 1.3f);
	nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
	nvgFill(args.vg);

	nvgRestore(args.vg);
}

IndicatorKnob::IndicatorKnob(int dim) {
	box.size = math::Vec(dim, dim);
	box.pos = math::Vec(0, 0);
	fb = new widget::FramebufferWidget;
	addChild(fb);
	fb->box.size = box.size;

	shadow = new CircularShadow;
	shadow->box.size = box.size;
	shadow->blurRadius = 2.0;
	// shadow->opacity = 0.15;
	shadow->box.pos = Vec(0.0, 3.0);
	fb->addChild(shadow);

	w = new IKWidget;
	w->box.size = box.size;
	w->box.pos = math::Vec(0, 0);
	fb->addChild(w);
}

void IndicatorKnob::onHover(const event::Hover& e) {
	math::Vec c = box.size.div(2);
	float dist = e.pos.minus(c).norm();
	if (dist <= c.x) {
		ParamWidget::onHover(e);
	}
}

void IndicatorKnob::onChange(const event::Change& e) {
	fb->dirty = true;
	if (paramQuantity) {
		w->setAngle(paramQuantity->getValue());
	}
	Knob::onChange(e);
}

void IndicatorKnob::redraw() {
	event::Change c;
	onChange(c);
}


Port24::Port24() {
	setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/port.svg")));
	box.size = Vec(24, 24);
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 1.5);
}


BlankPort24::BlankPort24() {
	setSvg(NULL);
	box.size = Vec(24, 24);
}


SliderSwitch::SliderSwitch() {
	shadow = new CircularShadow();
	addChild(shadow);
	shadow->box.size = Vec();
}


SliderSwitch2State14::SliderSwitch2State14() {
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/slider_switch_2_14px_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/slider_switch_2_14px_1.svg")));
	shadow->box.size = Vec(14.0, 24.0);
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 7.0);
}


StatefulButton::StatefulButton(const char* offSvgPath, const char* onSvgPath) {
	shadow = new CircularShadow();
	addChild(shadow);

	_svgWidget = new SvgWidget();
	addChild(_svgWidget);

	auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, offSvgPath));
	_frames.push_back(svg);
	_frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, onSvgPath)));

	_svgWidget->setSvg(svg);
	box.size = _svgWidget->box.size;
	shadow->box.size = _svgWidget->box.size;
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 1.0);
}

void StatefulButton::onDragStart(const event::DragStart& e) {
	if (paramQuantity) {
		_svgWidget->setSvg(_frames[1]);
		if (paramQuantity->getValue() >= paramQuantity->getMaxValue()) {
			paramQuantity->setValue(paramQuantity->getMinValue());
		}
		else {
			paramQuantity->setValue(paramQuantity->getValue() + 1.0);
		}
	}
}

void StatefulButton::onDragEnd(const event::DragEnd& e) {
	_svgWidget->setSvg(_frames[0]);
}


StatefulButton9::StatefulButton9() : StatefulButton("res/button_9px_0.svg", "res/button_9px_1.svg") {
}


StatefulButton18::StatefulButton18() : StatefulButton("res/button_18px_0.svg", "res/button_18px_1.svg") {
}


ToggleButton18::ToggleButton18() {
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1.svg")));
}


IndicatorButtonGreen9::IndicatorButtonGreen9() {
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_9px_0.svg")));
	addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_9px_1_green.svg")));
}


void InvertingIndicatorButton::IIBWidget::setValue(float v) {
	assert(v >= -1.0f && v <= 1.0f);

	if (v < 0.0f) {
		_color.r = 1.0f; // 0xff
		_color.g = 0.6f; // 0x99
		_color.b = 0.0f; // 0x00
		_color.a = -v;
	}
	else {
		_color.r = 0.333f; // 0x55
		_color.g = 1.0f; // 0xff
		_color.b = 0.333f; // 0x55
		_color.a = v;
	}
}

void InvertingIndicatorButton::IIBWidget::draw(const DrawArgs& args) {
	nvgSave(args.vg);

	float c = box.size.x * 0.5f;
	float s = ((float)_dim) / 18.0f;
	float r = c - 0.1f;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgFillColor(args.vg, nvgRGBA(0x88, 0x88, 0x88, 0xff));
	nvgFill(args.vg);

	r -= std::max(0.3f, 0.5f * s);
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgFillColor(args.vg, nvgRGBA(0x33, 0x33, 0x33, 0xff));
	nvgFill(args.vg);

	r -= std::max(0.2f, 0.3f * s);
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgFillColor(args.vg, nvgRGBA(0xee, 0xee, 0xee, 0xff));
	nvgFill(args.vg);
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgFillColor(args.vg, _color);
	nvgFill(args.vg);

	r -= 0.15f;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgStrokeColor(args.vg, nvgRGBA(0x66, 0x66, 0x66, 0x7f));
	nvgStrokeWidth(args.vg, 0.3f * s);
	nvgStroke(args.vg);

	float dr = std::max(0.2f, 0.3f * s);
	r -= dr;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgStrokeColor(args.vg, nvgRGBA(0x77, 0x77, 0x77, 0x7f));
	nvgStrokeWidth(args.vg, dr);
	nvgStroke(args.vg);

	r -= dr;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgStrokeColor(args.vg, nvgRGBA(0x88, 0x88, 0x88, 0x7f));
	nvgStrokeWidth(args.vg, dr);
	nvgStroke(args.vg);

	r -= dr;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgStrokeColor(args.vg, nvgRGBA(0x99, 0x99, 0x99, 0x7f));
	nvgStrokeWidth(args.vg, dr);
	nvgStroke(args.vg);

	r -= 0.15f;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, c, c, r);
	nvgFillColor(args.vg, nvgRGBA(0xaa, 0xaa, 0xaa, 0x7f));
	nvgFill(args.vg);

	nvgRestore(args.vg);
}

InvertingIndicatorButton::InvertingIndicatorButton(int dim) {
	box.size = math::Vec(dim, dim);
	// box.pos = math::Vec(0, 0);
	fb = new widget::FramebufferWidget;
	addChild(fb);
	fb->box.size = box.size;

	shadow = new CircularShadow;
	shadow->box.size = box.size;
	shadow->blurRadius = 2.0;
	shadow->box.pos = Vec(0.0, 1.0);
	fb->addChild(shadow);

	w = new IIBWidget(dim);
	w->box.size = box.size;
	// w->box.pos = math::Vec(0, 0);
	fb->addChild(w);
}

void InvertingIndicatorButton::reset() {
	if (paramQuantity) {
		paramQuantity->reset();
	}
}

void InvertingIndicatorButton::randomize() {
	if (paramQuantity) {
		paramQuantity->setValue(roundf(2.0f * random::uniform()) - 1.0f);
	}
}

void InvertingIndicatorButton::onHover(const event::Hover& e) {
	math::Vec c = box.size.div(2);
	float dist = e.pos.minus(c).norm();
	if (dist <= c.x) {
		ParamWidget::onHover(e);
	}
}

void InvertingIndicatorButton::onButton(const event::Button& e) {
	ParamWidget::onButton(e);
	if (!paramQuantity || !(e.action == GLFW_PRESS && (e.mods & RACK_MOD_MASK) == 0)) {
		return;
	}

	float value = paramQuantity->getValue();
	if (value <= -1.0f) {
		paramQuantity->setValue(0.0f);
	}
	else if (value < 1.0f) {
		paramQuantity->setValue(1.0f);
	}
	else {
		paramQuantity->setValue(-1.0f);
	}
}

void InvertingIndicatorButton::onChange(const event::Change& e) {
	fb->dirty = true;
	if (paramQuantity) {
		w->setValue(paramQuantity->getValue());
	}
	ParamWidget::onChange(e);
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
	float level = 0.0f;
	if (paramQuantity) {
		level = paramQuantity->getValue();
	}
	else {
		float minDb = -60.0f;
		float maxDb = 6.0f;
		level = fabsf(minDb) / (maxDb - minDb);
	}

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
		nvgTranslate(args.vg, 0, (box.size.y - 13.0f) * (1.0f - level));
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
