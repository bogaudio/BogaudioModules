
#include "Lmtr.hpp"

#define ATTACK_MS "attack_ms"
#define RELEASE_MS "release_ms"
#define THRESHOLD_RANGE "threshold_range"

void Lmtr::Engine::sampleRateChange() {
	detector.setSampleRate(APP->engine->getSampleRate());
}

float Lmtr::ThresholdParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	v *= 30.0f;
	v -= 24.0f;
	v *= dynamic_cast<Lmtr*>(module)->_thresholdRange;
	return v;
}

void Lmtr::ThresholdParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}
	Lmtr* m = dynamic_cast<Lmtr*>(module);
	v /= m->_thresholdRange;
	v = clamp(v, -24.0f, 6.0f);
	v += 24.0f;
	v /= 30.0f;
	setValue(v);
}

void Lmtr::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

json_t* Lmtr::saveToJson(json_t* root) {
	json_object_set_new(root, ATTACK_MS, json_real(_attackMs));
	json_object_set_new(root, RELEASE_MS, json_real(_releaseMs));
	json_object_set_new(root, THRESHOLD_RANGE, json_real(_thresholdRange));
	return root;
}

void Lmtr::loadFromJson(json_t* root) {
	json_t* a = json_object_get(root, ATTACK_MS);
	if (a) {
		_attackMs = std::max(0.0f, (float)json_real_value(a));
	}

	json_t* r = json_object_get(root, RELEASE_MS);
	if (r) {
		_releaseMs = std::max(0.0f, (float)json_real_value(r));
	}

	json_t* tr = json_object_get(root, THRESHOLD_RANGE);
	if (tr) {
		_thresholdRange = std::max(0.0f, (float)json_real_value(tr));
	}
}

bool Lmtr::active() {
	return outputs[LEFT_OUTPUT].isConnected() || outputs[RIGHT_OUTPUT].isConnected();
}

int Lmtr::channels() {
	return inputs[LEFT_INPUT].getChannels();
}

void Lmtr::addChannel(int c) {
	_engines[c] = new Engine();
	_engines[c]->sampleRateChange();
}

