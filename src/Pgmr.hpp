#pragma once

#include <mutex>
#include <unordered_map>

#include "bogaudio.hpp"
#include "addressable_sequence.hpp"
#include "expanders.hpp"

extern Model* modelPgmr;
extern Model* modelPgmrX;

namespace bogaudio {

struct PgmrExpanderMessage : ExpanderMessage {
	int baseID = -1;
	int position = -1;
	float rangeOffset = 0.0f;
	float rangeScale = 10.0f;
};

struct PgmrStep {
	Param& aParam;
	Param& bParam;
	Param& cParam;
	Param& dParam;
	Light& selectedLight;
	Param& selectParam;
	Input& selectInput;
	Output& selectedOutput;

	Trigger triggers[BGModule::maxChannels];
	rack::dsp::PulseGenerator pulseGens[BGModule::maxChannels];
	float lightSum = 0.0f;

	PgmrStep(
		Param& aParam,
		Param& bParam,
		Param& cParam,
		Param& dParam,
		Light& selectedLight,
		Param& selectParam,
		Input& selectInput,
		Output& selectedOutput
	)
	: aParam(aParam)
	, bParam(bParam)
	, cParam(cParam)
	, dParam(dParam)
	, selectedLight(selectedLight)
	, selectParam(selectParam)
	, selectInput(selectInput)
	, selectedOutput(selectedOutput)
	{
	}

	void reset();
};

struct PgmrBase {
	PgmrStep* _localSteps[4] {};

	virtual ~PgmrBase() {
		for (int i = 0; i < 4; ++i) {
			if (_localSteps[i]) {
				delete _localSteps[i];
			}
		}
	}
};

struct Pgmr;
struct PgmrX;

struct PgmrRegistry {
private:
	struct Base {
		Pgmr& module;
		std::vector<PgmrStep*> steps;

		Base(Pgmr& b);
	};

	std::mutex _lock;
	int _nextID = 1;
	std::unordered_map<int, Base> _bases;

public:
	int registerBase(Pgmr& b);
	void deregisterBase(int id);
	void registerExpander(int baseID, int position, PgmrX& x);
	void deregisterExpander(int baseID, int position);

