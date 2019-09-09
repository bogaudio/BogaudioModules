
#include "module.hpp"

using namespace bogaudio;

void BGModule::onReset() {
	_steps = _modulationSteps;
	reset();
}

void BGModule::onSampleRateChange() {
	_steps = _modulationSteps;
	sampleRateChange();
}

void BGModule::process(const ProcessArgs& args) {
	always(args);
	if (active()) {
		++_steps;
		if (_steps >= _modulationSteps) {
			_steps = 0;

			int channelsBefore = _channels;
			int channelsNow = channels();
			if (channelsBefore != channelsNow) {
				_channels = channelsNow;
				channelsChanged(channelsBefore, channelsNow);
			}

			modulate();
		}

		for (int i = 0; i < _channels; ++i) {
			processChannel(args, i);
		}
	}
}
