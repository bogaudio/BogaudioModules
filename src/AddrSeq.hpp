#pragma once

#include "AddrSeq_shared.hpp"

namespace bogaudio {

typedef ChainableExpandableModule<AddrSeqExpanderMessage, AddrSeqStep, 8, OutputRangeAddressableSequenceModule> AddrSeqBase;

struct AddrSeq : AddrSeqBase {
	enum ParamsIds {
		STEPS_PARAM,
		DIRECTION_PARAM,
		SELECT_PARAM,
		OUT1_PARAM,
		OUT2_PARAM,
		OUT3_PARAM,
		OUT4_PARAM,
		OUT5_PARAM,
		OUT6_PARAM,
		OUT7_PARAM,
		OUT8_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CLOCK_INPUT,
		RESET_INPUT,
		SELECT_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		OUT1_LIGHT,
		OUT2_LIGHT,
		OUT3_LIGHT,
		OUT4_LIGHT,
		OUT5_LIGHT,
		OUT6_LIGHT,
		OUT7_LIGHT,
		OUT8_LIGHT,
		NUM_LIGHTS
	};

	struct StepsParamQuantity : ParamQuantity {
		float getDisplayValue() override {
			float v = getValue();
			if (!module) {
				return v;
			}

			auto m = dynamic_cast<AddrSeq*>(module);
			std::lock_guard<SpinLock> lock(m->_elementsLock);
			int n = m->_elements.size();
			v = clamp(v, 1.0f, 8.0f);
			v -= 1.0f;
			v /= 7.0f;
			v *= n - 1;
			v += 1.0f;
			return (int)v;
		}

		void setDisplayValue(float dv) override {
			if (!module) {
				return;
			}

			auto m = dynamic_cast<AddrSeq*>(module);
			std::lock_guard<SpinLock> lock(m->_elementsLock);
			int n = m->_elements.size();
			dv = clamp(dv, 1.0f, (float)n);
			dv -= 1.0f;
			dv /= (float)(n - 1);
			dv *= 7.0f;
			dv += 1.0f;
			setValue(dv);
		}
	};

	struct SelectParamQuantity : ParamQuantity {
		float getDisplayValue() override {
			float v = getValue();
			if (!module) {
				return v;
			}

			auto m = dynamic_cast<AddrSeq*>(module);
			std::lock_guard<SpinLock> lock(m->_elementsLock);
			int n = m->_elements.size();
			v = clamp(v, 0.0f, 7.0f);
			v /= 7.0f;
			v *= n - 1;
			v += 1.0f;
			return (int)v;
		}

		void setDisplayValue(float dv) override {
			if (!module) {
				return;
			}

			auto m = dynamic_cast<AddrSeq*>(module);
			std::lock_guard<SpinLock> lock(m->_elementsLock);
			int n = m->_elements.size();
			dv = clamp(dv, 1.0f, (float)n);
			dv -= 1.0f;
			dv /= (float)(n - 1);
			dv *= 7.0f;
			setValue(dv);
		}
	};

	AddrSeq() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<StepsParamQuantity>(STEPS_PARAM, 1.0f, 8.0f, 8.0f, "Steps");
		configSwitch(DIRECTION_PARAM, 0.0f, 1.0f, 1.0f, "Direction", {"Reverse", "Forward"});
		configParam<SelectParamQuantity>(SELECT_PARAM, 0.0f, 7.0f, 0.0f, "Select step");
		configParam<OutputRangeParamQuantity>(OUT1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1", " V");
		configParam<OutputRangeParamQuantity>(OUT2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2", " V");
		configParam<OutputRangeParamQuantity>(OUT3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3", " V");
		configParam<OutputRangeParamQuantity>(OUT4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4", " V");
		configParam<OutputRangeParamQuantity>(OUT5_PARAM, -1.0f, 1.0f, 0.0f, "Step 5", " V");
		configParam<OutputRangeParamQuantity>(OUT6_PARAM, -1.0f, 1.0f, 0.0f, "Step 6", " V");
		configParam<OutputRangeParamQuantity>(OUT7_PARAM, -1.0f, 1.0f, 0.0f, "Step 7", " V");
		configParam<OutputRangeParamQuantity>(OUT8_PARAM, -1.0f, 1.0f, 0.0f, "Step 8", " V");

		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configInput(SELECT_INPUT, "Select CV");

		configOutput(OUT_OUTPUT, "OUT");

		setInputIDs(CLOCK_INPUT, SELECT_INPUT);
		setLocalElements({
			new AddrSeqStep(params[OUT1_PARAM], lights[OUT1_LIGHT]),
			new AddrSeqStep(params[OUT2_PARAM], lights[OUT2_LIGHT]),
			new AddrSeqStep(params[OUT3_PARAM], lights[OUT3_LIGHT]),
			new AddrSeqStep(params[OUT4_PARAM], lights[OUT4_LIGHT]),
			new AddrSeqStep(params[OUT5_PARAM], lights[OUT5_LIGHT]),
			new AddrSeqStep(params[OUT6_PARAM], lights[OUT6_LIGHT]),
			new AddrSeqStep(params[OUT7_PARAM], lights[OUT7_LIGHT]),
			new AddrSeqStep(params[OUT8_PARAM], lights[OUT8_LIGHT])
		});
		registerBase();
		setExpanderModelPredicate([](Model* m) { return m == modelAddrSeqX; });
	}

	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
