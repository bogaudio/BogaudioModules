
#include "bogaudio.hpp"

#include "VCO.hpp"
#include "XCO.hpp"
#include "Additator.hpp"
#include "FMOp.hpp"
#include "LFO.hpp"
#include "EightFO.hpp"

#include "Shaper.hpp"
#include "ShaperPlus.hpp"
#include "DADSRH.hpp"
#include "DADSRHPlus.hpp"

#include "Analyzer.hpp"

#include "RM.hpp"

#include "ADSR.hpp"
#include "Bool.hpp"
#include "CVDelay.hpp"
#include "Detune.hpp"
#include "DGate.hpp"
#include "FlipFlop.hpp"
#include "Follow.hpp"
#include "Manual.hpp"
#include "Mult.hpp"
#include "Noise.hpp"
#include "Offset.hpp"
#include "Pan.hpp"
#include "Reftone.hpp"
#include "SampleHold.hpp"
#include "Stack.hpp"
#include "Sums.hpp"
#include "Switch.hpp"
#include "VCA.hpp"
#include "VCAL.hpp"
#include "VCAmp.hpp"
#include "VU.hpp"

#include "Test.hpp"
#include "Test2.hpp"
#include "template_panels.hpp"

//NEW_INCLUDES_HERE

Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);
	p->website = "https://github.com/bogaudio/BogaudioModules";
	p->manual = "https://github.com/bogaudio/BogaudioModules/blob/master/README.md";

	p->addModel(modelVCO);
	p->addModel(modelXCO);
	p->addModel(modelAdditator);
	p->addModel(modelFMOp);
	p->addModel(modelLFO);
	p->addModel(modelEightFO);

	p->addModel(modelShaper);
	p->addModel(modelShaperPlus);
	p->addModel(modelDADSRH);
	p->addModel(modelDADSRHPlus);

	p->addModel(modelAnalyzer);

#ifdef EXPERIMENTAL
	p->addModel(modelRM);
#endif

#ifdef EXPERIMENTAL
	p->addModel(modelADSR);
	p->addModel(modelBool);
	p->addModel(modelCVDelay);
#endif
	p->addModel(modelDetune);
	p->addModel(modelDGate);
#ifdef EXPERIMENTAL
	p->addModel(modelFlipFlop);
	p->addModel(modelFollow);
#endif
	p->addModel(modelManual);
#ifdef EXPERIMENTAL
	p->addModel(modelMult);
#endif
	p->addModel(modelNoise);
	p->addModel(modelOffset);
#ifdef EXPERIMENTAL
	p->addModel(modelPan);
#endif
	p->addModel(modelReftone);
	p->addModel(modelSampleHold);
	p->addModel(modelStack);
#ifdef EXPERIMENTAL
	p->addModel(modelSums);
#endif
	p->addModel(modelSwitch);
	p->addModel(modelVCA);
#if 0
	p->addModel(modelVCAL);
#endif
#ifdef EXPERIMENTAL
	p->addModel(modelVCAmp);
	p->addModel(modelVU);
#endif

#ifdef TEST
	p->addModel(modelTest);
	p->addModel(modelTest2);

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

	//NEW_MODELS_HERE
}
