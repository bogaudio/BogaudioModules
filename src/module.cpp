
#include "module.hpp"

using namespace bogaudio;

void BGModule::onReset() {
	_steps = _modulationSteps;
	reset();
}

void BGModule::onSampleRateChange() {
	_modulationSteps = APP->engine->getSampleRate() * (2.5f / 1000.0f); // modulate every ~2.5ms regardless of sample rate.
	_steps = _modulationSteps;
	sampleRateChange();
}

void BGModule::process(const ProcessArgs& args) {
	if (_steps < 0) {
		onReset();
		onSampleRateChange();
	}

	processAlways(args);
	if (active()) {
		++_steps;
		if (_steps >= _modulationSteps) {
			_steps = 0;

			int channelsBefore = _channels;
			int channelsNow = std::max(1, channels());
			if (channelsBefore != channelsNow) {
				_channels = channelsNow;
				channelsChanged(channelsBefore, channelsNow);
				if (channelsBefore < channelsNow) {
					while (channelsBefore < channelsNow) {
						addChannel(channelsBefore);
						++channelsBefore;
					}
				}
				else {
					while (channelsNow < channelsBefore) {
						removeChannel(channelsBefore - 1);
						--channelsBefore;
					}
				}
			}

			modulate();
			for (int i = 0; i < _channels; ++i) {
				modulateChannel(i);
			}
		}

		processAll(args);
		for (int i = 0; i < _channels; ++i) {
			processChannel(args, i);
		}
		postProcess(args);
	}
	postProcessAlways(args);
}
