
#include "bogaudio.hpp"

#include "AD.hpp"
#include "ADSR.hpp"
#include "Additator.hpp"
#include "AddrSeq.hpp"
#include "AddrSeqX.hpp"
#include "AMRM.hpp"
#include "Analyzer.hpp"
#include "AnalyzerXL.hpp"
#include "Arp.hpp"
#include "ASR.hpp"
#include "Assign.hpp"
#include "Blank3.hpp"
#include "Blank6.hpp"
#include "Bool.hpp"
#include "Chirp.hpp"
#include "Clpr.hpp"
#include "Cmp.hpp"
#include "CmpDist.hpp"
#include "CVD.hpp"
#include "DADSRH.hpp"
#include "DADSRHPlus.hpp"
#include "DGate.hpp"
#include "Detune.hpp"
#include "Edge.hpp"
#include "EightFO.hpp"
#include "EightOne.hpp"
#include "EQ.hpp"
#include "EQS.hpp"
#include "FFB.hpp"
#include "FMOp.hpp"
#include "FourMan.hpp"
#include "FlipFlop.hpp"
#include "Follow.hpp"
#include "Inv.hpp"
#include "Lag.hpp"
#include "LFO.hpp"
#include "LLFO.hpp"
#include "LLPG.hpp"
#include "Lmtr.hpp"
#include "LLPG.hpp"
#include "LPG.hpp"
#include "LVCF.hpp"
#include "LVCO.hpp"
#include "Manual.hpp"
#include "Matrix18.hpp"
#include "Matrix44.hpp"
#include "Matrix44Cvm.hpp"
#include "Matrix81.hpp"
#include "Matrix88.hpp"
#include "Matrix88Cv.hpp"
#include "Matrix88M.hpp"
#include "MegaGate.hpp"
#include "Mix1.hpp"
#include "Mix2.hpp"
#include "Mix4.hpp"
#include "Mix4x.hpp"
#include "Mix8.hpp"
#include "Mix8x.hpp"
#include "Mono.hpp"
#include "Mult.hpp"
#include "Mumix.hpp"
#include "Mute8.hpp"
#include "Noise.hpp"
#include "Nsgt.hpp"
#include "Offset.hpp"
#include "OneEight.hpp"
#include "Pan.hpp"
#include "PEQ.hpp"
#include "PEQ6.hpp"
#include "PEQ6XF.hpp"
#include "PEQ14.hpp"
#include "PEQ14XF.hpp"
#include "PEQ14XR.hpp"
#include "PEQ14XV.hpp"
#include "Pgmr.hpp"
#include "PgmrX.hpp"
#include "PolyCon8.hpp"
#include "PolyCon16.hpp"
#include "PolyOff8.hpp"
#include "PolyOff16.hpp"
#include "PolyMult.hpp"
#include "Pressor.hpp"
#include "Pulse.hpp"
#include "Ranalyzer.hpp"
#include "Reftone.hpp"
#include "RGate.hpp"
#include "SampleHold.hpp"
#include "Shaper.hpp"
#include "ShaperPlus.hpp"
#include "Sine.hpp"
#include "Slew.hpp"
#include "Stack.hpp"
#include "Sums.hpp"
#include "Switch.hpp"
#include "Switch1616.hpp"
#include "Switch18.hpp"
#include "Switch44.hpp"
#include "Switch81.hpp"
#include "Switch88.hpp"
#include "UMix.hpp"
#include "Unison.hpp"
#include "VCA.hpp"
#include "VCAmp.hpp"
#include "VCF.hpp"
#include "VCM.hpp"
#include "VCO.hpp"
#include "Velo.hpp"
#include "Vish.hpp"
#include "VU.hpp"
#include "Walk.hpp"
#include "Walk2.hpp"
#include "XCO.hpp"
#include "XFade.hpp"

#include "Test.hpp"
#include "Test2.hpp"
#include "TestExpander.hpp"
#include "TestVCF.hpp"

