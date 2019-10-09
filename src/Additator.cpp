
#include "Additator.hpp"

void Additator::Engine::reset() {
	syncTrigger.reset();
	phase = PHASE_RESET;
}

void Additator::Engine::sampleRateChange() {
	float sampleRate = APP->engine->getSampleRate();
	oscillator.setSampleRate(sampleRate);
	maxFrequency = 0.475f * sampleRate;
	phase = PHASE_RESET;
	widthSL.setParams(sampleRate, slewLimitTime, maxWidth);
	oddSkewSL.setParams(sampleRate, slewLimitTime, 2.0f * maxSkew);
	evenSkewSL.setParams(sampleRate, slewLimitTime, 2.0f * maxSkew);
	amplitudeNormalizationSL.setParams(sampleRate, slewLimitTime, maxAmplitudeNormalization - minAmplitudeNormalization);
	decaySL.setParams(sampleRate, slewLimitTime, maxDecay - minDecay);
	balanceSL.setParams(sampleRate, slewLimitTime, 2.0f);
	filterSL.setParams(sampleRate, slewLimitTime, maxFilter - minFilter);
}

void Additator::reset() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->reset();
	}
}

void Additator::sampleRateChange() {
	for (int c = 0; c < _channels; ++c) {
		_engines[c]->sampleRateChange();
	}
}

bool Additator::active() {
	return outputs[AUDIO_OUTPUT].isConnected();
}

int Additator::channels() {
	return std::max(1, inputs[PITCH_INPUT].getChannels());
}

void Additator::addEngine(int c) {
	Engine& e = *(_engines[c] = new Engine());
	e.reset();
	e.sampleRateChange();

	e.widthSL._last = widthParam(c);
	e.oddSkewSL._last = oddSkewParam(c);
	e.evenSkewSL._last = evenSkewParam(c);
	e.amplitudeNormalizationSL._last = amplitudeNormalizationParam(c);
	e.decaySL._last = decayParam(c);
	e.balanceSL._last = balanceParam(c);
	e.filterSL._last = filterParam(c);

	modulateChannel(c);
	if (c > 0) {
		e.oscillator.syncTo(_engines[0]->oscillator);
	}
}

void Additator::removeEngine(int c) {
	delete _engines[c];
	_engines[c] = NULL;
}

float Additator::widthParam(int c) {
	return clamp(params[WIDTH_PARAM].getValue() + (maxWidth / 2.0f) * cvValue(c, inputs[WIDTH_INPUT]), 0.0f, maxWidth);
}

float Additator::oddSkewParam(int c) {
	return clamp(params[ODD_SKEW_PARAM].getValue() + cvValue(c, inputs[ODD_SKEW_INPUT]), -maxSkew, maxSkew);
}

float Additator::evenSkewParam(int c) {
	return clamp(params[EVEN_SKEW_PARAM].getValue() + cvValue(c, inputs[EVEN_SKEW_INPUT]), -maxSkew, maxSkew);
}

float Additator::amplitudeNormalizationParam(int c) {
	return clamp(params[GAIN_PARAM].getValue() + ((maxAmplitudeNormalization - minAmplitudeNormalization) / 2.0f) * cvValue(c, inputs[GAIN_INPUT]), minAmplitudeNormalization, maxAmplitudeNormalization);
}

float Additator::decayParam(int c) {
	return clamp(params[DECAY_PARAM].getValue() + ((maxDecay - minDecay) / 2.0f) * cvValue(c, inputs[DECAY_INPUT]), minDecay, maxDecay);
}

float Additator::balanceParam(int c) {
	return clamp(params[BALANCE_PARAM].getValue() + cvValue(c, inputs[BALANCE_INPUT]), -1.0f, 1.0f);
}

float Additator::filterParam(int c) {
	return clamp(params[FILTER_PARAM].getValue() + cvValue(c, inputs[FILTER_INPUT]), minFilter, maxFilter);
}

