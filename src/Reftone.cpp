
#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/pitch.hpp"

using namespace bogaudio::dsp;

struct Reftone : Module {
	enum ParamsIds {
		PITCH_PARAM,
		OCTAVE_PARAM,
		FINE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		CV_OUTPUT,
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	int _pitch = 9;
	int _octave = 4;
	float _fine = 0.0;
	float _frequency = 440.0;
	SineOscillator _sine;

	Reftone()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	, _sine(engineGetSampleRate(), _frequency, 5.0)
	{
	}

	virtual void onSampleRateChange() override {
		_sine.setSampleRate(engineGetSampleRate());
	}

	virtual void step() override;
};

void Reftone::step() {
	// C4 -- the pitch.hpp reference frequency -- in knob values:
	const int referencePitch = 0;
	const int referenceOctave = 4;

	_pitch = clampf(params[PITCH_PARAM].value, 0.0, 11.0);
	_octave = clampf(params[OCTAVE_PARAM].value, 1.0, 8.0);
	_fine = clampf(params[FINE_PARAM].value, -0.99, 0.99);
	_frequency = semitoneToFrequency(referenceSemitone + 12*(_octave - referenceOctave) + (_pitch - referencePitch) + _fine);

	if (outputs[CV_OUTPUT].active) {
		outputs[CV_OUTPUT].value = frequencyToCV(_frequency);
	}
	else {
		outputs[CV_OUTPUT].value = 0.0;
	}

	if (outputs[OUT_OUTPUT].active) {
		_sine.setFrequency(_frequency);
		outputs[OUT_OUTPUT].value = _sine.next();
	}
	else {
		outputs[OUT_OUTPUT].value = 0.0;
	}
}


struct ReftoneDisplay : TransparentWidget {
	const NVGcolor _textColor = nvgRGBA(0x00, 0xff, 0x00, 0xee);

	Reftone* _module;
	const Vec _size;
	std::shared_ptr<Font> _font;

	ReftoneDisplay(
		Reftone* module,
		Vec size
	)
	: _module(module)
	, _size(size)
	, _font(Font::load(assetPlugin(plugin, "res/fonts/inconsolata-bold.ttf")))
	{
	}

	void draw(NVGcontext* vg) override;
	void drawBackground(NVGcontext* vg);
	void drawText(NVGcontext* vg, const char* s, float x, float y, int size);
	void drawCenteredText(NVGcontext* vg, const char* s, float y, int size);
	float textRenderWidth(NVGcontext* vg, const char* s, int size);
};

void ReftoneDisplay::draw(NVGcontext* vg) {
	const int n = 20;
	char octave[n];
	snprintf(octave, n, "%d", _module->_octave);

	char fine[n];
	fine[0] = _module->_fine < 0.0 ? '-' : '+';
	snprintf(fine + 1, n - 1, "%02d", abs((int)(_module->_fine * 100)));

	char frequency[20];
	snprintf(frequency, n, _module->_frequency >= 1000.0 ? "%0.0f" : "%0.1f", _module->_frequency);

	const char* pitch = NULL;
	const char* sharpFlat = NULL;
	switch (_module->_pitch) {
		case 0: {
			pitch = "C";
			break;
		}
		case 1: {
			pitch = "C";
			sharpFlat = "#";
			break;
		}
		case 2: {
			pitch = "D";
			break;
		}
		case 3: {
			pitch = "E";
			sharpFlat = "b";
			break;
		}
		case 4: {
			pitch = "E";
			break;
		}
		case 5: {
			pitch = "F";
			break;
		}
		case 6: {
			pitch = "F";
			sharpFlat = "#";
			break;
		}
		case 7: {
			pitch = "G";
			break;
		}
		case 8: {
			pitch = "G";
			sharpFlat = "#";
			break;
		}
		case 9: {
			pitch = "A";
			break;
		}
		case 10: {
			pitch = "B";
			sharpFlat = "b";
			break;
		}
		case 11: {
			pitch = "B";
			break;
		}
	}

	drawBackground(vg);
	if (sharpFlat) {
		drawText(vg, pitch, 3, 20, 28);
		drawText(vg, sharpFlat, 16, 12, 16);
		drawText(vg, octave, 22, 20, 28);
	}
	else {
		char s[n];
		snprintf(s, n, "%s%s", pitch, octave);
		drawCenteredText(vg, s, 20, 28);
	}
	drawCenteredText(vg, fine, 32.5, 14);
	drawCenteredText(vg, frequency, 45, 14);
}

void ReftoneDisplay::drawBackground(NVGcontext* vg) {
	nvgSave(vg);
	nvgBeginPath(vg);
	nvgRect(vg, 0, 0, _size.x, _size.y);
	nvgFillColor(vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
	nvgFill(vg);
	nvgRestore(vg);
}

void ReftoneDisplay::drawText(NVGcontext* vg, const char* s, float x, float y, int size) {
	nvgSave(vg);
	nvgTranslate(vg, x, y);
	nvgFontSize(vg, size);
	nvgFontFaceId(vg, _font->handle);
	nvgFillColor(vg, _textColor);
	nvgText(vg, 0, 0, s, NULL);
	nvgRestore(vg);
}

void ReftoneDisplay::drawCenteredText(NVGcontext* vg, const char* s, float y, int size) {
	float x = textRenderWidth(vg, s, size);
	x = std::max(0.0f, _size.x - x);
	x /= 2.0;
	drawText(vg, s, x, y, size);
}

float ReftoneDisplay::textRenderWidth(NVGcontext* vg, const char* s, int size) {
	// nvgSave(vg);
	// nvgFontSize(vg, size);
	// float w = nvgTextBounds(vg, 0, 0, s, NULL, NULL);
	// nvgRestore(vg);
	// return w - size/4.0;
	return strlen(s) * (size / 2.1);
}


ReftoneWidget::ReftoneWidget() {
	Reftone *module = new Reftone();
	setModule(module);
	box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Reftone.svg")));
		addChild(panel);
	}

	{
		auto inset = Vec(3.5, 18);
		auto size = Vec(38, 48);
		auto display = new ReftoneDisplay(module, size);
		display->box.pos = inset;
		display->box.size = size;
		addChild(display);
	}

	addChild(createScrew<ScrewSilver>(Vec(0, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 15, 365)));

	// generated by svg_widgets.rb
	auto pitchParamPosition = Vec(9.5, 89.5);
	auto octaveParamPosition = Vec(9.5, 143.5);
	auto fineParamPosition = Vec(9.5, 197.5);

	auto cvOutputPosition = Vec(10.5, 239.0);
	auto outOutputPosition = Vec(10.5, 274.0);
	// end generated by svg_widgets.rb

	{
		auto w = createParam<Knob26>(pitchParamPosition, module, Reftone::PITCH_PARAM, 0.0, 11.0, 9.0);
		dynamic_cast<Knob*>(w)->snap = true;
		addParam(w);
	}
	{
		auto w = createParam<Knob26>(octaveParamPosition, module, Reftone::OCTAVE_PARAM, 1.0, 8.0, 4.0);
		dynamic_cast<Knob*>(w)->snap = true;
		addParam(w);
	}
	addParam(createParam<Knob26>(fineParamPosition, module, Reftone::FINE_PARAM, -0.99, 0.99, 0.0));

	addOutput(createOutput<Port24>(cvOutputPosition, module, Reftone::CV_OUTPUT));
	addOutput(createOutput<Port24>(outOutputPosition, module, Reftone::OUT_OUTPUT));
}
