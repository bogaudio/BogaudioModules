
#include "matrix_base.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio;
using namespace bogaudio::dsp;

#define CLIPPING_MODE "clipping_mode"
#define INPUT_GAIN_DB "input_gain_db"

json_t* MatrixBaseModule::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, CLIPPING_MODE, json_integer(_clippingMode));
	json_object_set_new(root, INPUT_GAIN_DB, json_real(_inputGainDb));
	return root;
}

void MatrixBaseModule::dataFromJson(json_t* root) {
	json_t* c = json_object_get(root, CLIPPING_MODE);
	if (c) {
		_clippingMode = (Clipping)json_integer_value(c);
		if (_clippingMode != HARD_CLIPPING) {
			_clippingMode = SOFT_CLIPPING;
		}
	}

	json_t* g = json_object_get(root, INPUT_GAIN_DB);
	if (g) {
		_inputGainDb = clamp(json_real_value(g), -60.0f, 6.0f);
	}
}

void MatrixBaseModule::modulate() {
	_inputGainLevel = decibelsToAmplitude(_inputGainDb);
}


int MatrixModule::channels() {
	return inputs[_firstInputID].getChannels();
}

void MatrixModule::processChannel(const ProcessArgs& args, int c) {
	for (int i = 0; i < _n; ++i) {
		int paramOffset = _firstParamID + i * _n;
		float out = 0.0f;
		for (int j = 0; j < _n; ++j) {
			out += inputs[_firstInputID + j].getPolyVoltage(c) * _inputGainLevel * params[paramOffset + j].getValue();
		}
		if (_clippingMode != HARD_CLIPPING) {
			out = _saturators[c * _n + i].next(out);
		}
		outputs[_firstOutputID + i].setChannels(_channels);
		outputs[_firstOutputID + i].setVoltage(out, c);
	}
}


#define INDICATOR_KNOBS "indicator_knobs"

json_t* KnobMatrixModule::dataToJson() {
	json_t* root = MatrixBaseModule::dataToJson();
	json_object_set_new(root, INDICATOR_KNOBS, json_boolean(_indicatorKnobs));
	return root;
}

void KnobMatrixModule::dataFromJson(json_t* root) {
	MatrixBaseModule::dataFromJson(root);
	json_t* k = json_object_get(root, INDICATOR_KNOBS);
	if (k) {
		_indicatorKnobs = json_is_true(k);
	}
}


#define INVERTING "inverting"
#define INVERTING_CLICK "click"
#define INVERTING_PARAM "param"
#define INVERTING_DISABLED "disabled"

json_t* SwitchMatrixModule::dataToJson() {
	json_t* root = MatrixBaseModule::dataToJson();
	switch (_inverting) {
		case CLICK_INVERTING: {
			json_object_set_new(root, INVERTING, json_string(INVERTING_CLICK));
			break;
		}
		case PARAM_INVERTING: {
			json_object_set_new(root, INVERTING, json_string(INVERTING_PARAM));
			break;
		}
		case NO_INVERTING: {
			json_object_set_new(root, INVERTING, json_string(INVERTING_DISABLED));
			break;
		}
	}
	return root;
}

void SwitchMatrixModule::dataFromJson(json_t* root) {
	MatrixBaseModule::dataFromJson(root);
	json_t* i = json_object_get(root, INVERTING);
	if (i) {
		const char* s = json_string_value(i);
		if (s) {
			if (0 == strcmp(INVERTING_CLICK, s)) {
				setInverting(CLICK_INVERTING);
			}
			else if (0 == strcmp(INVERTING_PARAM, s)) {
				setInverting(PARAM_INVERTING);
			}
			else if (0 == strcmp(INVERTING_DISABLED, s)) {
				setInverting(NO_INVERTING);
			}
		}
	}
}

void SwitchMatrixModule::setInverting(Inverting inverting) {
	_inverting = inverting;

	float minValue = -1.0f;
	switch (_inverting) {
		case CLICK_INVERTING:
		case PARAM_INVERTING: {
			minValue = -1.0f;
			break;
		}
		default: {
			minValue = 0.0f;
		}
	}
	for (ParamQuantity* pq : _switchParamQuantities) {
		pq->minValue = minValue;
		if (pq->getValue() < minValue) {
			pq->setValue(minValue);
		}
	}
}

void SwitchMatrixModule::configSwitchParam(int id, const char* label) {
	configParam(id, -1.0f, 1.0f, 0.0f, label, "%", 0.0f, 100.0f);
	_switchParamQuantities.push_back(paramQuantities[id]);
}
