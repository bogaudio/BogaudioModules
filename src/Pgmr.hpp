#pragma once

#include "Pgmr_shared.hpp"

namespace bogaudio {

struct Pgmr : ExpandableModule<PgmrExpanderMessage, OutputRangeAddressableSequenceModule>, PgmrBase {
	enum ParamsIds {
		DIRECTION_PARAM,
		SELECT_ON_CLOCK_PARAM,
		CVA1_PARAM,
		CVB1_PARAM,
		CVC1_PARAM,
		CVD1_PARAM,
		SELECT1_PARAM,
		CVA2_PARAM,
		CVB2_PARAM,
		CVC2_PARAM,
		CVD2_PARAM,
		SELECT2_PARAM,
		CVA3_PARAM,
		CVB3_PARAM,
		CVC3_PARAM,
		CVD3_PARAM,
		SELECT3_PARAM,
		CVA4_PARAM,
		CVB4_PARAM,
		CVC4_PARAM,
		CVD4_PARAM,
		SELECT4_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CLOCK_INPUT,
		SELECT_INPUT,
		SELECT1_INPUT,
		SELECT2_INPUT,
		SELECT3_INPUT,
		SELECT4_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		A_OUTPUT,
		B_OUTPUT,
		C_OUTPUT,
		D_OUTPUT,
		SELECT_ALL_OUTPUT,
		SELECT1_OUTPUT,
		SELECT2_OUTPUT,
		SELECT3_OUTPUT,
		SELECT4_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		SELECT1_LIGHT,
		SELECT2_LIGHT,
		SELECT3_LIGHT,
		SELECT4_LIGHT,
		NUM_LIGHTS
	};

	float _sampleTime = 0.001f;
	bool _selectTriggers = false;
	SpinLock _stepsLock;
	std::vector<PgmrStep*> _steps;
	int _lastSteps[maxChannels] {};
	rack::dsp::PulseGenerator _allPulseGens[maxChannels];
	int _id = -1;

	Pgmr() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(DIRECTION_PARAM, 0.0f, 1.0f, 1.0f, "Forward");
		configParam(SELECT_ON_CLOCK_PARAM, 0.0f, 1.0f, 0.0f, "Select on clock");
		configParam<OutputRangeParamQuantity>(CVA1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1A", " V");
		configParam<OutputRangeParamQuantity>(CVB1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1B", " V");
		configParam<OutputRangeParamQuantity>(CVC1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1C", " V");
		configParam<OutputRangeParamQuantity>(CVD1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1D", " V");
		configParam(SELECT1_PARAM, 0.0f, 1.0f, 0.0f, "Select 1");
		configParam<OutputRangeParamQuantity>(CVA2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2A", " V");
		configParam<OutputRangeParamQuantity>(CVB2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2B", " V");
		configParam<OutputRangeParamQuantity>(CVC2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2C", " V");
		configParam<OutputRangeParamQuantity>(CVD2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2D", " V");
		configParam(SELECT2_PARAM, 0.0f, 1.0f, 0.0f, "Select 2");
		configParam<OutputRangeParamQuantity>(CVA3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3A", " V");
		configParam<OutputRangeParamQuantity>(CVB3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3B", " V");
		configParam<OutputRangeParamQuantity>(CVC3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3C", " V");
		configParam<OutputRangeParamQuantity>(CVD3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3D", " V");
		configParam(SELECT3_PARAM, 0.0f, 1.0f, 0.0f, "Select 3");
		configParam<OutputRangeParamQuantity>(CVA4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4A", " V");
		configParam<OutputRangeParamQuantity>(CVB4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4B", " V");
		configParam<OutputRangeParamQuantity>(CVC4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4C", " V");
		configParam<OutputRangeParamQuantity>(CVD4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4D", " V");
		configParam(SELECT4_PARAM, 0.0f, 1.0f, 0.0f, "Select 4");
		setInputIDs(CLOCK_INPUT, SELECT_INPUT);

		_localSteps[0] = new PgmrStep(params[CVA1_PARAM], params[CVB1_PARAM], params[CVC1_PARAM], params[CVD1_PARAM], lights[SELECT1_LIGHT], params[SELECT1_PARAM], inputs[SELECT1_INPUT], outputs[SELECT1_OUTPUT]);
		_localSteps[1] = new PgmrStep(params[CVA2_PARAM], params[CVB2_PARAM], params[CVC2_PARAM], params[CVD2_PARAM], lights[SELECT2_LIGHT], params[SELECT2_PARAM], inputs[SELECT2_INPUT], outputs[SELECT2_OUTPUT]);
		_localSteps[2] = new PgmrStep(params[CVA3_PARAM], params[CVB3_PARAM], params[CVC3_PARAM], params[CVD3_PARAM], lights[SELECT3_LIGHT], params[SELECT3_PARAM], inputs[SELECT3_INPUT], outputs[SELECT3_OUTPUT]);
		_localSteps[3] = new PgmrStep(params[CVA4_PARAM], params[CVB4_PARAM], params[CVC4_PARAM], params[CVD4_PARAM], lights[SELECT4_LIGHT], params[SELECT4_PARAM], inputs[SELECT4_INPUT], outputs[SELECT4_OUTPUT]);

		setExpanderModelPredicate([](Model* m) { return m == modelPgmrX; });
		_id = PgmrRegistry::registry().registerBase(*this);
	}
	virtual ~Pgmr() {
		PgmrRegistry::registry().deregisterBase(_id);
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* toJson(json_t* root) override;
	void fromJson(json_t* root) override;
	void modulate() override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void setSteps(std::vector<PgmrStep*>& steps);
};

} // namespace bogaudio
