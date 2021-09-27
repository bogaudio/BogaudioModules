
#include "mixer.hpp"

const float MixerChannel::maxDecibels = 6.0f;
const float MixerChannel::minDecibels = Amplifier::minDecibels;
const float MixerChannel::levelSlewTimeMS = 5.0f;

void MixerChannel::setSampleRate(float sampleRate) {
	_levelSL.setParams(sampleRate, levelSlewTimeMS, maxDecibels - minDecibels);
	_levelCVSL.setParams(sampleRate, levelSlewTimeMS, 1.0f);
	_rms.setSampleRate(sampleRate);
}

void MixerChannel::reset() {
	out = rms = 0.0f;
}

void MixerChannel::next(float sample, bool solo, int c, bool linearCV) {
	float cv = 1.0f;
	if (_levelInput.isConnected()) {
		cv = clamp(_levelInput.getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}

	float mute = _muteParam.getValue();
	if (_muteInput) {
		mute += clamp(_muteInput->getPolyVoltage(c), 0.0f, 10.0f);
	}
	bool muted = solo ? mute < 2.0f : mute > 0.5f;
	if (muted) {
		_amplifier.setLevel(_levelSL.next(minDecibels));
	}
	else {
		float level = clamp(_levelParam.getValue(), 0.0f, 1.0f);
		if (!linearCV) {
			level *= cv;
		}
		level *= maxDecibels - minDecibels;
		level += minDecibels;
		_amplifier.setLevel(_levelSL.next(level));
	}

	out = _amplifier.next(sample);
	if (linearCV) {
		out *= _levelCVSL.next(cv);
	}
	rms = _rms.next(out / 5.0f);
}


#define LINEAR_CV "linear_cv"

json_t* LinearCVMixerModule::saveToJson(json_t* root) {
	json_object_set_new(root, LINEAR_CV, json_boolean(_linearCV));
	return root;
}

void LinearCVMixerModule::loadFromJson(json_t* root) {
	json_t* l = json_object_get(root, LINEAR_CV);
	if (l) {
		_linearCV = json_boolean_value(l);
	}
}


void LinearCVMixerWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<LinearCVMixerModule*>(module);
	assert(m);
	menu->addChild(new BoolOptionMenuItem("Linear level CV response", [m]() { return &m->_linearCV; }));
}


#define DIM_DB "dim_decibels"

json_t* DimmableMixerModule::saveToJson(json_t* root) {
	root = LinearCVMixerModule::saveToJson(root);
	json_object_set_new(root, DIM_DB, json_real(_dimDb));
	return root;
}

void DimmableMixerModule::loadFromJson(json_t* root) {
	LinearCVMixerModule::loadFromJson(root);
	json_t* o = json_object_get(root, DIM_DB);
	if (o) {
		_dimDb = json_real_value(o);
	}
}


void DimmableMixerWidget::contextMenu(Menu* menu) {
	LinearCVMixerWidget::contextMenu(menu);
	auto m = dynamic_cast<DimmableMixerModule*>(module);
	assert(m);
	OptionsMenuItem* da = new OptionsMenuItem("Dim amount");
	da->addItem(OptionMenuItem("-6 dB", [m]() { return m->_dimDb == 6.0f; }, [m]() { m->_dimDb = 6.0f; }));
	da->addItem(OptionMenuItem("-12 dB", [m]() { return m->_dimDb == 12.0f; }, [m]() { m->_dimDb = 12.0f; }));
	da->addItem(OptionMenuItem("-18 dB", [m]() { return m->_dimDb == 18.0f; }, [m]() { m->_dimDb = 18.0f; }));
	da->addItem(OptionMenuItem("-24 dB", [m]() { return m->_dimDb == 24.0f; }, [m]() { m->_dimDb = 24.0f; }));
	OptionsMenuItem::addToMenu(da, menu);
}


void MuteButton::randomize() {
	if (_randomize) {
		ToggleButton::randomize();
	}
}

void MuteButton::onButton(const event::Button& e) {
	if (!(e.action == GLFW_PRESS && (e.mods & RACK_MOD_MASK) == 0)) {
		return;
	}

	if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
		e.consume(this);
	}
	else {
		ToggleButton::onButton(e);
	}
}


SoloMuteButton::SoloMuteButton() {
	shadow = new CircularShadow();
	addChild(shadow);

	_svgWidget = new SvgWidget();
	addChild(_svgWidget);

	auto svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_0.svg"));
	_frames.push_back(svg);
	_frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1_orange.svg")));
	_frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1_green.svg")));
	_frames.push_back(APP->window->loadSvg(asset::plugin(pluginInstance, "res/button_18px_1_green.svg")));

	_svgWidget->setSvg(svg);
	box.size = _svgWidget->box.size;
	shadow->box.size = _svgWidget->box.size;
	shadow->blurRadius = 1.0;
	shadow->box.pos = Vec(0.0, 1.0);
}

void SoloMuteButton::reset() {
	if (paramQuantity) {
		paramQuantity->setValue(0.0f);
	}
}

void SoloMuteButton::randomize() {
	if (paramQuantity) {
		paramQuantity->setValue(random::uniform() > 0.5f ? 1.0f : 0.0f);
	}
}

void SoloMuteButton::onButton(const event::Button& e) {
	if (!paramQuantity || !(e.action == GLFW_PRESS && (e.mods & RACK_MOD_MASK) == 0)) {
		ParamWidget::onButton(e);
		return;
	}

	float value = paramQuantity->getValue();
	if (value >= 2.0f) {
		paramQuantity->setValue(value - 2.0f);
	}
	else if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
		paramQuantity->setValue(value + 2.0f);
	}
	else {
		paramQuantity->setValue(value > 0.5f ? 0.0f : 1.0f);
	}

	if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
		e.consume(this);
	} else {
		ParamWidget::onButton(e);
	}
}

void SoloMuteButton::onChange(const event::Change& e) {
	assert(_frames.size() == 4);
	if (paramQuantity) {
		float value = paramQuantity->getValue();
		assert(value >= 0.0f && value <= 3.0f);
		_svgWidget->setSvg(_frames[(int)value]);
	}
	ParamWidget::onChange(e);
}
