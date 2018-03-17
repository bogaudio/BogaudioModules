#include <math.h>

#include "table.hpp"

using namespace bogaudio::dsp;

void Table::generate() {
	if (!_table) {
		_table = new float[_length] {};
		_generate();
	}
}


void SineTable::_generate() {
	const float twoPI = 2.0f * M_PI;
	for (int i = 0, j = _length / 4; i <= j; ++i) {
		_table[i] = sinf(twoPI * (i / (float)_length));
	}
	for (int i = 1, j = _length / 4; i < j; ++i) {
		_table[i + j] = _table[j - i];
	}
	for (int i = 0, j = _length / 2; i < j; ++i) {
		_table[i + j] = -_table[i];
	}
}
