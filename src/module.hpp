#pragma once

#include "rack.hpp"

using namespace rack;

namespace bogaudio {

struct BGModule : Module {
	int _modulationSteps = 100;
	int _steps = -1;

	static constexpr int maxChannels = PORT_MAX_CHANNELS;
	int _channels = 0;

	BGModule() {
	}
	virtual ~BGModule() {
		while (_channels >= 1) {
			removeChannel(_channels - 1);
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
	virtual void addChannel(int c) {}
	virtual void removeChannel(int c) {}
	virtual void processAlways(const ProcessArgs& args) {} // called before modulate()!
	virtual void modulate() {}
	virtual void modulateChannel(int c) {}
	virtual void processAll(const ProcessArgs& args) {}
	virtual void processChannel(const ProcessArgs& args, int c) {}
	virtual void postProcess(const ProcessArgs& args) {}
	virtual void postProcessAlways(const ProcessArgs& args) {} // modulate() may not have been called.
};

} // namespace bogaudio
