#pragma once

#include <type_traits>

#include "rack.hpp"
#include "module.hpp"

using namespace rack;

namespace bogaudio {

struct ExpanderMessage {
	int channels = 0;

	virtual ~ExpanderMessage() {}
};

template<class MSG, class EM>
struct ExpandableModule : BGModule {
	MSG _messages[2] {};

	ExpandableModule() {
		static_assert(std::is_base_of<ExpanderMessage, MSG>::value, "type parameter MSG must derive from ExpanderMessage");

		rightExpander.producerMessage = &_messages[0];
		rightExpander.consumerMessage = &_messages[1];
	}

	inline bool connected() {
		return rightExpander.module && dynamic_cast<EM*>(rightExpander.module);
	}

	inline MSG* toExpander() {
		assert(connected());
		MSG* m = (MSG*)rightExpander.module->leftExpander.producerMessage;
		assert(m);
		m->channels = _channels;
		return m;
	}

	inline MSG* fromExpander() {
		assert(connected());
		MSG* m = (MSG*)rightExpander.consumerMessage;
		assert(m);
		return m;
	}

	void process(const ProcessArgs& args) override {
		BGModule::process(args);
		if (rightExpander.module) {
			rightExpander.module->leftExpander.messageFlipRequested = true;
		}
	}
};

// An expander must be to the right of the expanded module to work.
template<class MSG, class BM>
struct ExpanderModule : BGModule {
	MSG _messages[2] {};

	ExpanderModule() {
		static_assert(std::is_base_of<ExpanderMessage, MSG>::value, "type parameter MSG must derive from ExpanderMessage");

		leftExpander.producerMessage = &_messages[0];
		leftExpander.consumerMessage = &_messages[1];
	}

	inline bool connected() {
		return leftExpander.module && dynamic_cast<BM*>(leftExpander.module);
	}

	inline MSG* fromBase() {
		assert(connected());
		MSG* m = (MSG*)leftExpander.consumerMessage;
		assert(m);
		return m;
	}

	inline MSG* toBase() {
		assert(connected());
		MSG* m = (MSG*)leftExpander.module->rightExpander.producerMessage;
		assert(m);
		return m;
	}

	int channels() override final {
		if (connected()) {
			return fromBase()->channels;
		}
		return 1;
	}

	void process(const ProcessArgs& args) override {
		BGModule::process(args);
		if (leftExpander.module) {
			leftExpander.module->rightExpander.messageFlipRequested = true;
		}
	}
};

} // namespace bogaudio
