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

template<class MSG, class EM, class BASE>
struct ExpandableModule : BASE {
	MSG _messages[2] {};
	bool _wasConnected = false;

	ExpandableModule() {
		static_assert(std::is_base_of<ExpanderMessage, MSG>::value, "type parameter MSG must derive from ExpanderMessage");
		static_assert(std::is_base_of<BGModule, BASE>::value, "type parameter BASE must derive from BGModule");

		BGModule::rightExpander.producerMessage = &_messages[0];
		BGModule::rightExpander.consumerMessage = &_messages[1];
	}

	bool expanderConnected() {
		bool connected = BGModule::rightExpander.module && dynamic_cast<EM*>(BGModule::rightExpander.module);
		if (!connected && _wasConnected) {
			_messages[1] = _messages[0] = MSG();
		}
		return _wasConnected = connected;
	}

	MSG* toExpander() {
		assert(expanderConnected());
		MSG* m = (MSG*)BGModule::rightExpander.module->leftExpander.producerMessage;
		assert(m);
		m->channels = BGModule::_channels;
		return m;
	}

	MSG* fromExpander() {
		assert(expanderConnected());
		MSG* m = (MSG*)BGModule::rightExpander.consumerMessage;
		assert(m);
		return m;
	}

	void process(const BGModule::ProcessArgs& args) override {
		BASE::process(args);
		if (BGModule::rightExpander.module) {
			BGModule::rightExpander.module->leftExpander.messageFlipRequested = true;
		}
	}
};

// An expander must be to the right of the expanded module to work.
template<class MSG, class BM, class BASE>
struct ExpanderModule : BASE {
	MSG _messages[2] {};
	bool _wasConnected = false;

	ExpanderModule() {
		static_assert(std::is_base_of<ExpanderMessage, MSG>::value, "type parameter MSG must derive from ExpanderMessage");
		static_assert(std::is_base_of<BGModule, BASE>::value, "type parameter BASE must derive from BGModule");

		BGModule::leftExpander.producerMessage = &_messages[0];
		BGModule::leftExpander.consumerMessage = &_messages[1];
	}

	bool baseConnected() {
		bool connected = BGModule::leftExpander.module && dynamic_cast<BM*>(BGModule::leftExpander.module);
		if (!connected && _wasConnected) {
			_messages[1] = _messages[0] = MSG();
		}
		return _wasConnected = connected;
	}

	MSG* fromBase() {
		assert(baseConnected());
		MSG* m = (MSG*)BGModule::leftExpander.consumerMessage;
		assert(m);
		return m;
	}

	MSG* toBase() {
		assert(baseConnected());
		MSG* m = (MSG*)BGModule::leftExpander.module->rightExpander.producerMessage;
		assert(m);
		return m;
	}

	int channels() override final {
		if (baseConnected()) {
			return fromBase()->channels;
		}
		return 1;
	}

	void process(const BGModule::ProcessArgs& args) override {
		BASE::process(args);
		if (BGModule::leftExpander.module) {
			BGModule::leftExpander.module->rightExpander.messageFlipRequested = true;
		}
	}
};

} // namespace bogaudio
