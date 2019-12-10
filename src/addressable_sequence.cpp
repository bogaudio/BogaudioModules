
#include "addressable_sequence.hpp"

#define POLY_INPUT "poly_input"
#define SELECT_ON_CLOCK "select_on_clock"
#define TRIGGERED_SELECT "triggered_select"

void AddressableSequenceModule::reset() {
	for (int i = 0; i < maxChannels; ++i) {
		_step[i] = 0;
		_select[i] = 0;
		_clock[i].reset();
		_reset[i].reset();
		_selectTrigger[i].reset();
	}
}

void AddressableSequenceModule::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int i = 0; i < maxChannels; ++i) {
		_timer[i].setParams(sr, 0.001f);
	}
}

json_t* AddressableSequenceModule::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, POLY_INPUT, json_integer(_polyInputID));
	json_object_set_new(root, SELECT_ON_CLOCK, json_boolean(_selectOnClock));
	json_object_set_new(root, TRIGGERED_SELECT, json_boolean(_triggeredSelect));
	return root;
}

void AddressableSequenceModule::dataFromJson(json_t* root) {
	json_t* p = json_object_get(root, POLY_INPUT);
	if (p) {
		_polyInputID = json_integer_value(p);
	}

	json_t* s = json_object_get(root, SELECT_ON_CLOCK);
	if (s) {
		_selectOnClock = json_is_true(s);
	}

	json_t* t = json_object_get(root, TRIGGERED_SELECT);
	if (t) {
		_triggeredSelect = json_is_true(t);
	}
}

int AddressableSequenceModule::channels() {
	return _polyInputID == _selectInputID ? inputs[_selectInputID].getChannels() : inputs[_clockInputID].getChannels();
}

int AddressableSequenceModule::nextStep(
	int c,
	Input& resetInput,
	Input& clockInput,
	Param& stepsParam,
	Param& directionParam,
	Param& selectParam,
	Input& selectInput
) {
	bool reset = _reset[c].process(resetInput.getVoltage());
	if (reset) {
		_timer[c].reset();
	}
	bool timer = _timer[c].next();
	bool clock = _clock[c].process(clockInput.getPolyVoltage(c)) && !timer;

	int steps = clamp(stepsParam.getValue(), 1.0f, 8.0f);
	int reverse = 1 - 2 * (directionParam.getValue() == 0.0f);
	_step[c] = (_step[c] + reverse * clock) % steps;
	_step[c] += (_step[c] < 0) * steps;
	_step[c] -= _step[c] * reset;

	float select = selectParam.getValue();
	if (_triggeredSelect) {
		if (_selectTrigger[c].process(selectInput.getPolyVoltage(c))) {
			_select[c] = (1 + (int)_select[c]) % ((int)select + 1);
		}
		_select[c] -= _select[c] * reset;
	}
	else {
		select += clamp(selectInput.getPolyVoltage(c), 0.0f, 10.0f) * 0.1f * 8.0f;
		if (!_selectOnClock || clock) {
			_select[c] = select;
		}
	}
	return (_step[c] + (int)_select[c]) % 8;
}
