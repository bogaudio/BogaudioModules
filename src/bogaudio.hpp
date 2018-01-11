
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

namespace bogaudio {

struct ShaperWidget : ModuleWidget {
	ShaperWidget();
	virtual Menu* createContextMenu() override;
};

struct ShaperPlusWidget : ModuleWidget {
	ShaperPlusWidget();
	virtual Menu* createContextMenu() override;
};

struct DADSRHWidget : ModuleWidget {
	DADSRHWidget();
	virtual Menu* createContextMenu() override;
};

struct DADSRHPlusWidget : ModuleWidget {
	DADSRHPlusWidget();
	virtual Menu* createContextMenu() override;
};

struct AnalyzerWidget : ModuleWidget {
	AnalyzerWidget();
};


struct DGateWidget : ModuleWidget {
	DGateWidget();
	virtual Menu* createContextMenu() override;
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

struct VCAWidget : ModuleWidget {
	VCAWidget();
};


struct TestWidget : ModuleWidget {
	TestWidget();
};

} // namespae bogaudio
