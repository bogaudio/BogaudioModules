
#include "Pgmr_shared.hpp"

void PgmrStep::reset() {
	for (int c = 0; c < BGModule::maxChannels; ++c) {
		triggers[c].reset();
		pulseGens[c].process(1000.0f);
	}
}
