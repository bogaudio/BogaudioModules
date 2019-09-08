#pragma once

#include "rack.hpp"

using namespace rack;

namespace bogaudio {

struct BGModule : Module {
};

struct ModulatingBGModule : BGModule {
	int _modulationSteps;
	int _steps = 0;

	ModulatingBGModule(int modulationSteps = 100) : _modulationSteps(modulationSteps) {
		_steps = _modulationSteps;
	}

	void onReset() override final;
	void onSampleRateChange() override final;
	void process(const ProcessArgs& args) override final;

	virtual void reset() {}
	virtual void sampleRateChange() {}
	virtual bool active() { return true; }
	virtual void modulate() = 0;
	virtual void alwaysProcess(const ProcessArgs& args) {}
	virtual void processIfActive(const ProcessArgs& args) = 0;
};

} // namespace bogaudio
