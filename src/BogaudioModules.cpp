#include "BogaudioModules.hpp"

Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = "Bogaudio";
	p->name = "Bogaudio";
	createModel<ShaperWidget>(plugin, "Bogaudio-Shaper", "Shaper");
	createModel<ShaperPlusWidget>(plugin, "Bogaudio-ShaperPlus", "Shaper+");
	createModel<DADSRHWidget>(plugin, "Bogaudio-DADSRH", "DADSR(H)");
	createModel<DADSRHPlusWidget>(plugin, "Bogaudio-DADSRHPlus", "DADSR(H)+");
	createModel<OffsetWidget>(plugin, "Bogaudio-Offset", "Offset");
	createModel<SampleHoldWidget>(plugin, "Bogaudio-SampleHold", "S&H");
}
