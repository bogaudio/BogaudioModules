
#include "mixer.hpp"

const float MixerChannel::maxDecibels = 6.0f;
const float MixerChannel::minDecibels = Amplifier::minDecibels;
const float MixerChannel::levelSlewTimeMS = 5.0f;

void MixerChannel::setSampleRate(float sampleRate) {
	_levelSL.setParams(sampleRate, levelSlewTimeMS, maxDecibels - minDecibels);
	_rms.setSampleRate(sampleRate);
}

void MixerChannel::reset() {
	out = rms = 0.0f;
}

void MixerChannel::next(float sample, bool solo, int c) {
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
		if (_levelInput.isConnected()) {
			level *= clamp(_levelInput.getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		level *= maxDecibels - minDecibels;
		level += minDecibels;
		_amplifier.setLevel(_levelSL.next(level));
	}

	out = _amplifier.next(sample);
	rms = _rms.next(out / 5.0f);
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