void Additator::modulateChannel(int c) {
	Engine& e = *_engines[c];

	float width = e.widthSL.next(widthParam(c));
	float oddSkew = e.oddSkewSL.next(oddSkewParam(c));
	float evenSkew = e.evenSkewSL.next(evenSkewParam(c));
	if (
		e.width != width ||
		e.oddSkew != oddSkew ||
		e.evenSkew != evenSkew
	) {
		e.width = width;
		e.oddSkew = oddSkew;
		e.evenSkew = evenSkew;

		e.oscillator.setPartialFrequencyRatio(1, 1.0f);
		e.activePartials = 1;
		for (int i = 2, n = e.oscillator.partialCount(); i <= n; ++i) {
			float ii = i;
			if (i % 2 == 0) {
				ii += e.evenSkew;
			}
			else {
				ii += e.oddSkew;
			}
			if (e.oscillator.setPartialFrequencyRatio(i, powf(ii, e.width))) {
				e.activePartials = i;
			}
		}
	}

	int partials = clamp((int)roundf(params[PARTIALS_PARAM].getValue() * cvValue(c, inputs[PARTIALS_INPUT], true)), 0, maxPartials);
	float amplitudeNormalization = e.amplitudeNormalizationSL.next(amplitudeNormalizationParam(c));
	float decay = e.decaySL.next(decayParam(c));
	float balance = e.balanceSL.next(balanceParam(c));
	float filter = e.filterSL.next(filterParam(c));
	if (
		e.partials != partials ||
		e.amplitudeNormalization != amplitudeNormalization ||
		e.decay != decay ||
		e.balance != balance ||
		e.filter != filter
	) {
		int envelopes = e.partials != partials ? std::max(e.partials, partials) : 0;
		e.partials = partials;
		e.amplitudeNormalization = amplitudeNormalization;
		e.decay = decay;
		e.balance = balance;
		e.filter = filter;

		float as[maxPartials + 1];
		float total = as[1] = 1.0f;
		filter = log10f(e.filter) + 1.0f;
		int np = std::min(e.partials, e.activePartials);
		for (int i = 2, n = e.oscillator.partialCount(); i <= n; ++i) {
			as[i] = 0.0f;
			if (i <= np) {
				as[i] = powf(i, -e.decay) * powf(e.filter, i);
				if (i % 2 == 0) {
					if (e.balance > 0.0f) {
						as[i] *= 1.0f - e.balance;
					}
				}
				else {
					if (e.balance < 0.0f) {
						as[i] *= 1.0f + e.balance;
					}
				}
				total += as[i];
			}
		}
		float norm = std::max(np / (float)e.oscillator.partialCount(), 0.1f);
		norm = 1.0f + (e.amplitudeNormalization - 1.0f) * norm;
		norm = std::max(total / norm, 0.7f);
		for (int i = 1, n = e.oscillator.partialCount(); i <= n; ++i) {
			as[i] /= norm;
			e.oscillator.setPartialAmplitude(i, as[i], i <= envelopes);
		}
	}

	float frequency = params[FREQUENCY_PARAM].getValue();
	frequency += params[FINE_PARAM].getValue() / 12.0f;
	if (inputs[PITCH_INPUT].isConnected()) {
		frequency += clamp(inputs[PITCH_INPUT].getVoltage(c), -5.0f, 5.0f);
	}
	frequency = clamp(cvToFrequency(frequency), 20.0f, e.maxFrequency);
	e.oscillator.setFrequency(frequency);

	Phase phase = params[PHASE_PARAM].getValue() > 1.5f ? PHASE_COSINE : PHASE_SINE;
	if (e.phase != phase) {
		e.phase = phase;
		e.oscillator.syncToPhase(e.phase == PHASE_SINE ? 0.0f : M_PI / 2.0f);
	}
}

void Additator::always(const ProcessArgs& args) {
	Phase phase = params[PHASE_PARAM].getValue() > 1.5f ? PHASE_COSINE : PHASE_SINE;
	lights[SINE_LIGHT].value = phase == PHASE_SINE;
	lights[COSINE_LIGHT].value = phase == PHASE_COSINE;
}

void Additator::processChannel(const ProcessArgs& args, int c) {
	Engine& e = *_engines[c];

	if (e.syncTrigger.next(inputs[SYNC_INPUT].getPolyVoltage(c))) {
		e.oscillator.syncToPhase(e.phase == PHASE_SINE ? 0.0f : M_PI / 2.0f);
	}
	outputs[AUDIO_OUTPUT].setChannels(_channels);
	outputs[AUDIO_OUTPUT].setVoltage(e.oscillator.next() * 5.0, c);
}

