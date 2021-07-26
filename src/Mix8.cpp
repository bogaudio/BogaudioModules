
#include "Mix8.hpp"

#define POLY_OFFSET "poly_channel_offset"

json_t* Mix8::toJson(json_t* root) {
	root = DimmableMixerModule::toJson(root);
	json_object_set_new(root, POLY_OFFSET, json_integer(_polyChannelOffset));
	return root;
}

void Mix8::fromJson(json_t* root) {
	DimmableMixerModule::fromJson(root);
	json_t* o = json_object_get(root, POLY_OFFSET);
	if (o) {
		_polyChannelOffset = json_integer_value(o);
	}
}

void Mix8::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int i = 0; i < 8; ++i) {
		_channels[i]->setSampleRate(sr);
		_panSLs[i].setParams(sr, MIXER_PAN_SLEW_MS, 2.0f);
	}
	_slewLimiter.setParams(sr, MixerChannel::levelSlewTimeMS, MixerChannel::maxDecibels - MixerChannel::minDecibels);
	_levelCVSL.setParams(sr, MixerChannel::levelSlewTimeMS, 1.0f);
	_rms.setSampleRate(sr);
}

void Mix8::processAll(const ProcessArgs& args) {
	Mix8ExpanderMessage* toExp = &_dummyExpanderMessage;
	Mix8ExpanderMessage* fromExp = &_dummyExpanderMessage;
	if (expanderConnected()) {
		toExp = toExpander();
		fromExp = fromExpander();
	}

	if (!(
		inputs[IN1_INPUT].isConnected() ||
		inputs[IN2_INPUT].isConnected() ||
		inputs[IN3_INPUT].isConnected() ||
		inputs[IN4_INPUT].isConnected() ||
		inputs[IN5_INPUT].isConnected() ||
		inputs[IN6_INPUT].isConnected() ||
		inputs[IN7_INPUT].isConnected() ||
		inputs[IN8_INPUT].isConnected()
	)) {
		if (_wasActive > 0) {
			--_wasActive;
			for (int i = 0; i < 8; ++i) {
				_channels[i]->reset();
				toExp->active[i] = false;
			}
			_rmsLevel = 0.0f;
			outputs[L_OUTPUT].setVoltage(0.0f);
			outputs[R_OUTPUT].setVoltage(0.0f);
		}
		return;
	}
	_wasActive = 2;

	bool solo =
		params[MUTE1_PARAM].getValue() > 1.5f ||
		params[MUTE2_PARAM].getValue() > 1.5f ||
		params[MUTE3_PARAM].getValue() > 1.5f ||
		params[MUTE4_PARAM].getValue() > 1.5f ||
		params[MUTE5_PARAM].getValue() > 1.5f ||
		params[MUTE6_PARAM].getValue() > 1.5f ||
		params[MUTE7_PARAM].getValue() > 1.5f ||
		params[MUTE8_PARAM].getValue() > 1.5f;

	{
		float sample = 0.0f;
		if (_polyChannelOffset >= 0) {
			sample = inputs[IN1_INPUT].getPolyVoltage(_polyChannelOffset);
		} else {
			sample = inputs[IN1_INPUT].getVoltageSum();
		}
		_channels[0]->next(sample, solo, 0, _linearCV);
		toExp->preFader[0] = sample;
		toExp->active[0] = inputs[IN1_INPUT].isConnected();

		for (int i = 1; i < 8; ++i) {
			float sample = 0.0f;
			if (inputs[IN1_INPUT + 3 * i].isConnected()) {
				sample = inputs[IN1_INPUT + 3 * i].getVoltageSum();
				_channels[i]->next(sample, solo, 0, _linearCV);
				_channelActive[i] = true;
			}
			else if (_polyChannelOffset >= 0) {
				sample = inputs[IN1_INPUT].getPolyVoltage(_polyChannelOffset + i);
				_channels[i]->next(sample, solo, 0, _linearCV);
				_channelActive[i] = true;
			}
			else if (_channelActive[i]) {
				_channels[i]->reset();
				_channelActive[i] = false;
			}
			toExp->preFader[i] = sample;
			toExp->active[i] = _channelActive[i];
		}
	}

	float levelCV = 1.0f;
	if (inputs[MIX_CV_INPUT].isConnected()) {
		levelCV = clamp(inputs[MIX_CV_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
	}
	float level = Amplifier::minDecibels;
	if (params[MIX_MUTE_PARAM].getValue() < 0.5f) {
		level = params[MIX_PARAM].getValue();
		if (!_linearCV) {
			level *= levelCV;
		}
		level *= MixerChannel::maxDecibels - MixerChannel::minDecibels;
		level += MixerChannel::minDecibels;
		if (params[MIX_DIM_PARAM].getValue() > 0.5f) {
			level = std::max(Amplifier::minDecibels, level - _dimDb);
		}
	}
	_amplifier.setLevel(_slewLimiter.next(level));
	levelCV = _levelCVSL.next(levelCV);

	float outs[8];
	for (int i = 0; i < 8; ++i) {
		toExp->postFader[i] = outs[i] = _channels[i]->out;
	}

	float mono = 0.0f;
	float left = 0.0f;
	float right = 0.0f;
	if (expanderConnected()) {
		mono += fromExp->returnA[0] + fromExp->returnB[0];
		left += fromExp->returnA[0] + fromExp->returnB[0];
		right += fromExp->returnA[1] + fromExp->returnB[1];
		std::copy(fromExp->postEQ, fromExp->postEQ + 8, outs);
	}

	for (int i = 0; i < 8; ++i) {
		mono += outs[i];
	}
	mono = _amplifier.next(mono);
	if (_linearCV) {
		mono *= levelCV;
	}
	mono = _saturator.next(mono);
	_rmsLevel = _rms.next(mono) / 5.0f;

	if (outputs[L_OUTPUT].isConnected() && outputs[R_OUTPUT].isConnected()) {
		for (int i = 0; i < 8; ++i) {
			float pan = clamp(params[PAN1_PARAM + 3 * i].getValue(), -1.0f, 1.0f);
			if (inputs[PAN1_INPUT + 3 * i].isConnected()) {
				pan *= clamp(inputs[PAN1_INPUT + 3 * i].getVoltage() / 5.0f, -1.0f, 1.0f);
			}
			_panners[i].setPan(_panSLs[i].next(pan));
			float l, r;
			_panners[i].next(outs[i], l, r);
			left += l;
			right += r;
		}

		left = _amplifier.next(left);
		if (_linearCV) {
			left *= levelCV;
		}
		left = _saturator.next(left);
		outputs[L_OUTPUT].setVoltage(left);

		right = _amplifier.next(right);
		if (_linearCV) {
			right *= levelCV;
		}
		right = _saturator.next(right);
		outputs[R_OUTPUT].setVoltage(right);
	}
	else {
		outputs[L_OUTPUT].setVoltage(mono);
		outputs[R_OUTPUT].setVoltage(mono);
	}
}

struct Mix8Widget : DimmableMixerWidget {
	static constexpr int hp = 27;

	Mix8Widget(Mix8* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "Mix8");
		createScrews();

		// generated by svg_widgets.rb
		auto level1ParamPosition = Vec(17.5, 32.0);
		auto mute1ParamPosition = Vec(17.5, 185.0);
		auto pan1ParamPosition = Vec(18.5, 223.0);
		auto level2ParamPosition = Vec(61.5, 32.0);
		auto mute2ParamPosition = Vec(61.5, 185.0);
		auto pan2ParamPosition = Vec(62.5, 223.0);
		auto level3ParamPosition = Vec(105.5, 32.0);
		auto mute3ParamPosition = Vec(105.5, 185.0);
		auto pan3ParamPosition = Vec(106.5, 223.0);
		auto level4ParamPosition = Vec(149.5, 32.0);
		auto mute4ParamPosition = Vec(149.5, 185.0);
		auto pan4ParamPosition = Vec(150.5, 223.0);
		auto level5ParamPosition = Vec(193.5, 32.0);
		auto mute5ParamPosition = Vec(193.5, 185.0);
		auto pan5ParamPosition = Vec(194.5, 223.0);
		auto level6ParamPosition = Vec(237.5, 32.0);
		auto mute6ParamPosition = Vec(237.5, 185.0);
		auto pan6ParamPosition = Vec(238.5, 223.0);
		auto level7ParamPosition = Vec(281.5, 32.0);
		auto mute7ParamPosition = Vec(281.5, 185.0);
		auto pan7ParamPosition = Vec(282.5, 223.0);
		auto level8ParamPosition = Vec(325.5, 32.0);
		auto mute8ParamPosition = Vec(325.5, 185.0);
		auto pan8ParamPosition = Vec(326.5, 223.0);
		auto mixParamPosition = Vec(369.5, 32.0);
		auto mixMuteParamPosition = Vec(369.5, 185.0);
		auto mixDimParamPosition = Vec(369.5, 218.0);

		auto cv1InputPosition = Vec(14.5, 255.0);
		auto pan1InputPosition = Vec(14.5, 290.0);
		auto in1InputPosition = Vec(14.5, 325.0);
		auto cv2InputPosition = Vec(58.5, 255.0);
		auto pan2InputPosition = Vec(58.5, 290.0);
		auto in2InputPosition = Vec(58.5, 325.0);
		auto cv3InputPosition = Vec(102.5, 255.0);
		auto pan3InputPosition = Vec(102.5, 290.0);
		auto in3InputPosition = Vec(102.5, 325.0);
		auto cv4InputPosition = Vec(146.5, 255.0);
		auto pan4InputPosition = Vec(146.5, 290.0);
		auto in4InputPosition = Vec(146.5, 325.0);
		auto cv5InputPosition = Vec(190.5, 255.0);
		auto pan5InputPosition = Vec(190.5, 290.0);
		auto in5InputPosition = Vec(190.5, 325.0);
		auto cv6InputPosition = Vec(234.5, 255.0);
		auto pan6InputPosition = Vec(234.5, 290.0);
		auto in6InputPosition = Vec(234.5, 325.0);
		auto cv7InputPosition = Vec(278.5, 255.0);
		auto pan7InputPosition = Vec(278.5, 290.0);
		auto in7InputPosition = Vec(278.5, 325.0);
		auto cv8InputPosition = Vec(322.5, 255.0);
		auto pan8InputPosition = Vec(322.5, 290.0);
		auto in8InputPosition = Vec(322.5, 325.0);
		auto mixCvInputPosition = Vec(366.5, 252.0);

		auto lOutputPosition = Vec(366.5, 290.0);
		auto rOutputPosition = Vec(366.5, 325.0);
		// end generated by svg_widgets.rb

		addSlider(level1ParamPosition, module, Mix8::LEVEL1_PARAM, module ? &module->_channels[0]->rms : NULL);
		addParam(createParam<SoloMuteButton>(mute1ParamPosition, module, Mix8::MUTE1_PARAM));
		addParam(createParam<Knob16>(pan1ParamPosition, module, Mix8::PAN1_PARAM));
		addSlider(level2ParamPosition, module, Mix8::LEVEL2_PARAM, module ? &module->_channels[1]->rms : NULL);
		addParam(createParam<SoloMuteButton>(mute2ParamPosition, module, Mix8::MUTE2_PARAM));
		addParam(createParam<Knob16>(pan2ParamPosition, module, Mix8::PAN2_PARAM));
		addSlider(level3ParamPosition, module, Mix8::LEVEL3_PARAM, module ? &module->_channels[2]->rms : NULL);
		addParam(createParam<SoloMuteButton>(mute3ParamPosition, module, Mix8::MUTE3_PARAM));
		addParam(createParam<Knob16>(pan3ParamPosition, module, Mix8::PAN3_PARAM));
		addSlider(level4ParamPosition, module, Mix8::LEVEL4_PARAM, module ? &module->_channels[3]->rms : NULL);
		addParam(createParam<SoloMuteButton>(mute4ParamPosition, module, Mix8::MUTE4_PARAM));
		addParam(createParam<Knob16>(pan4ParamPosition, module, Mix8::PAN4_PARAM));
		addSlider(level5ParamPosition, module, Mix8::LEVEL5_PARAM, module ? &module->_channels[4]->rms : NULL);
		addParam(createParam<SoloMuteButton>(mute5ParamPosition, module, Mix8::MUTE5_PARAM));
		addParam(createParam<Knob16>(pan5ParamPosition, module, Mix8::PAN5_PARAM));
		addSlider(level6ParamPosition, module, Mix8::LEVEL6_PARAM, module ? &module->_channels[5]->rms : NULL);
		addParam(createParam<SoloMuteButton>(mute6ParamPosition, module, Mix8::MUTE6_PARAM));
		addParam(createParam<Knob16>(pan6ParamPosition, module, Mix8::PAN6_PARAM));
		addSlider(level7ParamPosition, module, Mix8::LEVEL7_PARAM, module ? &module->_channels[6]->rms : NULL);
		addParam(createParam<SoloMuteButton>(mute7ParamPosition, module, Mix8::MUTE7_PARAM));
		addParam(createParam<Knob16>(pan7ParamPosition, module, Mix8::PAN7_PARAM));
		addSlider(level8ParamPosition, module, Mix8::LEVEL8_PARAM, module ? &module->_channels[7]->rms : NULL);
		addParam(createParam<SoloMuteButton>(mute8ParamPosition, module, Mix8::MUTE8_PARAM));
		addParam(createParam<Knob16>(pan8ParamPosition, module, Mix8::PAN8_PARAM));
		addSlider(mixParamPosition, module, Mix8::MIX_PARAM, module ? &module->_rmsLevel : NULL);
		{
			auto b = createParam<MuteButton>(mixMuteParamPosition, module, Mix8::MIX_MUTE_PARAM);
			b->setRandomize(false);
			addParam(b);
		}
		{
			auto b = createParam<MuteButton>(mixDimParamPosition, module, Mix8::MIX_DIM_PARAM);
			b->setRandomize(false);
			addParam(b);
		}

		addInput(createInput<Port24>(cv1InputPosition, module, Mix8::CV1_INPUT));
		addInput(createInput<Port24>(pan1InputPosition, module, Mix8::PAN1_INPUT));
		addInput(createInput<Port24>(in1InputPosition, module, Mix8::IN1_INPUT));
		addInput(createInput<Port24>(cv2InputPosition, module, Mix8::CV2_INPUT));
		addInput(createInput<Port24>(pan2InputPosition, module, Mix8::PAN2_INPUT));
		addInput(createInput<Port24>(in2InputPosition, module, Mix8::IN2_INPUT));
		addInput(createInput<Port24>(cv3InputPosition, module, Mix8::CV3_INPUT));
		addInput(createInput<Port24>(pan3InputPosition, module, Mix8::PAN3_INPUT));
		addInput(createInput<Port24>(in3InputPosition, module, Mix8::IN3_INPUT));
		addInput(createInput<Port24>(cv4InputPosition, module, Mix8::CV4_INPUT));
		addInput(createInput<Port24>(pan4InputPosition, module, Mix8::PAN4_INPUT));
		addInput(createInput<Port24>(in4InputPosition, module, Mix8::IN4_INPUT));
		addInput(createInput<Port24>(cv5InputPosition, module, Mix8::CV5_INPUT));
		addInput(createInput<Port24>(pan5InputPosition, module, Mix8::PAN5_INPUT));
		addInput(createInput<Port24>(in5InputPosition, module, Mix8::IN5_INPUT));
		addInput(createInput<Port24>(cv6InputPosition, module, Mix8::CV6_INPUT));
		addInput(createInput<Port24>(pan6InputPosition, module, Mix8::PAN6_INPUT));
		addInput(createInput<Port24>(in6InputPosition, module, Mix8::IN6_INPUT));
		addInput(createInput<Port24>(cv7InputPosition, module, Mix8::CV7_INPUT));
		addInput(createInput<Port24>(pan7InputPosition, module, Mix8::PAN7_INPUT));
		addInput(createInput<Port24>(in7InputPosition, module, Mix8::IN7_INPUT));
		addInput(createInput<Port24>(cv8InputPosition, module, Mix8::CV8_INPUT));
		addInput(createInput<Port24>(pan8InputPosition, module, Mix8::PAN8_INPUT));
		addInput(createInput<Port24>(in8InputPosition, module, Mix8::IN8_INPUT));
		addInput(createInput<Port24>(mixCvInputPosition, module, Mix8::MIX_CV_INPUT));

		addOutput(createOutput<Port24>(lOutputPosition, module, Mix8::L_OUTPUT));
		addOutput(createOutput<Port24>(rOutputPosition, module, Mix8::R_OUTPUT));
	}

	void addSlider(Vec position, Mix8* module, int id, float* rms) {
		auto slider = createParam<VUSlider151>(position, module, id);
		if (rms) {
			dynamic_cast<VUSlider*>(slider)->setVULevel(rms);
		}
		addParam(slider);
	}

	void contextMenu(Menu* menu) override {
		DimmableMixerWidget::contextMenu(menu);
		auto m = dynamic_cast<Mix8*>(module);
		assert(m);
		OptionsMenuItem* mi = new OptionsMenuItem("Input 1 poly spread");
		mi->addItem(OptionMenuItem("None", [m]() { return m->_polyChannelOffset == -1; }, [m]() { m->_polyChannelOffset = -1; }));
		mi->addItem(OptionMenuItem("Channels 1-8", [m]() { return m->_polyChannelOffset == 0; }, [m]() { m->_polyChannelOffset = 0; }));
		mi->addItem(OptionMenuItem("Channels 9-16", [m]() { return m->_polyChannelOffset == 8; }, [m]() { m->_polyChannelOffset = 8; }));
		OptionsMenuItem::addToMenu(mi, menu);
	}
};

Model* modelMix8 = bogaudio::createModel<Mix8, Mix8Widget>("Bogaudio-Mix8", "MIX8", "8-channel mixer and panner", "Mixer", "Panning");
