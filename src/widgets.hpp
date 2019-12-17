#pragma once

#include <functional>
#include "rack.hpp"

using namespace rack;

extern Plugin *pluginInstance;

namespace bogaudio {

struct BGKnob : RoundKnob {
	BGKnob(const char* svg, int dim);

	void redraw();
};

struct Knob16 : BGKnob {
	Knob16();
};

struct Knob19 : BGKnob {
	Knob19();
};

struct Knob26 : BGKnob {
	Knob26();
};

struct Knob29 : BGKnob {
	Knob29();
};

struct Knob38 : BGKnob {
	Knob38();
};

struct Knob45 : BGKnob {
	Knob45();
};

struct Knob68 : BGKnob {
	Knob68();
};

struct IndicatorKnob : Knob {
	struct IKWidget : widget::Widget {
		float _angle = 0.0f;
		NVGcolor _color = nvgRGBA(0x00, 0x00, 0x00, 0x00);
		std::function<bool()> _drawColorsCB;

		void setAngle(float a);
		void draw(const DrawArgs& args) override;
	};

	widget::FramebufferWidget* fb;
	CircularShadow* shadow;
	IKWidget* w;

	IndicatorKnob(int dim);
	void onHover(const event::Hover& e) override;
	void onChange(const event::Change& e) override;
	inline void setDrawColorsCallback(std::function<bool()> fn) { w->_drawColorsCB = fn; }
	void redraw();
};

struct IndicatorKnob19 : IndicatorKnob {
	IndicatorKnob19() : IndicatorKnob(19) {}
};

struct Port24 : SvgPort {
	Port24();
};

struct BlankPort24 : Port24 {
	BlankPort24();
};

struct SliderSwitch : SvgSwitch {
	CircularShadow* shadow = NULL;
	SliderSwitch();
};

struct SliderSwitch2State14 : SliderSwitch {
	SliderSwitch2State14();
};

struct Button18 : SvgSwitch {
	Button18();
};

struct StatefulButton : ParamWidget {
	std::vector<std::shared_ptr<Svg>> _frames;
	SvgWidget* _svgWidget; // deleted elsewhere.
	CircularShadow* shadow = NULL;

	StatefulButton(const char* offSvgPath, const char* onSvgPath);
	void onDragStart(const event::DragStart& e) override;
	void onDragEnd(const event::DragEnd& e) override;
};

struct StatefulButton9 : StatefulButton {
	StatefulButton9();
};

struct StatefulButton18 : StatefulButton {
	StatefulButton18();
};

struct ToggleButton : SvgSwitch {
};

struct ToggleButton18 : ToggleButton {
	ToggleButton18();
};

struct IndicatorButtonGreen9 : SvgSwitch {
	IndicatorButtonGreen9();
};

struct InvertingIndicatorButton : ParamWidget {
	struct IIBWidget : widget::Widget {
		int _dim;
		NVGcolor _color = nvgRGBA(0x00, 0x00, 0x00, 0x00);

		IIBWidget(int dim) : _dim(dim) {}

		void setValue(float v);
		void draw(const DrawArgs& args) override;
	};

	widget::FramebufferWidget* fb;
	CircularShadow* shadow;
	IIBWidget* w;

	InvertingIndicatorButton(int dim);

	void reset() override;
	void randomize() override;
	void onHover(const event::Hover& e) override;
	void onDoubleClick(const event::DoubleClick& e) override {}
	void onButton(const event::Button& e) override;
	void onChange(const event::Change& e) override;
};

struct InvertingIndicatorButton9 : InvertingIndicatorButton {
	InvertingIndicatorButton9() : InvertingIndicatorButton(9) {}
};

struct InvertingIndicatorButton18 : InvertingIndicatorButton {
	InvertingIndicatorButton18() : InvertingIndicatorButton(18) {}
};

NVGcolor decibelsToColor(float db);

struct VUSlider : SliderKnob {
	const float slideHeight = 13.0f;
	float* _vuLevel = NULL;

	VUSlider(float height = 183.0f) {
		box.size = Vec(18.0f, height);
	}

	void setVULevel(float* vuLevel) {
		_vuLevel = vuLevel;
	}
	void draw(const DrawArgs& args) override;
};

struct VUSlider151 : VUSlider {
	VUSlider151() : VUSlider(151.0f) {}
};

} // namespace bogaudio
