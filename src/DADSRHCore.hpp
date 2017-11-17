
#include "dsp/digital.hpp"
#include "BogaudioModules.hpp"

struct DADSRHCore {
	enum Stage {
		STOPPED_STAGE,
		DELAY_STAGE,
		ATTACK_STAGE,
		DECAY_STAGE,
		SUSTAIN_STAGE,
		RELEASE_STAGE
	};

	Param& _delayParam;
	Param& _attackParam;
	Param& _decayParam;
	Param& _sustainParam;
	Param& _releaseParam;
	Param& _holdParam;
	Param& _attackShapeParam;
	Param& _decayShapeParam;
	Param& _releaseShapeParam;
	Param& _triggerParam;
	Param& _modeParam;
	Param& _loopParam;
	Param& _speedParam;
	Param& _retriggerParam;

	Input* _delayInput;
	Input* _attackInput;
	Input* _decayInput;
	Input* _sustainInput;
	Input* _releaseInput;
	Input* _holdInput;
	Input& _triggerInput;

	Output* _delayOutput;
	Output* _attackOutput;
	Output* _decayOutput;
	Output* _sustainOutput;
	Output* _releaseOutput;
	Output& _envOutput;
	Output& _invOutput;
	Output& _triggerOutput;

	float& _delayLight;
	float& _attackLight;
	float& _decayLight;
	float& _sustainLight;
	float& _releaseLight;
	float& _attackShape1Light;
	float& _attackShape2Light;
	float& _attackShape3Light;
	float& _decayShape1Light;
	float& _decayShape2Light;
	float& _decayShape3Light;
	float& _releaseShape1Light;
	float& _releaseShape2Light;
	float& _releaseShape3Light;

	SchmittTrigger _trigger;
	Stage _stage;
	float _envelope, _stageProgress, _holdProgress, _releaseLevel;

	DADSRHCore(
		Param& delayParam,
		Param& attackParam,
		Param& decayParam,
		Param& sustainParam,
		Param& releaseParam,
		Param& holdParam,
		Param& attackShapeParam,
		Param& decayShapeParam,
		Param& releaseShapeParam,
		Param& triggerParam,
		Param& modeParam,
		Param& loopParam,
		Param& speedParam,
		Param& retriggerParam,

		Input* delayInput,
		Input* attackInput,
		Input* decayInput,
		Input* sustainInput,
		Input* releaseInput,
		Input* holdInput,
		Input& triggerInput,

		Output* delayOutput,
		Output* attackOutput,
		Output* decayOutput,
		Output* sustainOutput,
		Output* releaseOutput,
		Output& envOutput,
		Output& invOutput,
		Output& triggerOutput,

		float& delayLight,
		float& attackLight,
		float& decayLight,
		float& sustainLight,
		float& releaseLight,
		float& attackShape1Light,
		float& attackShape2Light,
		float& attackShape3Light,
		float& decayShape1Light,
		float& decayShape2Light,
		float& decayShape3Light,
		float& releaseShape1Light,
		float& releaseShape2Light,
		float& releaseShape3Light
	) : _delayParam(delayParam)
	, _attackParam(attackParam)
	, _decayParam(decayParam)
	, _sustainParam(sustainParam)
	, _releaseParam(releaseParam)
	, _holdParam(holdParam)
	, _attackShapeParam(attackShapeParam)
	, _decayShapeParam(decayShapeParam)
	, _releaseShapeParam(releaseShapeParam)
	, _triggerParam(triggerParam)
	, _modeParam(modeParam)
	, _loopParam(loopParam)
	, _speedParam(speedParam)
	, _retriggerParam(retriggerParam)

	, _delayInput(delayInput)
	, _attackInput(attackInput)
	, _decayInput(decayInput)
	, _sustainInput(sustainInput)
	, _releaseInput(releaseInput)
	, _holdInput(holdInput)
	, _triggerInput(triggerInput)

	, _delayOutput(delayOutput)
	, _attackOutput(attackOutput)
	, _decayOutput(decayOutput)
	, _sustainOutput(sustainOutput)
	, _releaseOutput(releaseOutput)
	, _envOutput(envOutput)
	, _invOutput(invOutput)
	, _triggerOutput(triggerOutput)

	, _delayLight(delayLight)
	, _attackLight(attackLight)
	, _decayLight(decayLight)
	, _sustainLight(sustainLight)
	, _releaseLight(releaseLight)
	, _attackShape1Light(attackShape1Light)
	, _attackShape2Light(attackShape2Light)
	, _attackShape3Light(attackShape3Light)
	, _decayShape1Light(decayShape1Light)
	, _decayShape2Light(decayShape2Light)
	, _decayShape3Light(decayShape3Light)
	, _releaseShape1Light(releaseShape1Light)
	, _releaseShape2Light(releaseShape2Light)
	, _releaseShape3Light(releaseShape3Light)
	{
		reset();
	}

	void reset();
	void step();

	float stepAmount(const Param& knob, const Input* cv, bool slow, bool allowZero = false);
	float knobTime(const Param& knob, const Input* cv, bool slow, bool allowZero = false);
	float knobAmount(const Param& knob, const Input* cv) const;
};
