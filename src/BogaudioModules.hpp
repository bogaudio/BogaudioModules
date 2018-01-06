
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>

#include "rack.hpp"
#include "dsp/digital.hpp"
#include "widgets.hpp"

using namespace rack;
using namespace bogaudio;

extern Plugin *plugin;

namespace bogaudio {

struct ShaperWidget : ModuleWidget {
	ShaperWidget();
};

struct ShaperPlusWidget : ModuleWidget {
	ShaperPlusWidget();
};

struct DADSRHWidget : ModuleWidget {
	DADSRHWidget();
};

struct DADSRHPlusWidget : ModuleWidget {
	DADSRHPlusWidget();
};

struct AnalyzerWidget : ModuleWidget {
	AnalyzerWidget();
};


struct DGateWidget : ModuleWidget {
	DGateWidget();
};

struct OffsetWidget : ModuleWidget {
	OffsetWidget();
};

struct SampleHoldWidget : ModuleWidget {
	SampleHoldWidget();
};

struct ManualWidget : ModuleWidget {
	ManualWidget();
};

struct NoiseWidget : ModuleWidget {
	NoiseWidget();
};

struct ReftoneWidget : ModuleWidget {
	ReftoneWidget();
};

struct SwitchWidget : ModuleWidget {
	SwitchWidget();
};

} // namespae bogaudio
