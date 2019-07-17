
#include "FMOp.hpp"
#include "dsp/pitch.hpp"

#define LINEAR_LEVEL "linearLevel"

float FMOp::RatioParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	if (v < 0.0f) {
		return std::max(1.0f + v, 0.01f);
	}
	v *= 9.0f;
	v += 1.0f;
	return v;
}

void FMOp::RatioParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}

	if (v < 1.0f) {
		v = v - 1.0f;
	}
	else {
		v -= 1.0f;
		v /= 9.0f;
	}
	setValue(v);
}

bool FMOp::LevelParamQuantity::isLinear() {
	return dynamic_cast<FMOp*>(module)->_linearLevel;
}

void FMOp::onReset() {
	_steps = modulationSteps;
	_envelope.reset();
	_gateTrigger.reset();
}

void FMOp::onSampleRateChange() {
	_steps = modulationSteps;
	float sampleRate = APP->engine->getSampleRate();
	_envelope.setSampleRate(sampleRate);
	_phasor.setSampleRate(sampleRate);
	_decimator.setParams(sampleRate, oversample);
	_maxFrequency = 0.475f * sampleRate;
	_feedbackSL.setParams(sampleRate, 5.0f, 1.0f);
	_depthSL.setParams(sampleRate, 5.0f, 1.0f);
	_levelSL.setParams(sampleRate, 10.0f, 1.0f);
	_sustainSL.setParams(sampleRate, 1.0f, 1.0f);
}

json_t* FMOp::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, LINEAR_LEVEL, json_boolean(_linearLevel));
	return root;
}

void FMOp::dataFromJson(json_t* root) {
	json_t* ll = json_object_get(root, LINEAR_LEVEL);
	if (ll) {
		_linearLevel = json_is_true(ll);
	}
}

