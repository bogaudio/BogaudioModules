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
	bool* soloByColumns;

	Matrix44Element(
		Param** mutes,
		Input** cvs,
		bool* soloByColumns
	)
	: mutes(mutes)
	, cvs(cvs)
	, soloByColumns(soloByColumns)
	{}
};

} // namespace bogaudio
