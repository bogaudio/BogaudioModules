#include "rack_overrides.hpp"

using namespace bogaudio;

bool Trigger::process(float in) {
	return rack::dsp::SchmittTrigger::process(in, _lowThreshold, _highThreshold);
}


bool NegativeTrigger::process(float in) {
	return Trigger::process(-in);
}
