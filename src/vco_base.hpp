#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/resample.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"
#include <math.h>

using namespace bogaudio::dsp;

namespace bogaudio {

struct VCOBase : BGModule {
	struct Engine {
		static constexpr int oversample = 8;

		float frequency = INFINITY;
		float baseVOct = 0.0f;
		float baseHz = 0.0f;

		Phasor phasor;
		BandLimitedSquareOscillator square;
		BandLimitedSawOscillator saw;
		TriangleOscillator triangle;
		SineTableOscillator sine;
		CICDecimator squareDecimator;
		CICDecimator sawDecimator;
		CICDecimator triangleDecimator;
		float squareBuffer[oversample];
		float sawBuffer[oversample];
		float triangleBuffer[oversample];
		PositiveZeroCrossing syncTrigger;
		bogaudio::dsp::SlewLimiter squarePulseWidthSL;
		bool squareActive = false;
		bool sawActive = false;
		bool triangleActive = false;
		bool sineActive = false;
		float squareOut = 0.0f;
		float sawOut = 0.0f;
		float triangleOut = 0.0f;
		float sineOut = 0.0f;
		Phasor::phase_delta_t additionalPhaseOffset = 0;

		Engine() {
			saw.setQuality(12);
			square.setQuality(12);
		}
		void reset();
		void sampleRateChange(float sampleRate);
		void setFrequency(float frequency);
	};

	const float amplitude = 5.0f;
	const float slowModeOffset = -7.0f;
	Engine* _engines[maxChannels] {};
	float _oversampleThreshold = 0.0f;
	bool _slowMode = false;
	bool _linearMode = false;
	float _fmDepth = 0.0f;
	bool _fmLinearMode = false;
	int _frequencyParamID;
	int _fineFrequencyParamID;
	int _pitchInputID;
	int _syncInputID;
	int _fmInputID;
	int _polyInputID;
	bool _dcCorrection = true;

	struct VCOFrequencyParamQuantity : FrequencyParamQuantity {
		float offset() override;
		float getDisplayValue() override;
		void setDisplayValue(float v) override;
	};

	VCOBase(int fpID, int ffpID, int piID, int siID, int fiID)
	: _frequencyParamID(fpID)
	, _fineFrequencyParamID(ffpID)
	, _pitchInputID(piID)
	, _syncInputID(siID)
	, _fmInputID(fiID)
	, _polyInputID(piID)
	{}

	inline float linearModeVoltsToHertz(float v) { return _slowMode ? v : 1000.0f * v; }
	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

struct VCOBaseModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
