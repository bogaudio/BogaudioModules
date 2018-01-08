#include "BogaudioModules.hpp"

Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = "Bogaudio";
#if defined(VERSION)
	p->version = TOSTRING(VERSION);
	p->website = "https://github.com/bogaudio/BogaudioModules";
	p->manual = "https://github.com/bogaudio/BogaudioModules/blob/master/README.md";
#elif defined(REQUIRE_VERSION)
#error define VERSION=0.N.M to make dist
#endif

	p->addModel(createModel<ShaperWidget>("Bogaudio", "Bogaudio-Shaper", "Shaper", ENVELOPE_GENERATOR_TAG, AMPLIFIER_TAG));
	p->addModel(createModel<ShaperPlusWidget>("Bogaudio", "Bogaudio-ShaperPlus", "Shaper+", ENVELOPE_GENERATOR_TAG, AMPLIFIER_TAG));
	p->addModel(createModel<DADSRHWidget>("Bogaudio", "Bogaudio-DADSRH", "DADSR(H)", ENVELOPE_GENERATOR_TAG));
	p->addModel(createModel<DADSRHPlusWidget>("Bogaudio", "Bogaudio-DADSRHPlus", "DADSR(H)+", ENVELOPE_GENERATOR_TAG));

	p->addModel(createModel<AnalyzerWidget>("Bogaudio", "Bogaudio-Analyzer", "Analyzer", VISUAL_TAG));

	p->addModel(createModel<DGateWidget>("Bogaudio", "Bogaudio-DGate", "DGate", UTILITY_TAG));
	p->addModel(createModel<ManualWidget>("Bogaudio", "Bogaudio-Manual", "Manual", UTILITY_TAG));
	p->addModel(createModel<NoiseWidget>("Bogaudio", "Bogaudio-Noise", "Noise", NOISE_TAG, UTILITY_TAG));
	p->addModel(createModel<OffsetWidget>("Bogaudio", "Bogaudio-Offset", "Offset", ATTENUATOR_TAG, UTILITY_TAG));
	p->addModel(createModel<ReftoneWidget>("Bogaudio", "Bogaudio-Reftone", "Reftone", UTILITY_TAG));
	p->addModel(createModel<SampleHoldWidget>("Bogaudio", "Bogaudio-SampleHold", "S&H", SAMPLE_AND_HOLD_TAG, DUAL_TAG, UTILITY_TAG));
	p->addModel(createModel<SwitchWidget>("Bogaudio", "Bogaudio-Switch", "Switch", UTILITY_TAG));
	p->addModel(createModel<VCAWidget>("Bogaudio", "Bogaudio-VCA", "VCA", UTILITY_TAG, DUAL_TAG, AMPLIFIER_TAG));
}
