#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"
#include "expanders.hpp"

extern Model* modelMatrix44;
extern Model* modelMatrix44Cvm;

namespace bogaudio {

typedef ChainableExpanderMessage Matrix44ExpanderMessage;

struct Matrix44Element {
	Param** mutes;
	Input** cvs;

	Matrix44Element(
		Param** mutes,
		Input** cvs
	)
	: mutes(mutes)
	, cvs(cvs)
	{}
};

typedef ChainableRegistry<Matrix44Element, 1> Matrix44Registry;

} // namespace bogaudio