	static PgmrRegistry& registry();
};

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
		configParam<OutputParamQuantity>(CVA1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1A", " V");
		configParam<OutputParamQuantity>(CVB1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1B", " V");
		configParam<OutputParamQuantity>(CVC1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1C", " V");
		configParam<OutputParamQuantity>(CVD1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1D", " V");
		configParam(SELECT1_PARAM, 0.0f, 1.0f, 0.0f, "Select 1");
		configParam<OutputParamQuantity>(CVA2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2A", " V");
		configParam<OutputParamQuantity>(CVB2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2B", " V");
		configParam<OutputParamQuantity>(CVC2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2C", " V");
		configParam<OutputParamQuantity>(CVD2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2D", " V");
		configParam(SELECT2_PARAM, 0.0f, 1.0f, 0.0f, "Select 2");
		configParam<OutputParamQuantity>(CVA3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3A", " V");
		configParam<OutputParamQuantity>(CVB3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3B", " V");
		configParam<OutputParamQuantity>(CVC3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3C", " V");
		configParam<OutputParamQuantity>(CVD3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3D", " V");
		configParam(SELECT3_PARAM, 0.0f, 1.0f, 0.0f, "Select 3");
		configParam<OutputParamQuantity>(CVA4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4A", " V");
		configParam<OutputParamQuantity>(CVB4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4B", " V");
		configParam<OutputParamQuantity>(CVC4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4C", " V");
		configParam<OutputParamQuantity>(CVD4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4D", " V");
		configParam(SELECT4_PARAM, 0.0f, 1.0f, 0.0f, "Select 4");
		setInputIDs(CLOCK_INPUT, SELECT_INPUT);

		_localSteps[0] = new PgmrStep(params[CVA1_PARAM], params[CVB1_PARAM], params[CVC1_PARAM], params[CVD1_PARAM], lights[SELECT1_LIGHT], params[SELECT1_PARAM], inputs[SELECT1_INPUT], outputs[SELECT1_OUTPUT]);
		_localSteps[1] = new PgmrStep(params[CVA2_PARAM], params[CVB2_PARAM], params[CVC2_PARAM], params[CVD2_PARAM], lights[SELECT2_LIGHT], params[SELECT2_PARAM], inputs[SELECT2_INPUT], outputs[SELECT2_OUTPUT]);
		_localSteps[2] = new PgmrStep(params[CVA3_PARAM], params[CVB3_PARAM], params[CVC3_PARAM], params[CVD3_PARAM], lights[SELECT3_LIGHT], params[SELECT3_PARAM], inputs[SELECT3_INPUT], outputs[SELECT3_OUTPUT]);
		_localSteps[3] = new PgmrStep(params[CVA4_PARAM], params[CVB4_PARAM], params[CVC4_PARAM], params[CVD4_PARAM], lights[SELECT4_LIGHT], params[SELECT4_PARAM], inputs[SELECT4_INPUT], outputs[SELECT4_OUTPUT]);

		setExpanderModel(modelPgmrX);
		_id = PgmrRegistry::registry().registerBase(*this);
	}
	virtual ~Pgmr() {
		PgmrRegistry::registry().deregisterBase(_id);
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void modulate() override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void setSteps(std::vector<PgmrStep*>& steps);
};

struct PgmrX : ExpanderModule<PgmrExpanderMessage, ExpandableModule<PgmrExpanderMessage, BGModule>>, PgmrBase {
	enum ParamsIds {
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
		SELECT1_INPUT,
		SELECT2_INPUT,
		SELECT3_INPUT,
		SELECT4_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
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

	struct OutputParamQuantity : ParamQuantity {
		float getDisplayValue() override;
		void setDisplayValue(float v) override;
	};

	bool _registered = false;
	int _baseID = -1;
	int _position = -1;
	float _rangeOffset = 0.0f;
	float _rangeScale = 10.0f;

	PgmrX() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<OutputParamQuantity>(CVA1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1A", " V");
		configParam<OutputParamQuantity>(CVB1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1B", " V");
		configParam<OutputParamQuantity>(CVC1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1C", " V");
		configParam<OutputParamQuantity>(CVD1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1D", " V");
		configParam(SELECT1_PARAM, 0.0f, 1.0f, 0.0f, "Select 1");
		configParam<OutputParamQuantity>(CVA2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2A", " V");
		configParam<OutputParamQuantity>(CVB2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2B", " V");
		configParam<OutputParamQuantity>(CVC2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2C", " V");
		configParam<OutputParamQuantity>(CVD2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2D", " V");
		configParam(SELECT2_PARAM, 0.0f, 1.0f, 0.0f, "Select 2");
		configParam<OutputParamQuantity>(CVA3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3A", " V");
		configParam<OutputParamQuantity>(CVB3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3B", " V");
		configParam<OutputParamQuantity>(CVC3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3C", " V");
		configParam<OutputParamQuantity>(CVD3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3D", " V");
		configParam(SELECT3_PARAM, 0.0f, 1.0f, 0.0f, "Select 3");
		configParam<OutputParamQuantity>(CVA4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4A", " V");
		configParam<OutputParamQuantity>(CVB4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4B", " V");
		configParam<OutputParamQuantity>(CVC4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4C", " V");
		configParam<OutputParamQuantity>(CVD4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4D", " V");
		configParam<OutputParamQuantity>(SELECT4_PARAM, 0.0f, 1.0f, 0.0f, "Select 4");

		_localSteps[0] = new PgmrStep(params[CVA1_PARAM], params[CVB1_PARAM], params[CVC1_PARAM], params[CVD1_PARAM], lights[SELECT1_LIGHT], params[SELECT1_PARAM], inputs[SELECT1_INPUT], outputs[SELECT1_OUTPUT]);
		_localSteps[1] = new PgmrStep(params[CVA2_PARAM], params[CVB2_PARAM], params[CVC2_PARAM], params[CVD2_PARAM], lights[SELECT2_LIGHT], params[SELECT2_PARAM], inputs[SELECT2_INPUT], outputs[SELECT2_OUTPUT]);
		_localSteps[2] = new PgmrStep(params[CVA3_PARAM], params[CVB3_PARAM], params[CVC3_PARAM], params[CVD3_PARAM], lights[SELECT3_LIGHT], params[SELECT3_PARAM], inputs[SELECT3_INPUT], outputs[SELECT3_OUTPUT]);
		_localSteps[3] = new PgmrStep(params[CVA4_PARAM], params[CVB4_PARAM], params[CVC4_PARAM], params[CVD4_PARAM], lights[SELECT4_LIGHT], params[SELECT4_PARAM], inputs[SELECT4_INPUT], outputs[SELECT4_OUTPUT]);

		setBaseModel(modelPgmr);
		setChainableModel(modelPgmrX);
		setExpanderModel(modelPgmrX);
	}
	virtual ~PgmrX() {
		PgmrRegistry::registry().deregisterExpander(_baseID, _position);
	}

	void processAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
