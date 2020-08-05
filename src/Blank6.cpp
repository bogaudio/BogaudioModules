
#include "Blank6.hpp"

void Blank6::sampleRateChange() {
	_rms.setSampleRate(APP->engine->getSampleRate());
}

void Blank6::processChannel(const ProcessArgs& args, int _c) {
	if (inputs[IN_INPUT].isConnected()) {
		_level = _rms.next(inputs[IN_INPUT].getVoltageSum()) / 5.0f;
	}
	else {
		_level = -1.0f;
	}
}

struct Blank6Display : OpaqueWidget {
	const NVGcolor textColor = nvgRGBA(0x33, 0x33, 0x33, 0xff);
	const NVGcolor bgTextColor = nvgRGBA(0xaa, 0xaa, 0xaa, 0xff);
	const NVGcolor bgColor = nvgRGBA(0xdd, 0xdd, 0xdd, 0xff);
	Blank6* _module;
	const char* _text;
	std::shared_ptr<Font> _font;

	Blank6Display(Blank6* module, const char* text)
	: _module(module)
	, _text(text)
	, _font(APP->window->loadFont(asset::plugin(pluginInstance, "res/fonts/audiowide.ttf")))
	{
	}

	void draw(const DrawArgs& args) override {
		float level = -1.0f;
		if (_module) {
			level = _module->_level;
		}

		float offsetX = box.size.x / 2.0f;
		float offsetY = box.size.y / 2.0f;
		nvgSave(args.vg);
		nvgTranslate(args.vg, offsetX, offsetY);
		nvgRotate(args.vg, M_PI/2.0f);
		nvgTranslate(args.vg, -offsetY, offsetX);
		nvgFontSize(args.vg, 52.0f);
		nvgFontFaceId(args.vg, _font->handle);
		nvgTextLetterSpacing(args.vg, 9.0f);
		if (level < 0.0f) {
			nvgFillColor(args.vg, textColor);
			nvgText(args.vg, 0, 0, _text, NULL);
		}
		else {
			nvgFillColor(args.vg, bgTextColor);
			nvgText(args.vg, 0, 0, _text, NULL);
			if (level > 0.0001f) {
				nvgFillColor(args.vg, decibelsToColor(amplitudeToDecibels(level)));
				nvgText(args.vg, 0, 0, _text, NULL);
			}
		}
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 97, -20, 10, 10);
		nvgFillColor(args.vg, bgColor);
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}
};

struct Blank6Widget : BGModuleWidget {
	static constexpr int hp = 6;

	Blank6Widget(Blank6* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Blank6.svg")));
			addChild(panel);
		}

		{
			auto display = new Blank6Display(module, "BOGAUDIO");
			display->box.pos = Vec(30, 32);
			display->box.size = Vec(30, 316);
			addChild(display);
		}

		addChild(createWidget<ScrewSilver>(Vec(0, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));

		addInput(createInput<BlankPort24>(Vec(33, 346), module, Blank6::IN_INPUT));
	}
};

Model* modelBlank6 = bogaudio::createModel<Blank6, Blank6Widget>("Bogaudio-Blank6", "BLANK6", "6HP blank panel", "Blank");
