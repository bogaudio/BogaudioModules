
#include "Slew.hpp"

bool Slew::active() {
	return inputs[IN_INPUT].isConnected() && outputs[OUT_OUTPUT].isConnected();
}

int Slew::channels() {
	return inputs[IN_INPUT].getChannels();
}

void Slew::modulate() {
	_timeScale = params[SLOW_PARAM].getValue() > 0.5f ? 10.0f : 1.0f;
}

void Slew::modulateChannel(int c) {
	_slew[c].modulate(
		APP->engine->getSampleRate(),
		params[RISE_PARAM],
		&inputs[RISE_INPUT],
		10000.0f * _timeScale,
		params[RISE_SHAPE_PARAM],
		params[FALL_PARAM],
		&inputs[FALL_INPUT],
		10000.0f * _timeScale,
		params[FALL_SHAPE_PARAM],
		c
	);
}

void Slew::processChannel(const ProcessArgs& args, int c) {
	outputs[OUT_OUTPUT].setChannels(_channels);
	outputs[OUT_OUTPUT].setVoltage(_slew[c].next(inputs[IN_INPUT].getPolyVoltage(c)), c);
}

struct SlewWidget : BGModuleWidget {
	static constexpr int hp = 3;

	SlewWidget(Slew* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Slew");
		createScrews();

		// generated by svg_widgets.rb
		auto riseParamPosition = Vec(9.5, 33.0);
		auto riseShapeParamPosition = Vec(14.5, 74.0);
		auto fallParamPosition = Vec(9.5, 151.0);
		auto fallShapeParamPosition = Vec(14.5, 192.0);
		auto slowParamPosition = Vec(31.0, 251.0);

		auto riseInputPosition = Vec(10.5, 103.0);
		auto fallInputPosition = Vec(10.5, 221.0);
		auto inInputPosition = Vec(10.5, 266.0);

		auto outOutputPosition = Vec(10.5, 304.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(riseParamPosition, module, Slew::RISE_PARAM));
		addParam(createParam<Knob16>(riseShapeParamPosition, module, Slew::RISE_SHAPE_PARAM));
		addParam(createParam<Knob26>(fallParamPosition, module, Slew::FALL_PARAM));
		addParam(createParam<Knob16>(fallShapeParamPosition, module, Slew::FALL_SHAPE_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(slowParamPosition, module, Slew::SLOW_PARAM));

		addInput(createInput<Port24>(riseInputPosition, module, Slew::RISE_INPUT));
		addInput(createInput<Port24>(fallInputPosition, module, Slew::FALL_INPUT));
		addInput(createInput<Port24>(inInputPosition, module, Slew::IN_INPUT));

		addOutput(createOutput<Port24>(outOutputPosition, module, Slew::OUT_OUTPUT));
	}
};

Model* modelSlew = bogaudio::createModel<Slew, SlewWidget>("Bogaudio-Slew", "SLEW", "Slew limiter / lag generator / glide", "Slew limiter", "Polyphonic");