float Additator::cvValue(int c, Input& cv, bool dc) {
	if (!cv.isConnected()) {
		return dc ? 1.0f : 0.0f;
	}
	if (dc) {
		return clamp(cv.getPolyVoltage(c) / 10.0f, 0.0f, 1.0f);
	}
	return clamp(cv.getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
}

struct AdditatorWidget : ModuleWidget {
	static constexpr int hp = 15;

	AdditatorWidget(Additator* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Additator.svg")));
			addChild(panel);
		}

		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// generated by svg_widgets.rb
		auto frequencyParamPosition = Vec(40.0, 45.0);
		auto partialsParamPosition = Vec(165.0, 60.0);
		auto fineParamPosition = Vec(30.0, 160.0);
		auto widthParamPosition = Vec(79.0, 155.0);
		auto oddSkewParamPosition = Vec(132.0, 155.0);
		auto evenSkewParamPosition = Vec(184.0, 155.0);
		auto gainParamPosition = Vec(25.0, 218.0);
		auto decayParamPosition = Vec(79.0, 218.0);
		auto balanceParamPosition = Vec(132.0, 218.0);
		auto filterParamPosition = Vec(184.0, 218.0);
		auto phaseParamPosition = Vec(194.0, 299.0);

		auto syncInputPosition = Vec(16.0, 274.0);
		auto partialsInputPosition = Vec(50.0, 274.0);
		auto widthInputPosition = Vec(84.0, 274.0);
		auto oddSkewInputPosition = Vec(118.0, 274.0);
		auto evenSkewInputPosition = Vec(152.0, 274.0);
		auto pitchInputPosition = Vec(16.0, 318.0);
		auto gainInputPosition = Vec(50.0, 318.0);
		auto decayInputPosition = Vec(84.0, 318.0);
		auto balanceInputPosition = Vec(118.0, 318.0);
		auto filterInputPosition = Vec(152.0, 318.0);

		auto audioOutputPosition = Vec(186.0, 318.0);

		auto sineLightPosition = Vec(185.0, 272.0);
		auto cosineLightPosition = Vec(185.0, 287.0);
		// end generated by svg_widgets.rb

		addParam(createParam<Knob68>(frequencyParamPosition, module, Additator::FREQUENCY_PARAM));
		{
			auto w = createParam<Knob38>(partialsParamPosition, module, Additator::PARTIALS_PARAM);
			dynamic_cast<Knob*>(w)->snap = true;
			addParam(w);
		}
		addParam(createParam<Knob16>(fineParamPosition, module, Additator::FINE_PARAM));
		addParam(createParam<Knob26>(widthParamPosition, module, Additator::WIDTH_PARAM));
		addParam(createParam<Knob26>(oddSkewParamPosition, module, Additator::ODD_SKEW_PARAM));
		addParam(createParam<Knob26>(evenSkewParamPosition, module, Additator::EVEN_SKEW_PARAM));
		addParam(createParam<Knob26>(gainParamPosition, module, Additator::GAIN_PARAM));
		addParam(createParam<Knob26>(decayParamPosition, module, Additator::DECAY_PARAM));
		addParam(createParam<Knob26>(balanceParamPosition, module, Additator::BALANCE_PARAM));
		addParam(createParam<Knob26>(filterParamPosition, module, Additator::FILTER_PARAM));
		addParam(createParam<StatefulButton9>(phaseParamPosition, module, Additator::PHASE_PARAM));

		addInput(createInput<Port24>(partialsInputPosition, module, Additator::PARTIALS_INPUT));
		addInput(createInput<Port24>(widthInputPosition, module, Additator::WIDTH_INPUT));
		addInput(createInput<Port24>(oddSkewInputPosition, module, Additator::ODD_SKEW_INPUT));
		addInput(createInput<Port24>(evenSkewInputPosition, module, Additator::EVEN_SKEW_INPUT));
		addInput(createInput<Port24>(gainInputPosition, module, Additator::GAIN_INPUT));
		addInput(createInput<Port24>(decayInputPosition, module, Additator::DECAY_INPUT));
		addInput(createInput<Port24>(balanceInputPosition, module, Additator::BALANCE_INPUT));
		addInput(createInput<Port24>(filterInputPosition, module, Additator::FILTER_INPUT));
		addInput(createInput<Port24>(pitchInputPosition, module, Additator::PITCH_INPUT));
		addInput(createInput<Port24>(syncInputPosition, module, Additator::SYNC_INPUT));

		addOutput(createOutput<Port24>(audioOutputPosition, module, Additator::AUDIO_OUTPUT));

		addChild(createLight<SmallLight<GreenLight>>(sineLightPosition, module, Additator::SINE_LIGHT));
		addChild(createLight<SmallLight<GreenLight>>(cosineLightPosition, module, Additator::COSINE_LIGHT));
	}
};

Model* modelAdditator = bogaudio::createModel<Additator, AdditatorWidget>("Bogaudio-Additator", "ADDITATOR", "additive oscillator", "Oscillator", "Polyphonic");
