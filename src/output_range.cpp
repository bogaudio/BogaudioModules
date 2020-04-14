
#include "output_range.hpp"

using namespace bogaudio;

float OutputRange::OutputRangeParamQuantity::getDisplayValue() {
	float v = getValue();
	if (!module) {
		return v;
	}

	auto m = dynamic_cast<OutputRange*>(module);
	v += m->_rangeOffset;
	v *= m->_rangeScale;
	return v;
}

void OutputRange::OutputRangeParamQuantity::setDisplayValue(float v) {
	if (!module) {
		return;
	}

	auto m = dynamic_cast<OutputRange*>(module);
	v /= m->_rangeScale;
	v -= m->_rangeOffset;
	setValue(v);
}
