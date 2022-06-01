
#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

namespace bogaudio {

struct PitchModeListener {
	virtual void pitchModeChanged() = 0;
};

struct LFOBase : BGModule {
	struct OffsetParamQuantity : ParamQuantity {
		float getDisplayValue() override {
			float v = getValue();
			if (!module) {
				return v;
			}
			auto m = dynamic_cast<LFOBase*>(module);
			return v * m->_offsetScale * 5.0f;
		}

		void setDisplayValue(float dv) override {
			if (!module) {
				return;
			}
			auto m = dynamic_cast<LFOBase*>(module);
			setValue(dv / (m->_offsetScale * 5.0f));
		}
	};

	struct Smoother {
		float _sampleRate = 0.0f;
		float _frequency = 0.0f;
		float _amount = 0.0f;
		ShapedSlewLimiter _slewLimiter;

		void setParams(float sampleRate, float frequency, float amount);
		float next(float sample);
	};

	bool _slowMode = false;
	float _offsetScale = 1.0f;
	PitchModeListener* _pitchModeListener = NULL;

	struct LFOFrequencyParamQuantity : FrequencyParamQuantity {
		float offset() override;
	};

	LFOBase(int np, int ni, int no, int nl = 0) {
		config(np, ni, no, nl);
	}

	float getPitchOffset();
	void setFrequency(Param& frequency, Input& pitch, Phasor& phasor, int c = 0); // FIXME
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
};

struct LFOBaseModuleWidget : BGModuleWidget {
	void contextMenu(Menu* menu) override;
};

} // namespace bogaudio
