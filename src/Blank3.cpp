
#include "Blank3.hpp"

void Blank3::onSampleRateChange() {
	_rms.setSampleRate(APP->engine->getSampleRate());
}

void Blank3::process(const ProcessArgs& args) {
	if (inputs[IN_INPUT].active) {
		_level = _rms.next(inputs[IN_INPUT].value) / 5.0f;
	}
	else {
		_level = -1.0f;
	}
}

struct Blank3Display : OpaqueWidget {
	const NVGcolor textColor = nvgRGBA(0x33, 0x33, 0x33, 0xff);
	const NVGcolor bgTextColor = nvgRGBA(0xaa, 0xaa, 0xaa, 0xff);
	const NVGcolor bgColor = nvgRGBA(0xdd, 0xdd, 0xdd, 0xff);
	Blank3* _module;
	const char* _text;
	std::shared_ptr<Font> _font;

	Blank3Display(Blank3* module, const char* text)
	: _module(module)
	, _text(text)
	, _font(APP->window->loadFont(asset::plugin(pluginInstance, "res/fonts/audiowide.ttf")))
	{
	}

	void draw(const DrawArgs& args) override {
		// FIXME.v1
		if (!_module) {
			return;
		}

		float offsetX = box.size.x / 2.0f;
		float offsetY = box.size.y / 2.0f;
		nvgSave(args.vg);
		nvgTranslate(args.vg, offsetX, offsetY);
		nvgRotate(args.vg, M_PI/2.0f);
		nvgTranslate(args.vg, -offsetY, offsetX);
		nvgFontSize(args.vg, 54.0f);
		nvgFontFaceId(args.vg, _font->handle);
		nvgTextLetterSpacing(args.vg, 9.0f);
		if (_module->_level < 0.0f) {
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, 0, 0, _text, NULL);
		}
		else {
			nvgFillColor(args.vg, bgTextColor);
			nvgText(args.vg, 0, 0, _text, NULL);
			if (_module->_level > 0.0001f) {
				nvgFillColor(args.vg, decibelsToColor(amplitudeToDecibels(_module->_level)));
				nvgText(args.vg, 0, 0, _text, NULL);
			}
		}
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 55, -20, 10, 10);
		nvgFillColor(args.vg, bgColor);
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}
};

struct Blank3Widget : ModuleWidget {
	static constexpr int hp = 3;

	Blank3Widget(Blank3* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blank3.svg")));
			addChild(panel);
		}

		{
			auto display = new Blank3Display(module, "BGA");
			display->box.pos = Vec(7.5, 125);
			display->box.size = Vec(30, 130);
			addChild(display);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		addInput(createInput<BlankPort24>(Vec(10.5, 346), module, Blank3::IN_INPUT));
	}
};

Model* modelBlank3 = bogaudio::createModel<Blank3, Blank3Widget>("Bogaudio-Blank3", "Blank3", "blank panel", BLANK_TAG);
