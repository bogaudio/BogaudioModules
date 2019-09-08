
#include "module.hpp"

using namespace bogaudio;

void ModulatingBGModule::onReset() {
	_steps = _modulationSteps;
	reset();
}

void ModulatingBGModule::onSampleRateChange() {
	_steps = _modulationSteps;
	sampleRateChange();
}

void ModulatingBGModule::process(const ProcessArgs& args) {
	alwaysProcess(args);
	if (active()) {
		++_steps;
		if (_steps >= _modulationSteps) {
			_steps = 0;
			modulate();
		}
		processIfActive(args);
	}
}
