#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"
#include "expanders.hpp"

extern Model* modelMatrix88;
extern Model* modelMatrix88Cv;
extern Model* modelMatrix88M;

namespace bogaudio {

typedef ChainableExpanderMessage Matrix88ExpanderMessage;

struct Matrix88Element {
	Param** mutes;
	Input** cvs;

	Matrix88Element(
		Param** mutes,
		Input** cvs
	)
	: mutes(mutes)
	, cvs(cvs)
	{}
};

typedef ChainableRegistry<Matrix88Element, 1> Matrix88Registry;

} // namespace bogaudio
