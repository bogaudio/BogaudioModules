
// #define LPF 1
// #define SINE 1
// #define SQUARE 1
// #define SAW 1
// #define TRIANGLE 1
#define SINEBANK 1

#include "bogaudio.hpp"
#include "pitch.hpp"
#ifdef LPF
#include "dsp/filter.hpp"
#elif SINE
#include "dsp/oscillator.hpp"
#elif SQUARE
#include "dsp/oscillator.hpp"
#elif SAW
#include "dsp/oscillator.hpp"
#elif TRIANGLE
#include "dsp/oscillator.hpp"
#elif SINEBANK
#include "dsp/oscillator.hpp"
#else
#error what
#endif

using namespace bogaudio::dsp;

struct Test : Module {
	enum ParamsIds {
		PARAM1_PARAM,
		PARAM2_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		CV2_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

#ifdef LPF
	LowPassFilter _lpf;
#elif SINE
	SineOscillator _sine;
#elif SQUARE
	SquareOscillator _square;
#elif SAW
	SawOscillator _saw;
#elif TRIANGLE
	TriangleOscillator _triangle;
#elif SINEBANK
	SineBankOscillator _sineBank;
	SineBankOscillator2 _sineBank2;
#endif

	Test()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
#ifdef LPF
	, _lpf(44100.0, 1000.0, 1.0)
#elif SINE
	, _sine(44100.0, 1000.0, 5.0)
#elif SQUARE
	, _square(44100.0, 1000.0, 5.0)
#elif SAW
	, _saw(44100.0, 1000.0, 5.0)
#elif TRIANGLE
	, _triangle(44100.0, 1000.0, 5.0)
#elif SINEBANK
	, _sineBank(44101.0, 1000.0, 50)
	, _sineBank2(44101.0, 1000.0, 50)
#endif
	{
		onReset();

#ifdef SINEBANK
    const float baseAmplitude = 5.0;
		switch (5) {
			case 1: {
				// saw
				float phase = M_PI;
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, baseAmplitude / (float)i, &phase);
				}
				for (int i = 1, n = _sineBank2.partialCount(); i <= n; ++i) {
					_sineBank2.setPartial(i, i, baseAmplitude / (float)i, &phase);
				}
				break;
			}

			case 2: {
				// square
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)i : 0.0);
				}
				for (int i = 1, n = _sineBank2.partialCount(); i <= n; ++i) {
					_sineBank2.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)i : 0.0);
				}
				break;
			}

			case 3: {
				// triangle
				float phase = M_PI / 2.0;
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)(i * i) : 0.0, &phase);
				}
				for (int i = 1, n = _sineBank2.partialCount(); i <= n; ++i) {
					_sineBank2.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)(i * i) : 0.0, &phase);
				}
				break;
			}

			case 4: {
				// saw-square
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)i : baseAmplitude / (float)(2 * i));
				}
				for (int i = 1, n = _sineBank2.partialCount(); i <= n; ++i) {
					_sineBank2.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)i : baseAmplitude / (float)(2 * i));
				}
				break;
			}

			case 5: {
				// ?
				float phase = 0.0;
				float factor = 0.717;
				float factor2 = factor;
				{
					float multiple = 1.0;
					for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
						_sineBank.setPartial(i, multiple, baseAmplitude / multiple, &phase);
						multiple += i % 2 == 1 ? factor : factor2;
					}
				}
				{
					float multiple = 1.0;
					for (int i = 1, n = _sineBank2.partialCount(); i <= n; ++i) {
						_sineBank2.setPartial(i, multiple, baseAmplitude / multiple, &phase);
						multiple += i % 2 == 1 ? factor : factor2;
					}
				}
				break;
			}
		}
#endif
	}

	virtual void onReset() override;
	virtual void step() override;
	float oscillatorPitch();
};

void Test::onReset() {
}

void Test::step() {
	if (!outputs[OUT_OUTPUT].active) {
		return;
	}

#ifdef LPF
	if (!inputs[IN_INPUT].active) {
		return;
	}
	_lpf.setParams(
		engineGetSampleRate(),
		10000.0 * clamp(params[PARAM1_PARAM].value, 0.0f, 1.0f),
		std::max(10.0 * clamp(params[PARAM2_PARAM].value, 0.0f, 1.0f), 0.1)
	);
	outputs[OUT_OUTPUT].value = _lpf.next(inputs[IN_INPUT].value);

#elif SINE
	_sine.setSampleRate(engineGetSampleRate());
	_sine.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].value = _sine.next();

#elif SQUARE
	_square.setSampleRate(engineGetSampleRate());
	_square.setFrequency(oscillatorPitch());
	float pw = params[PARAM2_PARAM].value;
	if (inputs[CV2_INPUT].active) {
		pw += clamp(inputs[CV2_INPUT].value, -5.0f, 5.0f) / 10.0f;
	}
	_square.setPulseWidth(pw);
	outputs[OUT_OUTPUT].value = _square.next();

#elif SAW
	_saw.setSampleRate(engineGetSampleRate());
	_saw.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].value = _saw.next();

#elif TRIANGLE
	_triangle.setSampleRate(engineGetSampleRate());
	_triangle.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].value = _triangle.next();

#elif SINEBANK
	_sineBank.setSampleRate(engineGetSampleRate());
	_sineBank.setFrequency(oscillatorPitch());
	outputs[OUT_OUTPUT].value = _sineBank.next();
	_sineBank2.setSampleRate(engineGetSampleRate());
	_sineBank2.setFrequency(oscillatorPitch());
	outputs[OUT2_OUTPUT].value = _sineBank2.next();
#endif
}

float Test::oscillatorPitch() {
	if (inputs[CV1_INPUT].active) {
		return cvToFrequency(inputs[CV1_INPUT].value);
	}
	return 10000.0 * powf(params[PARAM1_PARAM].value, 2.0);
}


struct TestWidget : ModuleWidget {
	TestWidget(Test* module);
};

TestWidget::TestWidget(Test* module) : ModuleWidget(module) {
	box.size = Vec(RACK_GRID_WIDTH * 3, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Test.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

	// generated by svg_widgets.rb
	auto param1ParamPosition = Vec(9.5, 28.5);
	auto param2ParamPosition = Vec(9.5, 128.5);

	auto cv1InputPosition = Vec(10.5, 66.0);
	auto cv2InputPosition = Vec(10.5, 168.0);
	auto inInputPosition = Vec(10.5, 258.0);

	auto outOutputPosition = Vec(10.5, 296.0);
	auto out2OutputPosition = Vec(20.5, 316.0);
	// end generated by svg_widgets.rb

	addParam(ParamWidget::create<Knob26>(param1ParamPosition, module, Test::PARAM1_PARAM, 0.0, 1.0, 0.5));
	addParam(ParamWidget::create<Knob26>(param2ParamPosition, module, Test::PARAM2_PARAM, 0.0, 1.0, 0.5));

	addInput(Port::create<Port24>(cv1InputPosition, Port::INPUT, module, Test::CV1_INPUT));
	addInput(Port::create<Port24>(cv2InputPosition, Port::INPUT, module, Test::CV2_INPUT));
	addInput(Port::create<Port24>(inInputPosition, Port::INPUT, module, Test::IN_INPUT));

	addOutput(Port::create<Port24>(outOutputPosition, Port::OUTPUT, module, Test::OUT_OUTPUT));
	addOutput(Port::create<Port24>(out2OutputPosition, Port::OUTPUT, module, Test::OUT2_OUTPUT));
}


Model* modelTest = Model::create<Test, TestWidget>("Bogaudio", "Bogaudio-Test", "Test");
