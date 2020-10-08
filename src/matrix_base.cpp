
#include "matrix_base.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio;
using namespace bogaudio::dsp;

#define CLIPPING_MODE "clipping_mode"
#define INPUT_GAIN_DB "input_gain_db"
#define SUM "sum"

json_t* MatrixBaseModule::toJson(json_t* root) {
	json_object_set_new(root, CLIPPING_MODE, json_integer(_clippingMode));
	json_object_set_new(root, INPUT_GAIN_DB, json_real(_inputGainDb));
	json_object_set_new(root, SUM, json_boolean(_sum));
	return root;
}

void MatrixBaseModule::fromJson(json_t* root) {
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

	json_t* s = json_object_get(root, SUM);
	if (s) {
		_sum = json_is_true(s);
	}
}

void MatrixBaseModule::modulate() {
	_inputGainLevel = decibelsToAmplitude(_inputGainDb);
}


void MatrixModule::configMatrixModule(int ins, int outs, int firstParamID, int firstInputID, int firstOutputID) {
	assert(!_paramValues && !_sls && !_saturators && !_inActive);
	_ins = ins;
	_outs = outs;
	_firstParamID = firstParamID;
	_firstInputID = firstInputID;
	_firstOutputID = firstOutputID;
	assert(_ins <= maxN);
	assert(_outs <= maxN);
	_paramValues = new float[_ins * _outs] {};
	_sls = new bogaudio::dsp::SlewLimiter[_ins * _outs];
	_saturators = new Saturator[_outs * maxChannels];
	_inActive = new bool[_ins] {};
}

void MatrixModule::sampleRateChange() {
	float sr = APP->engine->getSampleRate();
	for (int i = 0, n = _ins * _outs; i < n; ++i) {
		_sls[i].setParams(sr, 0.5f, 1.0f);
	}
}

int MatrixModule::channels() {
	return inputs[_firstInputID].getChannels();
}

void MatrixModule::modulate() {
	MatrixBaseModule::modulate();

	bool solo = false;
	if (_muteParams) {
		for (int i = 0, n = _ins * _outs; i < n; ++i) {
			if (_muteParams[i]->getValue() > 1.5f) {
				solo = true;
				break;
			}
		}
	}

	int active = 0;
	for (int i = 0; i < _ins; ++i) {
		_inActive[i] = inputs[_firstInputID + i].isConnected();
		if (_inActive[i]) {
			++active;
		}

		for (int j = 0; j < _outs; ++j) {
			int ii = j * _ins + i;
			float v = params[_firstParamID + ii].getValue();
			if (_muteParams) {
				bool muted = solo ? _muteParams[ii]->getValue() < 2.0f : _muteParams[ii]->getValue() > 0.5f;
				v *= !muted;
			}
			_paramValues[ii] = _sls[ii].next(v);
		}
	}

	_invActive = (!_sum && active > 0) ? 1.0f / (float)active : 0.0f;
}

void MatrixModule::processChannel(const ProcessArgs& args, int c) {
	float in[maxN] {};
	for (int i = 0; i < _ins; ++i) {
		if (_inActive[i]) {
			in[i] = inputs[_firstInputID + i].getPolyVoltage(c) * _inputGainLevel;
		}
	}

	for (int i = 0; i < _outs; ++i) {
		if (!outputs[_firstOutputID + i].isConnected()) {
			continue;
		}
		float out = 0.0f;
		for (int j = 0; j < _ins; ++j) {
			if (_inActive[j]) {
				int ii = i * _ins + j;
				float cv = 1.0f;
				if (_cvInputs && _cvInputs[ii]->isConnected()) {
					cv = clamp(_cvInputs[ii]->getPolyVoltage(c) / 5.0f, -1.0f, 1.0f);
				}
				out += in[j] * _paramValues[ii] * cv;
			}
		}
		if (!_sum && _invActive > 0.0f) {
			out *= _invActive;
		}
		if (_clippingMode == HARD_CLIPPING) {
			out = clamp(out, -12.0f, 12.0f);
		}
		else {
			out = _saturators[c * _outs + i].next(out);
		}
		outputs[_firstOutputID + i].setChannels(_channels);
		outputs[_firstOutputID + i].setVoltage(out, c);
	}
}


void MatrixModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<MatrixModule*>(module);
	assert(m);

	if (m->_ins > 1) {
		OptionsMenuItem* g = new OptionsMenuItem("Input gain");
		g->addItem(OptionMenuItem("Unity", [m]() { return (int)m->_inputGainDb == 0; }, [m]() { m->_inputGainDb = 0.0f; }));
		g->addItem(OptionMenuItem("-3db", [m]() { return (int)m->_inputGainDb == -3; }, [m]() { m->_inputGainDb = -3.0f; }));
		g->addItem(OptionMenuItem("-6db", [m]() { return (int)m->_inputGainDb == -6; }, [m]() { m->_inputGainDb = -6.0f; }));
		g->addItem(OptionMenuItem("-12db", [m]() { return (int)m->_inputGainDb == -12; }, [m]() { m->_inputGainDb = -12.0f; }));
		OptionsMenuItem::addToMenu(g, menu);

		OptionsMenuItem* c = new OptionsMenuItem("Output clipping");
		c->addItem(OptionMenuItem("Soft/saturated (better for audio)", [m]() { return m->_clippingMode == MatrixBaseModule::SOFT_CLIPPING; }, [m]() { m->_clippingMode = MatrixBaseModule::SOFT_CLIPPING; }));
		c->addItem(OptionMenuItem("Hard/clipped (better for CV)", [m]() { return m->_clippingMode == MatrixBaseModule::HARD_CLIPPING; }, [m]() { m->_clippingMode = MatrixBaseModule::HARD_CLIPPING; }));
		OptionsMenuItem::addToMenu(c, menu);

		menu->addChild(new OptionMenuItem("Average", [m]() { return !m->_sum; }, [m]() { m->_sum = !m->_sum; }));
	}
}


#define INDICATOR_KNOBS "indicator_knobs"

json_t* KnobMatrixModule::toJson(json_t* root) {
	root = MatrixBaseModule::toJson(root);
	json_object_set_new(root, INDICATOR_KNOBS, json_boolean(_indicatorKnobs));
	return root;
}

void KnobMatrixModule::fromJson(json_t* root) {
	MatrixBaseModule::fromJson(root);
	json_t* k = json_object_get(root, INDICATOR_KNOBS);
	if (k) {
		_indicatorKnobs = json_is_true(k);
	}
}


void KnobMatrixModuleWidget::createKnob(math::Vec& position, KnobMatrixModule* module, int id) {
	auto knob = dynamic_cast<IndicatorKnob19*>(createParam<IndicatorKnob19>(position, module, id));
	if (module) {
		knob->setDrawColorsCallback([module]() { return module->_indicatorKnobs; });
	}
	addParam(knob);
	_knobs.push_back(knob);
}

void KnobMatrixModuleWidget::redrawKnobs() {
	for (IndicatorKnob19* knob : _knobs) {
		knob->redraw();
	}
}

void KnobMatrixModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<KnobMatrixModule*>(module);
	assert(m);
	MatrixModuleWidget::contextMenu(menu);
	menu->addChild(new OptionMenuItem(
		"Indicator knobs",
		[m]() { return m->_indicatorKnobs; },
		[m, this]() { m->_indicatorKnobs = !m->_indicatorKnobs; this->redrawKnobs(); }
	));
}


#define INVERTING "inverting"
#define INVERTING_CLICK "click"
#define INVERTING_PARAM "param"
#define INVERTING_DISABLED "disabled"

#define ROW_EXCLUSIVE "row_exclusive"
#define COLUMN_EXCLUSIVE "column_exclusive"

json_t* SwitchMatrixModule::toJson(json_t* root) {
	root = MatrixBaseModule::toJson(root);

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

	json_object_set_new(root, ROW_EXCLUSIVE, json_boolean(_rowExclusive));
	json_object_set_new(root, COLUMN_EXCLUSIVE, json_boolean(_columnExclusive));

	return root;
}

