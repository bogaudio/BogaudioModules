#include "rack.hpp"
#include "widgets.hpp"

using namespace rack;

extern Plugin *plugin;

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
