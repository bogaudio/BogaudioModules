
#include "Pgmr_shared.hpp"
#include "Pgmr.hpp"
#include "PgmrX.hpp"

void PgmrStep::reset() {
	for (int c = 0; c < BGModule::maxChannels; ++c) {
		triggers[c].reset();
		pulseGens[c].process(1000.0f);
	}
}


PgmrRegistry::Base::Base(Pgmr& b) : module(b) {
	std::copy(b._localSteps, b._localSteps + 4, std::back_inserter(steps));
}

int PgmrRegistry::registerBase(Pgmr& b) {
	std::lock_guard<std::mutex> lock(_lock);

	int id = _nextID;
	++_nextID;
	auto p = _bases.emplace(id, Base(b));
	b.setSteps(p.first->second.steps);
	return id;
}

void PgmrRegistry::deregisterBase(int id) {
	std::lock_guard<std::mutex> lock(_lock);
	_bases.erase(id);
}

void PgmrRegistry::registerExpander(int baseID, int position, PgmrX& x) {
	std::lock_guard<std::mutex> lock(_lock);

	assert(position > 0);
	auto base = _bases.find(baseID);
	if (base != _bases.end()) {
		int i = 4 * position;
		if (i < (int)base->second.steps.size()) {
			assert(!base->second.steps[i]);
			std::copy(x._localSteps, x._localSteps + 4, base->second.steps.begin() + i);
		}
		else {
			base->second.steps.resize(i + 4, NULL);
			std::copy(x._localSteps, x._localSteps + 4, base->second.steps.begin() + i);
		}
		for (auto i = base->second.steps.begin(), n = base->second.steps.end(); i != n; ++i) {
			if (!*i) {
				return;
			}
		}
		base->second.module.setSteps(base->second.steps);
	}
}

void PgmrRegistry::deregisterExpander(int baseID, int position) {
	std::lock_guard<std::mutex> lock(_lock);

	auto base = _bases.find(baseID);
	if (base != _bases.end()) {
		int n = 4 * position;
		if (n < (int)base->second.steps.size()) {
			int i = 0;
			for (; i < n; ++i) {
				if (!base->second.steps[i]) {
					break;
				}
			}
			base->second.steps.resize(i);
			base->second.module.setSteps(base->second.steps);
		}
	}
}

PgmrRegistry& PgmrRegistry::registry() {
	static PgmrRegistry instance;
	return instance;
}