void SwitchMatrixModule::fromJson(json_t* root) {
	MatrixBaseModule::fromJson(root);

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

	json_t* re = json_object_get(root, ROW_EXCLUSIVE);
	if (re) {
		_rowExclusive = json_is_true(re);
	}
	json_t* ce = json_object_get(root, COLUMN_EXCLUSIVE);
	if (ce) {
		_columnExclusive = json_is_true(ce);
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

void SwitchMatrixModule::switchChanged(int id, float value) {
	if (value != 0.0f) {
		int row = (id - _firstParamID) % _ins;
		int col = (id - _firstParamID) / _ins;

		if (_rowExclusive) {
			for (int i = 0; i < col; ++i) {
				_switchParamQuantities[i * _ins + row]->setValue(0.0f);
			}
			for (int i = col + 1; i < _outs; ++i) {
				_switchParamQuantities[i * _ins + row]->setValue(0.0f);
			}
		}

		if (_columnExclusive) {
			for (int i = 0; i < row; ++i) {
				_switchParamQuantities[col * _ins + i]->setValue(0.0f);
			}
			for (int i = row + 1; i < _ins; ++i) {
				_switchParamQuantities[col * _ins + i]->setValue(0.0f);
			}
		}
	}
}

void SwitchMatrixModule::setRowExclusive(bool e) {
	_rowExclusive = e;
	if (e) {
		for (int i = 0; i < _ins; ++i) {
			int j = 0;
			for (; j < _outs; ++j) {
				if (_switchParamQuantities[j * _ins + i]->getValue() != 0.0f) {
					break;
				}
			}
			++j;
			for (; j < _outs; ++j) {
				_switchParamQuantities[j * _ins + i]->setValue(0.0f);
			}
		}
	}
}

void SwitchMatrixModule::setColumnExclusive(bool e) {
	_columnExclusive = e;
	if (e) {
		for (int i = 0; i < _outs; ++i) {
			int j = 0;
			for (; j < _ins; ++j) {
				if (_switchParamQuantities[i * _ins + j]->getValue() != 0.0f) {
					break;
				}
			}
			++j;
			for (; j < _ins; ++j) {
				_switchParamQuantities[i * _ins + j]->setValue(0.0f);
			}
		}
	}
}


void SwitchMatrixModuleWidget::contextMenu(Menu* menu) {
	auto m = dynamic_cast<SwitchMatrixModule*>(module);
	assert(m);
	MatrixModuleWidget::contextMenu(menu);

	OptionsMenuItem* i = new OptionsMenuItem("Inverting");
	i->addItem(OptionMenuItem("By param entry (right-click)", [m]() { return m->_inverting == SwitchMatrixModule::PARAM_INVERTING; }, [m]() { m->setInverting(SwitchMatrixModule::PARAM_INVERTING); }));
	i->addItem(OptionMenuItem("On second click", [m]() { return m->_inverting == SwitchMatrixModule::CLICK_INVERTING; }, [m]() { m->setInverting(SwitchMatrixModule::CLICK_INVERTING); }));
	i->addItem(OptionMenuItem("Disabled", [m]() { return m->_inverting == SwitchMatrixModule::NO_INVERTING; }, [m]() { m->setInverting(SwitchMatrixModule::NO_INVERTING); }));
	OptionsMenuItem::addToMenu(i, menu);

	if (m->_ins > 1) {
		std::string label("Exclusive switching");
		if (m->_outs > 1) {
			label += " by row";
		}
		menu->addChild(new OptionMenuItem(label.c_str(), [m]() { return m->_columnExclusive; }, [m]() { m->setColumnExclusive(!m->_columnExclusive); }));
	}
	if (m->_outs > 1) {
		std::string label("Exclusive switching");
		if (m->_ins > 1) {
			label += " by column";
		}
		menu->addChild(new OptionMenuItem(label.c_str(), [m]() { return m->_rowExclusive; }, [m]() { m->setRowExclusive(!m->_rowExclusive); }));
	}
}
