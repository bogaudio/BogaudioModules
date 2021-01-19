
#include "TestGl.hpp"

void TestGl::reset() {
}

void TestGl::sampleRateChange() {
	// float sampleRate = APP->engine->getSampleRate();
}

void TestGl::processChannel(const ProcessArgs& args, int c) {
}

struct TestGlDisplay : OpenGlWidget {
};

struct TestGlWidget : BGModuleWidget {
	static constexpr int hp = 20;

	TestGlWidget(TestGl* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);
		setPanel(box.size, "TestGl");
		createScrews();

		{
			auto inset = Vec(10, 25);
			auto size = Vec(box.size.x - 2*inset.x, 230);
			auto display = new TestGlDisplay();
			display->box.pos = inset;
			display->box.size = size;
			addChild(display);
		}
	}
};

Model* modelTestGl = createModel<TestGl, TestGlWidget>("Bogaudio-TestGl", "TESTGL", "");
