
#include "Reftone.hpp"

void Reftone::processAll(const ProcessArgs& args) {
	// C4 -- the pitch.hpp reference frequency -- in knob values:
	const float referencePitch = 0.0f;
	const float referenceOctave = 4.0f;

	if (!(
		_pitch == params[PITCH_PARAM].getValue() &&
		_octave == params[OCTAVE_PARAM].getValue() &&
		_fine == params[FINE_PARAM].getValue()
	)) {
		_pitch = params[PITCH_PARAM].getValue();
		_octave = params[OCTAVE_PARAM].getValue();
		_fine = params[FINE_PARAM].getValue();
		_frequency = semitoneToFrequency(referenceSemitone + 12.0f*roundf(_octave - referenceOctave) + roundf(_pitch - referencePitch) + _fine);
		_cv = frequencyToCV(_frequency);
	}

	if (outputs[CV_OUTPUT].isConnected()) {
		outputs[CV_OUTPUT].setVoltage(_cv);
	}
	else {
		outputs[CV_OUTPUT].setVoltage(0.0);
	}

	if (outputs[OUT_OUTPUT].isConnected()) {
		_sine.setFrequency(_frequency);
		outputs[OUT_OUTPUT].setVoltage(_sine.next() * 5.0f);
	}
	else {
		outputs[OUT_OUTPUT].setVoltage(0.0);
	}
}

struct ReftoneDisplay : TransparentWidget {
	const NVGcolor _textColor = nvgRGBA(0x00, 0xff, 0x00, 0xee);

	Reftone* _module;
	const Vec _size;
	std::string _fontPath;

	ReftoneDisplay(
		Reftone* module,
		Vec size
	)
	: _module(module)
	, _size(size)
	, _fontPath(asset::plugin(pluginInstance, "res/fonts/inconsolata-bold.ttf"))
	{
	}

	void draw(const DrawArgs& args) override;
	void drawBackground(const DrawArgs& args);
	void drawText(const DrawArgs& args, const char* s, float x, float y, int size);
	void drawCenteredText(const DrawArgs& args, const char* s, float y, int size);
	float textRenderWidth(const DrawArgs& args, const char* s, int size);
};

void ReftoneDisplay::draw(const DrawArgs& args) {
	int mPitch = 9;
	int mOctave = 4;
	float mFine = 0.0f;
	float mFrequency = 440.0f;
	if (_module) {
		mPitch = _module->_pitch;
		mOctave = _module->_octave;
		mFine = _module->_fine;
		mFrequency = _module->_frequency;
	}

	std::string octave = std::to_string(mOctave);
	std::string fine = format("%s%02d", mFine < 0.0 ? "-" : "+", abs((int)(mFine * 100)));
	std::string frequency = format(mFrequency >= 1000.0 ? "%0.0f" : "%0.1f", mFrequency);
	const char* pitch = NULL;
	const char* sharpFlat = NULL;
	switch (mPitch) {
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

	nvgSave(args.vg);
	if (_module && _module->isBypassed()) {
		drawBackground(args);
	}
	else {
		nvgGlobalTint(args.vg, color::WHITE);
		drawBackground(args);
		if (sharpFlat) {
			drawText(args, pitch, 3, 20, 28);
			drawText(args, sharpFlat, 16, 12, 16);
			drawText(args, octave.c_str(), 22, 20, 28);
		}
		else {
			drawCenteredText(args, (pitch + octave).c_str(), 20, 28);
		}
		drawCenteredText(args, fine.c_str(), 32.5, 14);
		drawCenteredText(args, frequency.c_str(), 45, 14);
	}
	nvgRestore(args.vg);
}

void ReftoneDisplay::drawBackground(const DrawArgs& args) {
	nvgSave(args.vg);
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0, 0, _size.x, _size.y);
	nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
	nvgFill(args.vg);
	nvgStrokeColor(args.vg, nvgRGBA(0x50, 0x50, 0x50, 0xff));
	nvgStroke(args.vg);
	nvgRestore(args.vg);
}

void ReftoneDisplay::drawText(const DrawArgs& args, const char* s, float x, float y, int size) {
	std::shared_ptr<Font> font = APP->window->loadFont(_fontPath);
	nvgSave(args.vg);
	nvgTranslate(args.vg, x, y);
	nvgFontSize(args.vg, size);
	nvgFontFaceId(args.vg, font->handle);
	nvgFillColor(args.vg, _textColor);
	nvgText(args.vg, 0, 0, s, NULL);
	nvgRestore(args.vg);
}

void ReftoneDisplay::drawCenteredText(const DrawArgs& args, const char* s, float y, int size) {
	float x = textRenderWidth(args, s, size);
	x = std::max(0.0f, _size.x - x);
	x /= 2.0;
	drawText(args, s, x, y, size);
}

float ReftoneDisplay::textRenderWidth(const DrawArgs& args, const char* s, int size) {
	// nvgSave(args.vg);
	// nvgFontSize(args.vg, size);
	// float w = nvgTextBounds(args.vg, 0, 0, s, NULL, NULL);
	// nvgRestore(args.vg);
	// return w - size/4.0;
	return strlen(s) * (size / 2.1);
}

struct ReftoneWidget : BGModuleWidget {
	static constexpr int hp = 3;

	ReftoneWidget(Reftone* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Reftone");
		createScrews();

		{
			auto inset = Vec(3.5, 18);
			auto size = Vec(38, 48);
			auto display = new ReftoneDisplay(module, size);
			display->box.pos = inset;
			display->box.size = size;
			addChild(display);
		}

		// generated by svg_widgets.rb
		auto pitchParamPosition = Vec(9.5, 89.5);
		auto octaveParamPosition = Vec(9.5, 143.5);
		auto fineParamPosition = Vec(9.5, 197.5);

		auto cvOutputPosition = Vec(10.5, 239.0);
		auto outOutputPosition = Vec(10.5, 274.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(pitchParamPosition, module, Reftone::PITCH_PARAM));
		addParam(createParam<Knob26>(octaveParamPosition, module, Reftone::OCTAVE_PARAM));
		addParam(createParam<Knob26>(fineParamPosition, module, Reftone::FINE_PARAM));

		addOutput(createOutput<Port24>(cvOutputPosition, module, Reftone::CV_OUTPUT));
		addOutput(createOutput<Port24>(outOutputPosition, module, Reftone::OUT_OUTPUT));
	}
};

Model* modelReftone = bogaudio::createModel<Reftone, ReftoneWidget>("Bogaudio-Reftone", "REFTONE", "Precision pitch CV generator", "Tuner");
