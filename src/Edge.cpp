
#include "Edge.hpp"

void Edge::reset() {
	for (int c = 0; c < _channels; ++c) {
		_state[c] = LOW_STATE;
		_timer[c].reset();
		_riseOutputPulseGen[c].process(10.0f);
		_fallOutputPulseGen[c].process(10.0f);
	}
}

int Edge::channels() {
	return inputs[IN_INPUT].getChannels();
}

void Edge::addChannel(int c) {
	_state[c] = LOW_STATE;
}

void Edge::modulate() {
	_riseThreshold = params[RISE_PARAM].getValue() * 10.0f;
	_fallThreshold = params[FALL_PARAM].getValue() * 10.0f;
	_holdSeconds = params[HOLD_PARAM].getValue();
	_holdSeconds *= _holdSeconds;
}

void Edge::processAlways(const ProcessArgs& args) {
	_highLightSum = 0;
}

void Edge::processAll(const ProcessArgs& args) {
	outputs[HIGH_OUTPUT].setChannels(_channels);
	outputs[RISE_OUTPUT].setChannels(_channels);
	outputs[FALL_OUTPUT].setChannels(_channels);
}

void Edge::processChannel(const ProcessArgs& args, int c) {
	float in = inputs[IN_INPUT].getPolyVoltage(c);

	switch (_state[c]) {
		case LOW_STATE: {
			if (in >= _riseThreshold) {
				_state[c] = HIGH_STATE;
				_timer[c].reset();
				_timer[c].setParams(APP->engine->getSampleRate(), _holdSeconds);
				_riseOutputPulseGen[c].trigger(0.001f);
			}
			break;
		}

		case HIGH_STATE: {
			bool expired = !_timer[c].next();
			++_highLightSum;

			if (_fallThreshold > _riseThreshold && in > _fallThreshold) {
				_state[c] = HIGH2_STATE;
			}
			else if (in < std::min(_riseThreshold, _fallThreshold) && expired) {
				_state[c] = _riseThreshold > _fallThreshold ? LOW_STATE : LOW2_STATE;
				_fallOutputPulseGen[c].trigger(0.001f);
			}
			break;
		}

		case HIGH2_STATE: {
			bool expired = !_timer[c].next();
			++_highLightSum;

			if (in < std::max(_riseThreshold, _fallThreshold) && expired) {
				_state[c] = _riseThreshold > _fallThreshold ? LOW_STATE : LOW2_STATE;
				_fallOutputPulseGen[c].trigger(0.001f);
			}
			break;
		}

		case LOW2_STATE: {
			if (in < std::min(_riseThreshold, _fallThreshold)) {
				_state[c] = LOW_STATE;
			}
			break;
		}
	}

	outputs[HIGH_OUTPUT].setVoltage((_state[c] == HIGH_STATE || _state[c] == HIGH2_STATE) * 5.0f, c);
	float st = APP->engine->getSampleTime();
	outputs[RISE_OUTPUT].setVoltage(_riseOutputPulseGen[c].process(st) * 5.0f, c);
	outputs[FALL_OUTPUT].setVoltage(_fallOutputPulseGen[c].process(st) * 5.0f, c);
}

void Edge::postProcessAlways(const ProcessArgs& args) {
	lights[HIGH_LIGHT].value = _highLightSum * _inverseChannels;
}

struct EdgeWidget : BGModuleWidget {
	static constexpr int hp = 3;

	EdgeWidget(Edge* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Edge");
		createScrews();

		// generated by svg_widgets.rb
		auto riseParamPosition = Vec(8.0, 38.0);
		auto fallParamPosition = Vec(8.0, 102.0);
		auto holdParamPosition = Vec(14.5, 162.0);

		auto inInputPosition = Vec(10.5, 194.0);

		auto highOutputPosition = Vec(10.5, 232.0);
		auto riseOutputPosition = Vec(10.5, 267.0);
		auto fallOutputPosition = Vec(10.5, 302.0);

		auto highLightPosition = Vec(7.5, 258.3);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob29>(riseParamPosition, module, Edge::RISE_PARAM));
		addParam(createParam<Knob29>(fallParamPosition, module, Edge::FALL_PARAM));
		addParam(createParam<Knob16>(holdParamPosition, module, Edge::HOLD_PARAM));

		addInput(createInput<Port24>(inInputPosition, module, Edge::IN_INPUT));

		addOutput(createOutput<Port24>(highOutputPosition, module, Edge::HIGH_OUTPUT));
		addOutput(createOutput<Port24>(riseOutputPosition, module, Edge::RISE_OUTPUT));
		addOutput(createOutput<Port24>(fallOutputPosition, module, Edge::FALL_OUTPUT));

		addChild(createLight<BGSmallLight<GreenLight>>(highLightPosition, module, Edge::HIGH_LIGHT));
	}
};

Model* modelEdge = createModel<Edge, EdgeWidget>("Bogaudio-Edge", "EDGE", "Edge detector, gate-to-trigger, comparator", "Logic", "Polyphonic");
