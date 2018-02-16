
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>

#include "rack.hpp"

#include "dsp/digital.hpp"
#include "trigger_on_load.hpp"
#include "widgets.hpp"

using namespace rack;
using namespace bogaudio;

extern Plugin *plugin;

extern Model* modelShaper;
extern Model* modelShaperPlus;
extern Model* modelDADSRH;
extern Model* modelDADSRHPlus;

extern Model* modelAnalyzer;

extern Model* modelDetune;
extern Model* modelDGate;
extern Model* modelManual;
extern Model* modelNoise;
extern Model* modelOffset;
extern Model* modelReftone;
extern Model* modelSampleHold;
extern Model* modelStack;
extern Model* modelSwitch;
extern Model* modelVCA;

extern Model* modelTest;
extern Model* modelSixHP;
