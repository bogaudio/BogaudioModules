#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/experiments.hpp"
#include "dsp/filters/resample.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelTestVCF;

namespace bogaudio {

struct TestVCF : BGModule {
	enum ParamsIds {
		CUTOFF_PARAM,
		Q_PARAM,
		DRIVE_PARAM,
		TOPOLOGY_PARAM,
		MODE_PARAM,
		POLES_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		OUT_B_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LOWPASS_LIGHT,
		HIGHPASS_LIGHT,
		BANDPASS_LIGHT,
		BAND_REJECT_LIGHT,
		POLES_2_LIGHT,
		POLES_4_LIGHT,
		POLES_6_LIGHT,
		POLES_8_LIGHT,
		POLES_10_LIGHT,
		POLES_12_LIGHT,
		NUM_LIGHTS
	};

	enum Mode {
		LOWPASS_MODE,
		HIGHPASS_MODE,
		BANDPASS_MODE,
		BAND_REJECT_MODE
	};

	enum Poles {
		POLES_2,
		POLES_4,
		POLES_6,
		POLES_8,
		POLES_10,
		POLES_12
	};

	template<typename T>
	struct BicubicFilter : Filter {
		T _a0 = 0.0;
		T _a1 = 0.0;
		T _a2 = 0.0;
		T _a3 = 0.0;
		T _b1 = 0.0;
		T _b2 = 0.0 ;
		T _b3 = 0.0 ;

		T _x[4] {};
		T _y[4] {};

		void setParams(T a0, T a1, T a2, T a3, T b0, T b1, T b2, T b3) {
			if (b0 == 1.0) {
				_a0 = a0;
				_a1 = a1;
				_a2 = a2;
				_a3 = a3;
				_b1 = b1;
				_b2 = b2;
				_b3 = b3;
			}
			else {
				_a0 = a0 / b0;
				_a1 = a1 / b0;
				_a2 = a2 / b0;
				_a3 = a3 / b0;
				_b1 = b1 / b0;
				_b2 = b2 / b0;
				_b3 = b3 / b0;
			}
		}

		void reset() {
			_x[0] = _x[1] = _x[2] = _x[3] = 0.0;
			_y[0] = _y[1] = _y[2] = _y[3] = 0.0;
		}

		float next(float sample) override {
			_x[3] = _x[2];
			_x[2] = _x[1];
			_x[1] = _x[0];
			_x[0] = sample;

			_y[3] = _y[2];
			_y[2] = _y[1];
			_y[1] = _y[0];
			_y[0] = _a0 * _x[0];
			_y[0] += _a1 * _x[1];
			_y[0] += _a2 * _x[2];
			_y[0] += _a3 * _x[3];
			_y[0] -= _b1 * _y[1];
			_y[0] -= _b2 * _y[2];
			_y[0] -= _b3 * _y[3];

			return _y[0];
		}
	};

	template<typename T>
	struct BiquarticFilter : Filter {
		T _a0 = 0.0;
		T _a1 = 0.0;
		T _a2 = 0.0;
		T _a3 = 0.0;
		T _a4 = 0.0;
		T _b1 = 0.0;
		T _b2 = 0.0 ;
		T _b3 = 0.0 ;
		T _b4 = 0.0 ;

		T _x[5] {};
		T _y[5] {};

		void setParams(T a0, T a1, T a2, T a3, T a4, T b0, T b1, T b2, T b3, T b4) {
			if (b0 == 1.0) {
				_a0 = a0;
				_a1 = a1;
				_a2 = a2;
				_a3 = a3;
				_a4 = a4;
				_b1 = b1;
				_b2 = b2;
				_b3 = b3;
				_b4 = b4;
			}
			else {
				_a0 = a0 / b0;
				_a1 = a1 / b0;
				_a2 = a2 / b0;
				_a3 = a3 / b0;
				_a4 = a4 / b0;
				_b1 = b1 / b0;
				_b2 = b2 / b0;
				_b3 = b3 / b0;
				_b4 = b4 / b0;
			}
		}

		void reset() {
			_x[0] = _x[1] = _x[2] = _x[3] = _x[4] = 0.0;
			_y[0] = _y[1] = _y[2] = _y[3] = _y[4] = 0.0;
		}

		float next(float sample) override {
			_x[4] = _x[3];
			_x[3] = _x[2];
			_x[2] = _x[1];
			_x[1] = _x[0];
			_x[0] = sample;

			_y[4] = _y[3];
			_y[3] = _y[2];
			_y[2] = _y[1];
			_y[1] = _y[0];
			_y[0] = _a0 * _x[0];
			_y[0] += _a1 * _x[1];
			_y[0] += _a2 * _x[2];
			_y[0] += _a3 * _x[3];
			_y[0] += _a4 * _x[4];
			_y[0] -= _b1 * _y[1];
			_y[0] -= _b2 * _y[2];
			_y[0] -= _b3 * _y[3];
			_y[0] -= _b4 * _y[4];

			return _y[0];
		}
	};

	struct Model {
		virtual ~Model() {}
		virtual void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) = 0;
		virtual float next(float sample) = 0;
	};

