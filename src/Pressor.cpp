
#include "Pressor.hpp"

void Pressor::onReset() {
	_modulationStep = modulationSteps;
}

void Pressor::onSampleRateChange() {
	_detectorRMS.setSampleRate(APP->engine->getSampleRate());
	_modulationStep = modulationSteps;
}

void Pressor::process(const ProcessArgs& args) {
	if (!(
		outputs[LEFT_OUTPUT].isConnected() ||
		outputs[RIGHT_OUTPUT].isConnected() ||
		outputs[ENVELOPE_OUTPUT].isConnected() ||
		outputs[LEFT_INPUT].isConnected() ||
		outputs[RIGHT_INPUT].isConnected() ||
		outputs[SIDECHAIN_INPUT].isConnected()
	)) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		_thresholdDb = params[THRESHOLD_PARAM].getValue();
		if (inputs[THRESHOLD_INPUT].isConnected()) {
			_thresholdDb *= clamp(inputs[THRESHOLD_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		_thresholdDb *= 30.0f;
		_thresholdDb -= 24.0f;

		float ratio = params[RATIO_PARAM].getValue();
		if (inputs[RATIO_INPUT].isConnected()) {
			ratio *= clamp(inputs[RATIO_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		if (_ratioKnob != ratio) {
			_ratioKnob = ratio;
			_ratio = powf(_ratioKnob, 1.5f);
			_ratio = 1.0f - _ratio;
			_ratio *= M_PI;
			_ratio *= 0.25f;
			_ratio = tanf(_ratio);
			_ratio = 1.0f / _ratio;
		}

		float sampleRate = APP->engine->getSampleRate();
		float attack = params[ATTACK_PARAM].getValue();
		if (inputs[ATTACK_INPUT].isConnected()) {
			attack *= clamp(inputs[ATTACK_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		attack *= attack;
		_attackSL.setParams(sampleRate, attack * 500.0f);

		float release = params[RELEASE_PARAM].getValue();
		if (inputs[RELEASE_INPUT].isConnected()) {
			release *= clamp(inputs[RELEASE_INPUT].getVoltage() / 10.0f, 0.0f, 1.0f);
		}
		release *= release;
		_releaseSL.setParams(sampleRate, release * 2000.0f);

		float inGain = params[INPUT_GAIN_PARAM].getValue();
		if (inputs[INPUT_GAIN_INPUT].isConnected()) {
			inGain = clamp(inGain + inputs[INPUT_GAIN_INPUT].getVoltage() / 5.0f, -1.0f, 1.0f);
		}
		inGain *= 12.0f;
		if (_inGain != inGain) {
			_inGain = inGain;
			_inLevel = decibelsToAmplitude(_inGain);
		}

		float outGain = params[OUTPUT_GAIN_PARAM].getValue();
		if (inputs[OUTPUT_GAIN_INPUT].isConnected()) {
			outGain = clamp(outGain + inputs[OUTPUT_GAIN_INPUT].getVoltage() / 5.0f, 0.0f, 1.0f);
		}
		outGain *= 24.0f;
		if (_outGain != outGain) {
			_outGain = outGain;
			_outLevel = decibelsToAmplitude(_outGain);
		}

		_detectorMix.setParams(params[DETECTOR_MIX_PARAM].getValue(), 0.0f, true);

		_compressorMode = params[MODE_PARAM].getValue() > 0.5f;
		_rmsDetector = params[DECTECTOR_MODE_PARAM].getValue() > 0.5f;
		_softKnee = params[KNEE_PARAM].getValue() > 0.5f;
	}

	float leftInput = inputs[LEFT_INPUT].getVoltage() * _inLevel;
	float rightInput = inputs[RIGHT_INPUT].getVoltage() * _inLevel;
	float env = leftInput + rightInput;
	if (inputs[SIDECHAIN_INPUT].isConnected()) {
		env = _detectorMix.next(env, inputs[SIDECHAIN_INPUT].getVoltage());
	}
	if (_rmsDetector) {
		env = _detectorRMS.next(env);
	}
	else {
		env = fabsf(env);
	}
	if (env > _lastEnv) {
		env = _attackSL.next(env, _lastEnv);
	}
	else {
		env = _releaseSL.next(env, _lastEnv);
	}
	_lastEnv = env;

	float detectorDb = amplitudeToDecibels(env / 5.0f);
	if (_compressorMode) {
		_compressionDb = _compressor.compressionDb(detectorDb, _thresholdDb, _ratio, _softKnee);
	}
	else {
		_compressionDb = _noiseGate.compressionDb(detectorDb, _thresholdDb, _ratio, _softKnee);
	}
	_amplifier.setLevel(-_compressionDb);
	outputs[ENVELOPE_OUTPUT].setVoltage(env);
	if (outputs[LEFT_OUTPUT].isConnected()) {
		outputs[LEFT_OUTPUT].setVoltage(_saturator.next(_amplifier.next(leftInput) * _outLevel));
	}
	if (outputs[RIGHT_OUTPUT].isConnected()) {
		outputs[RIGHT_OUTPUT].setVoltage(_saturator.next(_amplifier.next(rightInput) * _outLevel));
	}
}

struct CompressionDisplay : OpaqueWidget {
	struct Level {
		float db;
		NVGcolor color;
		Level(float db, const NVGcolor& color) : db(db), color(color) {}
	};

	const NVGcolor bgColor = nvgRGBA(0xaa, 0xaa, 0xaa, 0xff);
	Pressor* _module;
	std::vector<Level> _levels;

	CompressionDisplay(Pressor* module) : _module(module) {
		auto color = nvgRGBA(0xff, 0xaa, 0x00, 0xff);
		_levels.push_back(Level(30.0f, color));
		for (int i = 1; i <= 15; ++i) {
			float db = 30.0f - i*2.0f;
			_levels.push_back(Level(db, color)); // decibelsToColor(db - 15.0f)));
		}
	}

	void draw(const DrawArgs& args) override {
		float compressionDb = 0.0f;
		if (_module) {
			compressionDb = _module->_compressionDb;
		}

		nvgSave(args.vg);
		for (int i = 0; i < 80; i += 5) {
			const Level& l = _levels.at(i / 5);

			nvgBeginPath(args.vg);
			nvgRect(args.vg, 3, i + 1, 5, 4);
			nvgFillColor(args.vg, bgColor);
			nvgFill(args.vg);
			if (compressionDb > l.db) {
				nvgFillColor(args.vg, l.color);
				nvgFill(args.vg);
			}
		}
		nvgRestore(args.vg);
	}
};

struct PressorWidget : ModuleWidget {
	static constexpr int hp = 15;

	PressorWidget(Pressor* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Pressor.svg")));
			addChild(panel);
		}

		{
			auto display = new CompressionDisplay(module);
			display->box.pos = Vec(208.0, 30.0 - 0.5);
			display->box.size = Vec(18, 180);
			addChild(display);
		}

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto thresholdParamPosition = Vec(36.0, 53.0);
		auto ratioParamPosition = Vec(125.0, 53.0);
		auto attackParamPosition = Vec(42.0, 141.0);
		auto releaseParamPosition = Vec(131.0, 141.0);
		auto inputGainParamPosition = Vec(28.0, 213.0);
		auto outputGainParamPosition = Vec(89.0, 213.0);
		auto detectorMixParamPosition = Vec(150.0, 213.0);
		auto modeParamPosition = Vec(198.5, 129.5);
		auto dectectorModeParamPosition = Vec(198.5, 178.5);
		auto kneeParamPosition = Vec(198.5, 227.5);

		auto leftInputPosition = Vec(16.0, 274.0);
		auto sidechainInputPosition = Vec(50.0, 274.0);
		auto thresholdInputPosition = Vec(84.0, 274.0);
		auto ratioInputPosition = Vec(118.0, 274.0);
		auto rightInputPosition = Vec(16.0, 318.0);
		auto attackInputPosition = Vec(50.0, 318.0);
		auto releaseInputPosition = Vec(84.0, 318.0);
		auto inputGainInputPosition = Vec(118.0, 318.0);
		auto outputGainInputPosition = Vec(152.0, 318.0);

		auto envelopeOutputPosition = Vec(152.0, 274.0);
		auto leftOutputPosition = Vec(186.0, 274.0);
		auto rightOutputPosition = Vec(186.0, 318.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob38>(thresholdParamPosition, module, Pressor::THRESHOLD_PARAM));
		addParam(createParam<Knob38>(ratioParamPosition, module, Pressor::RATIO_PARAM));
		addParam(createParam<Knob26>(attackParamPosition, module, Pressor::ATTACK_PARAM));
		addParam(createParam<Knob26>(releaseParamPosition, module, Pressor::RELEASE_PARAM));
		addParam(createParam<Knob26>(outputGainParamPosition, module, Pressor::OUTPUT_GAIN_PARAM));
		addParam(createParam<Knob26>(inputGainParamPosition, module, Pressor::INPUT_GAIN_PARAM));
		addParam(createParam<Knob26>(detectorMixParamPosition, module, Pressor::DETECTOR_MIX_PARAM));
		addParam(createParam<SliderSwitch2State14>(modeParamPosition, module, Pressor::MODE_PARAM));
		addParam(createParam<SliderSwitch2State14>(dectectorModeParamPosition, module, Pressor::DECTECTOR_MODE_PARAM));
		addParam(createParam<SliderSwitch2State14>(kneeParamPosition, module, Pressor::KNEE_PARAM));

		addInput(createInput<Port24>(leftInputPosition, module, Pressor::LEFT_INPUT));
		addInput(createInput<Port24>(sidechainInputPosition, module, Pressor::SIDECHAIN_INPUT));
		addInput(createInput<Port24>(thresholdInputPosition, module, Pressor::THRESHOLD_INPUT));
		addInput(createInput<Port24>(ratioInputPosition, module, Pressor::RATIO_INPUT));
		addInput(createInput<Port24>(rightInputPosition, module, Pressor::RIGHT_INPUT));
		addInput(createInput<Port24>(attackInputPosition, module, Pressor::ATTACK_INPUT));
		addInput(createInput<Port24>(releaseInputPosition, module, Pressor::RELEASE_INPUT));
		addInput(createInput<Port24>(inputGainInputPosition, module, Pressor::INPUT_GAIN_INPUT));
		addInput(createInput<Port24>(outputGainInputPosition, module, Pressor::OUTPUT_GAIN_INPUT));

		addOutput(createOutput<Port24>(envelopeOutputPosition, module, Pressor::ENVELOPE_OUTPUT));
		addOutput(createOutput<Port24>(leftOutputPosition, module, Pressor::LEFT_OUTPUT));
		addOutput(createOutput<Port24>(rightOutputPosition, module, Pressor::RIGHT_OUTPUT));
	}
};

Model* modelPressor = bogaudio::createModel<Pressor, PressorWidget>("Bogaudio-Pressor", "Pressor", "stereo compressor", "Compressor", "Dynamics");