void FMOp::process(const ProcessArgs& args) {
	if (!outputs[AUDIO_OUTPUT].isConnected()) {
		lights[ENV_TO_LEVEL_LIGHT].value = params[ENV_TO_LEVEL_PARAM].getValue() > 0.5f;
		lights[ENV_TO_FEEDBACK_LIGHT].value = params[ENV_TO_FEEDBACK_PARAM].getValue() > 0.5f;
		lights[ENV_TO_DEPTH_LIGHT].value = params[ENV_TO_DEPTH_PARAM].getValue() > 0.5f;
		return;
	}
	lights[ENV_TO_LEVEL_LIGHT].value = _levelEnvelopeOn;
	lights[ENV_TO_FEEDBACK_LIGHT].value = _feedbackEnvelopeOn;
	lights[ENV_TO_DEPTH_LIGHT].value = _depthEnvelopeOn;

	float pitchIn = 0.0f;
	if (inputs[PITCH_INPUT].isConnected()) {
		pitchIn = inputs[PITCH_INPUT].getVoltage();
	}
	float gateIn = 0.0f;
	if (inputs[GATE_INPUT].isConnected()) {
		gateIn = inputs[GATE_INPUT].getVoltage();
	}

	++_steps;
	if (_steps >= modulationSteps) {
		_steps = 0;

		float ratio = params[RATIO_PARAM].getValue();
		if (ratio < 0.0f) {
			ratio = std::max(1.0f + ratio, 0.01f);
		}
		else {
			ratio *= 9.0f;
			ratio += 1.0f;
		}
		float frequency = pitchIn;
		frequency += params[FINE_PARAM].getValue() / 12.0f;
		frequency = cvToFrequency(frequency);
		frequency *= ratio;
		frequency = clamp(frequency, -_maxFrequency, _maxFrequency);
		_phasor.setFrequency(frequency / (float)oversample);

		bool levelEnvelopeOn = params[ENV_TO_LEVEL_PARAM].getValue() > 0.5f;
		bool feedbackEnvelopeOn = params[ENV_TO_FEEDBACK_PARAM].getValue() > 0.5f;
		bool depthEnvelopeOn = params[ENV_TO_DEPTH_PARAM].getValue() > 0.5f;
		if (_levelEnvelopeOn != levelEnvelopeOn || _feedbackEnvelopeOn != feedbackEnvelopeOn || _depthEnvelopeOn != depthEnvelopeOn) {
			_levelEnvelopeOn = levelEnvelopeOn;
			_feedbackEnvelopeOn = feedbackEnvelopeOn;
			_depthEnvelopeOn = depthEnvelopeOn;
			bool envelopeOn = _levelEnvelopeOn || _feedbackEnvelopeOn || _depthEnvelopeOn;
			if (envelopeOn && !_envelopeOn) {
				_envelope.reset();
			}
			_envelopeOn = envelopeOn;
		}
		if (_envelopeOn) {
			float sustain = params[SUSTAIN_PARAM].getValue();
			if (inputs[SUSTAIN_INPUT].isConnected()) {
				sustain *= clamp(inputs[SUSTAIN_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
			}
			_envelope.setAttack(powf(params[ATTACK_PARAM].getValue(), 2.0f) * 10.f);
			_envelope.setDecay(powf(params[DECAY_PARAM].getValue(), 2.0f) * 10.f);
			_envelope.setSustain(_sustainSL.next(sustain));
			_envelope.setRelease(powf(params[RELEASE_PARAM].getValue(), 2.0f) * 10.f);
		}

		_feedback = params[FEEDBACK_PARAM].getValue();
		if (inputs[FEEDBACK_INPUT].isConnected()) {
			_feedback *= clamp(inputs[FEEDBACK_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}

		_depth = params[DEPTH_PARAM].getValue();
		if (inputs[DEPTH_INPUT].isConnected()) {
			_depth *= clamp(inputs[DEPTH_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}

		_level = params[LEVEL_PARAM].getValue();
		if (inputs[LEVEL_INPUT].isConnected()) {
			_level *= clamp(inputs[LEVEL_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
	}

	float envelope = 0.0f;
	if (_envelopeOn) {
		_gateTrigger.process(gateIn);
		_envelope.setGate(_gateTrigger.isHigh());
		envelope = _envelope.next();
	}

	float feedback = _feedbackSL.next(_feedback);
	if (_feedbackEnvelopeOn) {
		feedback *= envelope;
	}
	bool feedbackOn = feedback > 0.001f;

	float out = _levelSL.next(_level);
	if (_levelEnvelopeOn) {
		out *= envelope;
	}

	float offset = 0.0f;
	if (feedbackOn) {
		offset = feedback * _feedbackDelayedSample;
	}

	if (inputs[FM_INPUT].isConnected()) {
		float depth = _depthSL.next(_depth);
		if (_depthEnvelopeOn) {
			depth *= envelope;
		}
		offset += inputs[FM_INPUT].getVoltage() * depth * 2.0f;
	}

	float sample = 0.0f;
	if (out > 0.0001f) {
		Phasor::phase_delta_t o = Phasor::radiansToPhase(offset);
		if (feedbackOn) {
			if (_oversampleMix < 1.0f) {
				_oversampleMix += oversampleMixIncrement;
			}
		}
		else if (_oversampleMix > 0.0f) {
			_oversampleMix -= oversampleMixIncrement;
		}

		if (_oversampleMix > 0.0f) {
			for (int i = 0; i < oversample; ++i) {
				_phasor.advancePhase();
				_buffer[i] = _sineTable.nextFromPhasor(_phasor, o);
			}
			sample = _oversampleMix * _decimator.next(_buffer);
		}
		else {
			_phasor.advancePhase(oversample);
		}
		if (_oversampleMix < 1.0f) {
			sample += (1.0f - _oversampleMix) * _sineTable.nextFromPhasor(_phasor, o);
		}

		if (_linearLevel) {
			sample *= out;
		}
		else {
			out = (1.0f - out) * Amplifier::minDecibels;
			_amplifier.setLevel(out);
			sample = _amplifier.next(sample);
		}
	}
	else {
		_phasor.advancePhase(oversample);
	}

	outputs[AUDIO_OUTPUT].setVoltage(_feedbackDelayedSample = amplitude * sample);
}

struct FMOpWidget : ModuleWidget {
	static constexpr int hp = 10;

	FMOpWidget(FMOp* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/FMOp.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto ratioParamPosition = Vec(30.0, 45.0);
		auto fineParamPosition = Vec(112.0, 57.0);
		auto attackParamPosition = Vec(107.0, 94.0);
		auto decayParamPosition = Vec(107.0, 139.0);
		auto sustainParamPosition = Vec(107.0, 184.0);
		auto releaseParamPosition = Vec(107.0, 229.0);
		auto depthParamPosition = Vec(36.0, 106.0);
		auto envToDepthParamPosition = Vec(59.0, 139.7);
		auto feedbackParamPosition = Vec(36.0, 162.0);
		auto envToFeedbackParamPosition = Vec(59.0, 195.7);
		auto levelParamPosition = Vec(36.0, 218.0);
		auto envToLevelParamPosition = Vec(59.0, 251.7);

		auto depthInputPosition = Vec(15.0, 274.0);
		auto feedbackInputPosition = Vec(47.0, 274.0);
		auto levelInputPosition = Vec(79.0, 274.0);
		auto sustainInputPosition = Vec(111.0, 274.0);
		auto pitchInputPosition = Vec(15.0, 318.0);
		auto fmInputPosition = Vec(47.0, 318.0);
		auto gateInputPosition = Vec(79.0, 318.0);

		auto audioOutputPosition = Vec(111.0, 318.0);

		auto envToDepthLightPosition = Vec(30.0, 141.0);
		auto envToFeedbackLightPosition = Vec(30.0, 197.0);
		auto envToLevelLightPosition = Vec(30.0, 253.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(ratioParamPosition, module, FMOp::RATIO_PARAM));
		addParam(createParam<Knob16>(fineParamPosition, module, FMOp::FINE_PARAM));
		addParam(createParam<Knob26>(attackParamPosition, module, FMOp::ATTACK_PARAM));
		addParam(createParam<Knob26>(decayParamPosition, module, FMOp::DECAY_PARAM));
		addParam(createParam<Knob26>(sustainParamPosition, module, FMOp::SUSTAIN_PARAM));
		addParam(createParam<Knob26>(releaseParamPosition, module, FMOp::RELEASE_PARAM));
		addParam(createParam<Knob26>(depthParamPosition, module, FMOp::DEPTH_PARAM));
		addParam(createParam<Knob26>(feedbackParamPosition, module, FMOp::FEEDBACK_PARAM));
		addParam(createParam<Knob26>(levelParamPosition, module, FMOp::LEVEL_PARAM));
		addParam(createParam<StatefulButton9>(envToLevelParamPosition, module, FMOp::ENV_TO_LEVEL_PARAM));
		addParam(createParam<StatefulButton9>(envToFeedbackParamPosition, module, FMOp::ENV_TO_FEEDBACK_PARAM));
		addParam(createParam<StatefulButton9>(envToDepthParamPosition, module, FMOp::ENV_TO_DEPTH_PARAM));

		addInput(createInput<Port24>(sustainInputPosition, module, FMOp::SUSTAIN_INPUT));
		addInput(createInput<Port24>(depthInputPosition, module, FMOp::DEPTH_INPUT));
		addInput(createInput<Port24>(feedbackInputPosition, module, FMOp::FEEDBACK_INPUT));
		addInput(createInput<Port24>(levelInputPosition, module, FMOp::LEVEL_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, FMOp::PITCH_INPUT));
		addInput(createInput<Port24>(gateInputPosition, module, FMOp::GATE_INPUT));
		addInput(createInput<Port24>(fmInputPosition, module, FMOp::FM_INPUT));

		addOutput(createOutput<Port24>(audioOutputPosition, module, FMOp::AUDIO_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(envToLevelLightPosition, module, FMOp::ENV_TO_LEVEL_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(envToFeedbackLightPosition, module, FMOp::ENV_TO_FEEDBACK_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(envToDepthLightPosition, module, FMOp::ENV_TO_DEPTH_LIGHT));
	}

	struct LinearLevelMenuItem : MenuItem {
		FMOp* _module;

		LinearLevelMenuItem(FMOp* module, const char* label)
		: _module(module)
		{
			this->text = label;
		}

		void onAction(const event::Action& e) override {
			_module->_linearLevel = !_module->_linearLevel;
		}

		void step() override {
			rightText = _module->_linearLevel ? "✔" : "";
		}
	};

	void appendContextMenu(Menu* menu) override {
	  FMOp* fmop = dynamic_cast<FMOp*>(module);
		assert(fmop);
		menu->addChild(new MenuLabel());
		menu->addChild(new LinearLevelMenuItem(fmop, "Linear level response"));
	}
};

Model* modelFMOp = bogaudio::createModel<FMOp, FMOpWidget>("Bogaudio-FMOp", "FM-OP",  "FM oscillator", "Oscillator", "Synth voice");
