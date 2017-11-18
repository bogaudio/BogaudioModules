#include "BogaudioModules.hpp"

Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = "Bogaudio";
#ifdef VERSION
	p->version = TOSTRING(VERSION);
#endif
	p->addModel(createModel<ShaperWidget>("Bogaudio", "Bogaudio-Shaper", "Shaper", ENVELOPE_GENERATOR_TAG, AMPLIFIER_TAG));
	p->addModel(createModel<ShaperPlusWidget>("Bogaudio", "Bogaudio-ShaperPlus", "Shaper+", ENVELOPE_GENERATOR_TAG, AMPLIFIER_TAG));
	p->addModel(createModel<DADSRHWidget>("Bogaudio", "Bogaudio-DADSRH", "DADSR(H)", ENVELOPE_GENERATOR_TAG));
	p->addModel(createModel<DADSRHPlusWidget>("Bogaudio", "Bogaudio-DADSRHPlus", "DADSR(H)+", ENVELOPE_GENERATOR_TAG));
	p->addModel(createModel<OffsetWidget>("Bogaudio", "Bogaudio-Offset", "Offset", ATTENUATOR_TAG));
	p->addModel(createModel<SampleHoldWidget>("Bogaudio", "Bogaudio-SampleHold", "S&H", SAMPLE_AND_HOLD_TAG, DUAL_TAG));
}
