#include "rack.hpp"

using namespace rack;

extern Plugin *plugin;

struct StatefulButton : ParamWidget, FramebufferWidget {
	std::vector<std::shared_ptr<SVG>> _frames;
	SVGWidget* _svgWidget; // deleted elsewhere.

	StatefulButton(const char* offSVGPath, const char* onSVGPath);
	void step() override;
	void onDragStart(EventDragStart& e) override;
	void onDragEnd(EventDragEnd& e) override;
};

struct StatefulButton9 : StatefulButton {
	StatefulButton9() : StatefulButton("res/button_9px_0.svg", "res/button_9px_1.svg") {}
};
