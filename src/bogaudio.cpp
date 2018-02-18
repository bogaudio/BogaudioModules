#include "bogaudio.hpp"

Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = "Bogaudio"; // TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);
	p->website = "https://github.com/bogaudio/BogaudioModules";
	p->manual = "https://github.com/bogaudio/BogaudioModules/blob/master/README.md";

	p->addModel(modelShaper);
	p->addModel(modelShaperPlus);
	p->addModel(modelDADSRH);
	p->addModel(modelDADSRHPlus);

	p->addModel(modelAnalyzer);

	p->addModel(modelDetune);
	p->addModel(modelDGate);
	p->addModel(modelManual);
	p->addModel(modelNoise);
	p->addModel(modelOffset);
	p->addModel(modelReftone);
	p->addModel(modelSampleHold);
	p->addModel(modelStack);
	p->addModel(modelSwitch);
	p->addModel(modelVCA);

#ifdef TEST
	p->addModel(modelTest);

	p->addModel(modelThreeHP);
	p->addModel(modelSixHP);
	p->addModel(modelEightHP);
	p->addModel(modelTenHP);
	p->addModel(modelTwelveHP);
	p->addModel(modelThirteenHP);
	p->addModel(modelFifteenHP);
	p->addModel(modelEighteenHP);
	p->addModel(modelTwentyHP);
	p->addModel(modelTwentyTwoHP);
	p->addModel(modelTwentyFiveHP);
	p->addModel(modelThirtyHP);
#endif
}