	struct LPFModel : Model {
		LowPassFilter _filter;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	struct MultipoleModel : Model {
		MultipoleFilter _filter;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	struct ComplexModel : Model {
		ComplexBiquadFilter _pair1a;
		ComplexBiquadFilter _pair1b;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	struct BookExampleModel : Model {
		BiquadFilter<float> _filter1;
		BiquadFilter<float> _filter2;
		BiquadFilter<float> _filter3;
		BiquadFilter<float> _filter4;
		Poles _poles = POLES_2;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	struct ButterworthModel : Model {
		static constexpr int maxPoles = 12;
		BiquadFilter<float> _filters[maxPoles / 2];
		int _nFilters = 1;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	template<typename T>
	struct BPButterworthModel1 : Model {
		static constexpr int maxPoles = 12;
		BicubicFilter<T> _filters[maxPoles / 2];
		int _nFilters = 1;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	template<typename T>
	struct BandButterworthModel : Model {
		static constexpr int maxPoles = 12;
		BiquadFilter<T> _filters[maxPoles];
		int _nFilterPairs = 1;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	template<typename T>
	struct QuarticBandButterworthModel : Model {
		static constexpr int maxPoles = 12;
		BiquarticFilter<T> _filters[maxPoles / 2];
		int _nFilters = 1;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	struct AllPassModel : Model {
		BiquadFilter<float> _filter;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	template<typename T>
	struct ChebyshevModel : Model {
		static constexpr int maxPoles = 12;
		BiquadFilter<T> _filters[maxPoles];
		int _nFilters = 1;
		float _outGain = 1.0f;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		void setParamsLPHP(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology);
		void setParamsBPBR(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology);
		int polesToFilters(Mode mode, BiquadFilter<T>* fs, T x, T y, T wa, T W, T w02);
		void polesToFilterLPHP(Mode mode, BiquadFilter<T>& f, T x, T y, T wa);
		void polesToFiltersBPBR(Mode mode, BiquadFilter<T>& f0, BiquadFilter<T>& f1, T re, T im, T W, T w02);
		float next(float sample) override;
	};

	template<typename T>
	struct TwoPoleResonatorModel : Model {
		BiquadFilter<T> _filter;
		T _outGain = 0.0;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	template<typename M, int FACTOR>
	struct OversamplingModel : Model {
		M _model;
		CICInterpolator _interpolator;
		CICDecimator _decimator;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	template<typename M>
	struct FeedbackModel : Model {
		M _model;
		float _q = 0.0f;
		float _last = 0.0f;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	template<typename M, typename T>
	struct AddResonanceModel : Model {
		M _model;
		TwoPoleResonatorModel<T> _resonator;

		void setParams(float cutoff, float bandwidth, float resonance, Mode mode, Poles poles, float topology) override;
		float next(float sample) override;
	};

	static constexpr float maxCutoff = 22000.0f;
	Mode _mode = LOWPASS_MODE;
	Poles _poles = POLES_2;
	Model* _model = NULL;
	Model* _model2 = NULL;
	Amplifier _amplifier;
	Saturator _saturator;
	Saturator _saturator2;

	TestVCF() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CUTOFF_PARAM, 0.0f, 1.0f, 0.0f, "cutoff", " hz", 0.0f, maxCutoff);
		configParam(Q_PARAM, 0.0f, 1.0f, 0.0f, "resonance/bandwidth");
		configParam(DRIVE_PARAM, 0.0f, 1.0f, 0.0f, "drive", " dB", 0.0f, Amplifier::maxDecibels);
		configParam(TOPOLOGY_PARAM, 0.0f, 1.0f, 0.0f, "topology");
		configParam(MODE_PARAM, 0.0f, 3.0f, 0.0f, "mode");
		configParam(POLES_PARAM, 0.0f, 5.0f, 0.0f, "poles");

		// _model = new LPFModel();
		// _model = new MultipoleModel();
		// _model = new ComplexModel();
		// _model = new BookExampleModel();
		// _model = new ButterworthModel();
		// _model = new BPButterworthModel1<double>();
		// _model = new OversamplingModel<BPButterworthModel1<double>, 8>();
		// _model = new BandButterworthModel<double>();
		// _model = new OversamplingModel<BandButterworthModel<double>, 8>();
		// _model = new AllPassModel();
		// _model = new QuarticBandButterworthModel<double>();
		// _model = new OversamplingModel<QuarticBandButterworthModel<long double>, 32>();
		_model = new ChebyshevModel<double>();
		// _model = new OversamplingModel<ChebyshevModel<double>, 8>();
		// _model = new FeedbackModel<OversamplingModel<ChebyshevModel<double>, 8>>();
		// _model = new TwoPoleResonatorModel<double>();
		// _model = new AddResonanceModel<OversamplingModel<ChebyshevModel<double>, 8>, double>();

		// _model2 = new OversamplingModel<MultipoleModel, 8>();
		// _model2 = new FeedbackModel<OversamplingModel<ButterworthModel, 8>>();
		// _model2 = new OversamplingModel<ButterworthModel, 8>();
		_model2 = new OversamplingModel<BandButterworthModel<double>, 8>();
	}
	virtual ~TestVCF() {
		if (_model) {
			delete _model;
		}
		if (_model2) {
			delete _model2;
		}
	}

	void modulate() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
