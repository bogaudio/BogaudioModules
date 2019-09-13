#pragma once

#include "rack.hpp"

using namespace rack;

namespace bogaudio {

struct BGModule : Module {
	int _modulationSteps;
	int _steps = 0;

	static constexpr int maxChannels = PORT_MAX_CHANNELS;
	int _channels = 0;

	BGModule(int modulationSteps = 100) : _modulationSteps(modulationSteps) {
		_steps = _modulationSteps;
	}
	virtual ~BGModule() {
		while (_channels >= 1) {
			removeEngine(_channels - 1);
			--_channels;
		}
	}

	void onReset() override final;
	void onSampleRateChange() override final;
	void process(const ProcessArgs& args) override final;

	virtual void reset() {}
	virtual void sampleRateChange() {}
	virtual bool active() { return true; }
	virtual int channels() { return 1; }
	virtual void channelsChanged(int before, int after) {}
	virtual void addEngine(int c) {}
	virtual void removeEngine(int c) {}
	virtual void modulate() {}
	virtual void modulateChannel(int c) {}
	virtual void always(const ProcessArgs& args) {}
	virtual void processChannel(const ProcessArgs& args, int c) = 0;
};

} // namespace bogaudio
