
#include "VCA.hpp"

bool VCA::LevelParamQuantity::isLinear() {
	return dynamic_cast<VCA*>(module)->isLinear();
}

void VCA::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	for (int c = 0; c < maxChannels; ++c) {
		_levelSL1[c].setParams(sampleRate, 5.0f, 1.0f);
		_levelSL2[c].setParams(sampleRate, 5.0f, 1.0f);
	}
}

void VCA::processAll(const ProcessArgs& args) {
	bool linear = isLinear();
	channelStep(inputs[IN1_INPUT], outputs[OUT1_OUTPUT], params[LEVEL1_PARAM], inputs[CV1_INPUT], _amplifier1, _levelSL1, linear);
	channelStep(inputs[IN2_INPUT], outputs[OUT2_OUTPUT], params[LEVEL2_PARAM], inputs[CV2_INPUT], _amplifier2, _levelSL2, linear);
}

void VCA::channelStep(Input& input, Output& output, Param& knob, Input& cv, Amplifier* amplifier, bogaudio::dsp::SlewLimiter* levelSL, bool linear) {
	if (!(input.isConnected() && output.isConnected())) {
		return;
	}

	int n = input.getChannels();
	output.setChannels(n);
	for (int c = 0; c < n; ++c) {
		float level = knob.getValue();
		if (cv.isConnected()) {
			level *= clamp(cv.getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
		}
		level = levelSL[c].next(level);
		if (linear) {
			output.setVoltage(level * input.getVoltage(c), c);
		}
		else {
			level = 1.0f - level;
			level *= Amplifier::minDecibels;
			amplifier[c].setLevel(level);
			output.setVoltage(amplifier[c].next(input.getVoltage(c)), c);
		}
	}
}

struct VCAWidget : BGModuleWidget {
	static constexpr int hp = 3;

	VCAWidget(VCA* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "VCA");
		createScrews();

		// generated by svg_widgets.rb
		auto level1ParamPosition = Vec(9.5, 22.5);
		auto level2ParamPosition = Vec(9.5, 181.5);
		auto linearParamPosition = Vec(25.5, 333.0);

		auto cv1InputPosition = Vec(10.5, 60.0);
		auto in1InputPosition = Vec(10.5, 95.0);
		auto cv2InputPosition = Vec(10.5, 219.0);
		auto in2InputPosition = Vec(10.5, 254.0);

		auto out1OutputPosition = Vec(10.5, 133.0);
		auto out2OutputPosition = Vec(10.5, 292.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob26>(level1ParamPosition, module, VCA::LEVEL1_PARAM));
		addParam(createParam<Knob26>(level2ParamPosition, module, VCA::LEVEL2_PARAM));
		addParam(createParam<IndicatorButtonGreen9>(linearParamPosition, module, VCA::LINEAR_PARAM));

		addInput(createInput<Port24>(cv1InputPosition, module, VCA::CV1_INPUT));
		addInput(createInput<Port24>(in1InputPosition, module, VCA::IN1_INPUT));
		addInput(createInput<Port24>(cv2InputPosition, module, VCA::CV2_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, VCA::IN2_INPUT));

		addOutput(createOutput<Port24>(out1OutputPosition, module, VCA::OUT1_OUTPUT));
		addOutput(createOutput<Port24>(out2OutputPosition, module, VCA::OUT2_OUTPUT));
	}
};

Model* modelVCA = bogaudio::createModel<VCA, VCAWidget>("Bogaudio-VCA", "VCA", "Dual voltage controlled attenuator", "VCA", "Dual", "Polyphonic");