//NEW_INCLUDES_HERE

Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;

	p->addModel(modelVCO);
	p->addModel(modelLVCO);
	p->addModel(modelSine);
	p->addModel(modelPulse);
	p->addModel(modelXCO);
	p->addModel(modelAdditator);
	p->addModel(modelFMOp);
	p->addModel(modelChirp);

	p->addModel(modelLFO);
	p->addModel(modelEightFO);
	p->addModel(modelLLFO);

	p->addModel(modelVCF);
	p->addModel(modelLVCF);
	p->addModel(modelFFB);
	p->addModel(modelEQ);
	p->addModel(modelEQS);
	p->addModel(modelLPG);
	p->addModel(modelLLPG);
	p->addModel(modelMegaGate);
	p->addModel(modelPEQ);
	p->addModel(modelPEQ6);
	p->addModel(modelPEQ6XF);
	p->addModel(modelPEQ14);
	p->addModel(modelPEQ14XF);

	p->addModel(modelDADSRH);
	p->addModel(modelDADSRHPlus);
	p->addModel(modelShaper);
	p->addModel(modelShaperPlus);
	p->addModel(modelAD);
	p->addModel(modelASR);
	p->addModel(modelADSR);
	p->addModel(modelVish);
	p->addModel(modelFollow);
	p->addModel(modelDGate);
	p->addModel(modelRGate);
	p->addModel(modelEdge);

	p->addModel(modelNoise);
	p->addModel(modelSampleHold);
	p->addModel(modelWalk2);
	p->addModel(modelWalk);

	p->addModel(modelMix8);
	p->addModel(modelMix8x);
	p->addModel(modelMix4);
	p->addModel(modelMix4x);
	p->addModel(modelMix2);
	p->addModel(modelMix1);
	p->addModel(modelVCM);
	p->addModel(modelMute8);
	p->addModel(modelPan);
	p->addModel(modelXFade);
	p->addModel(modelVCA);
	p->addModel(modelVCAmp);
	p->addModel(modelVelo);

	p->addModel(modelUMix);
	p->addModel(modelMumix);
	p->addModel(modelMatrix81);
	p->addModel(modelMatrix18);
	p->addModel(modelMatrix44);
	p->addModel(modelMatrix44Cvm);
	p->addModel(modelMatrix88);
	p->addModel(modelMatrix88Cv);
	p->addModel(modelMatrix88M);
	p->addModel(modelSwitch81);
	p->addModel(modelSwitch18);
	p->addModel(modelSwitch44);
	p->addModel(modelSwitch88);
	p->addModel(modelSwitch1616);

	p->addModel(modelAMRM);
	p->addModel(modelPressor);
	p->addModel(modelClpr);
	p->addModel(modelLmtr);
	p->addModel(modelNsgt);
	p->addModel(modelCmpDist);

	p->addModel(modelOneEight);
	p->addModel(modelEightOne);
	p->addModel(modelAddrSeq);
	p->addModel(modelAddrSeqX);
	p->addModel(modelPgmr);
	p->addModel(modelPgmrX);

	p->addModel(modelVU);
	p->addModel(modelAnalyzer);
	p->addModel(modelAnalyzerXL);
	p->addModel(modelRanalyzer);

	p->addModel(modelDetune);
	p->addModel(modelStack);
	p->addModel(modelReftone);

	p->addModel(modelMono);
	p->addModel(modelArp);
	p->addModel(modelAssign);
	p->addModel(modelUnison);
	p->addModel(modelPolyCon8);
	p->addModel(modelPolyCon16);
	p->addModel(modelPolyOff8);
	p->addModel(modelPolyOff16);
	p->addModel(modelPolyMult);

	p->addModel(modelBool);
	p->addModel(modelCmp);
	p->addModel(modelCVD);
	p->addModel(modelFlipFlop);
	p->addModel(modelInv);
	p->addModel(modelManual);
	p->addModel(modelFourMan);
	p->addModel(modelMult);
	p->addModel(modelOffset);
	p->addModel(modelSlew);
	p->addModel(modelSums);
	p->addModel(modelSwitch);

	p->addModel(modelBlank3);
	p->addModel(modelBlank6);

#ifdef EXPERIMENTAL
	p->addModel(modelLag);
	p->addModel(modelPEQ14XR);
	p->addModel(modelPEQ14XV);
#endif

#ifdef TEST
	p->addModel(modelTest);
	p->addModel(modelTest2);
	p->addModel(modelTestExpanderBase);
	p->addModel(modelTestExpanderExtension);
	p->addModel(modelTestVCF);
#endif

	//NEW_MODELS_HERE
}