void Lmtr::removeChannel(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

void Lmtr::modulate() {
	_softKnee = params[KNEE_PARAM].getValue() > 0.5f;
}

void Lmtr::modulateChannel(int c) {
	Engine& e = *_engines[c];

	e.thresholdDb = params[THRESHOLD_PARAM].getValue();
	if (inputs[THRESHOLD_INPUT].isConnected()) {
		e.thresholdDb *= clamp(inputs[THRESHOLD_INPUT].getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	e.thresholdDb *= 30.0f;
	e.thresholdDb -= 24.0f;
	e.thresholdDb *= _thresholdRange;

	float outGain = params[OUTPUT_GAIN_PARAM].getValue();
	if (inputs[OUTPUT_GAIN_INPUT].isConnected()) {
		outGain = clamp(outGain + inputs[OUTPUT_GAIN_INPUT].getPolyVoltage(c) / 5.0f, 0.0f, 1.0f);
	}
	outGain *= 24.0f;
	if (e.outGain != outGain) {
		e.outGain = outGain;
		e.outLevel = decibelsToAmplitude(e.outGain);
	}

	float sr = APP->engine->getSampleRate();
	e.attackSL.setParams(sr, _attackMs);
	e.releaseSL.setParams(sr, _releaseMs);
}

void Lmtr::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	float leftInput = inputs[LEFT_INPUT].getPolyVoltage(c);
	float rightInput = inputs[RIGHT_INPUT].getPolyVoltage(c);
	float env = e.detector.next(leftInput + rightInput);
	if (env > e.lastEnv) {
		env = e.attackSL.next(env, e.lastEnv);
	}
	else {
		env = e.releaseSL.next(env, e.lastEnv);
	}
	e.lastEnv = env;

	float detectorDb = amplitudeToDecibels(env / 5.0f);
	float compressionDb = e.compressor.compressionDb(detectorDb, e.thresholdDb, Compressor::maxEffectiveRatio, _softKnee);
	e.amplifier.setLevel(-compressionDb);
	if (outputs[LEFT_OUTPUT].isConnected()) {
		outputs[LEFT_OUTPUT].setChannels(_channels);
		outputs[LEFT_OUTPUT].setVoltage(e.saturator.next(e.amplifier.next(leftInput) * e.outLevel), c);
	}
	if (outputs[RIGHT_OUTPUT].isConnected()) {
		outputs[RIGHT_OUTPUT].setChannels(_channels);
		outputs[RIGHT_OUTPUT].setVoltage(e.saturator.next(e.amplifier.next(rightInput) * e.outLevel), c);
	}
}

struct ARQuantity : Quantity {
	typedef std::function<float&(Lmtr* m)> ValueRefFN;

	Lmtr* _module;
	std::string _label;
	float _maxMs;
	float _defaultMs;
	ValueRefFN _moduleValueRef;

	ARQuantity(
		Lmtr* m,
		const char* label,
		float maxMs,
		float defaultMs,
		ValueRefFN moduleValueRef
	)
	: _module(m)
	, _label(label)
	, _maxMs(maxMs)
	, _defaultMs(defaultMs)
	, _moduleValueRef(moduleValueRef)
	{}

	void setValue(float value) override {
		value = clamp(value, getMinValue(), getMaxValue());
		if (_module) {
			_moduleValueRef(_module) = valueToMs(value);
		}
	}

	float getValue() override {
		if (_module) {
			return msToValue(_moduleValueRef(_module));
		}
		return getDefaultValue();
	}

	float getMinValue() override { return 0.0f; }
	float getMaxValue() override { return msToValue(_maxMs); }
	float getDefaultValue() override { return msToValue(_defaultMs); }
	float getDisplayValue() override { return roundf(valueToMs(getValue())); }
	void setDisplayValue(float displayValue) override { setValue(msToValue(displayValue)); }
	std::string getLabel() override { return _label; }
	std::string getUnit() override { return "ms"; }
	float valueToMs(float v) { return v * v * _maxMs; }
	float msToValue(float ms) { return sqrtf(ms / _maxMs); };
};

struct ARSlider : ui::Slider {
	ARSlider(ARQuantity* q) {
		quantity = q; // q now owned.
		box.size.x = 200.0f;
	}
	virtual ~ARSlider() {
		delete quantity;
	}
};

struct AttackMenuItem : MenuItem {
	Lmtr* _module;

	AttackMenuItem(Lmtr* m) : _module(m) {
		this->text = "Attack time";
		this->rightText = "▸";
	}

	Menu* createChildMenu() override {
		Menu* menu = new Menu;
		menu->addChild(new ARSlider(new ARQuantity(
			_module,
			"Attack",
			Lmtr::maxAttackMs,
			Lmtr::defaultAttackMs,
			[](Lmtr* m) -> float& { return m->_attackMs; }
		)));
		return menu;
	}
};

struct ReleaseMenuItem : MenuItem {
	Lmtr* _module;

	ReleaseMenuItem(Lmtr* m) : _module(m) {
		this->text = "Release time";
		this->rightText = "▸";
	}

	Menu* createChildMenu() override {
		Menu* menu = new Menu;
		menu->addChild(new ARSlider(new ARQuantity(
			_module,
			"Release",
			Lmtr::maxReleaseMs,
			Lmtr::defaultReleaseMs,
			[](Lmtr* m) -> float& { return m->_releaseMs; }
		)));
		return menu;
	}
};

struct LmtrWidget : BGModuleWidget {
	static constexpr int hp = 6;

	LmtrWidget(Lmtr* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Lmtr");
		createScrews();

		// generated by svg_widgets.rb
		auto thresholdParamPosition = Vec(26.0, 52.0);
		auto outputGainParamPosition = Vec(26.0, 134.0);
		auto kneeParamPosition = Vec(39.5, 199.5);

		auto leftInputPosition = Vec(16.0, 244.0);
		auto rightInputPosition = Vec(50.0, 244.0);
		auto thresholdInputPosition = Vec(16.0, 280.0);
		auto outputGainInputPosition = Vec(50.0, 280.0);

		auto leftOutputPosition = Vec(16.0, 320.0);
		auto rightOutputPosition = Vec(50.0, 320.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(thresholdParamPosition, module, Lmtr::THRESHOLD_PARAM));
		addParam(createParam<Knob38>(outputGainParamPosition, module, Lmtr::OUTPUT_GAIN_PARAM));
		addParam(createParam<SliderSwitch2State14>(kneeParamPosition, module, Lmtr::KNEE_PARAM));

		addInput(createInput<Port24>(leftInputPosition, module, Lmtr::LEFT_INPUT));
		addInput(createInput<Port24>(rightInputPosition, module, Lmtr::RIGHT_INPUT));
		addInput(createInput<Port24>(thresholdInputPosition, module, Lmtr::THRESHOLD_INPUT));
		addInput(createInput<Port24>(outputGainInputPosition, module, Lmtr::OUTPUT_GAIN_INPUT));

		addOutput(createOutput<Port24>(leftOutputPosition, module, Lmtr::LEFT_OUTPUT));
		addOutput(createOutput<Port24>(rightOutputPosition, module, Lmtr::RIGHT_OUTPUT));
	}

	void contextMenu(Menu* menu) override {
		auto m = dynamic_cast<Lmtr*>(module);
		assert(m);

		menu->addChild(new AttackMenuItem(m));
		menu->addChild(new ReleaseMenuItem(m));

		OptionsMenuItem* tr = new OptionsMenuItem("Threshold range");
		tr->addItem(OptionMenuItem("1x (-24dB to 6dB)", [m]() { return m->_thresholdRange == 1.0f; }, [m]() { m->_thresholdRange = 1.0f; }));
		tr->addItem(OptionMenuItem("2x (-48dB to 12dB)", [m]() { return m->_thresholdRange == 2.0f; }, [m]() { m->_thresholdRange = 2.0f; }));
		OptionsMenuItem::addToMenu(tr, menu);
	}
};

Model* modelLmtr = bogaudio::createModel<Lmtr, LmtrWidget>("Bogaudio-Lmtr", "LMTR", "Limiter", "Limiter", "Dynamics", "Polyphonic");
