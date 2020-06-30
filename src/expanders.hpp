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

template<class MSG, class BASE>
struct ExpandableModule : BASE {
	std::function<bool(Model*)> _expanderModel;
	MSG _messages[2] {};
	bool _wasConnected = false;

	ExpandableModule() {
		static_assert(std::is_base_of<ExpanderMessage, MSG>::value, "type parameter MSG must derive from ExpanderMessage");
		static_assert(std::is_base_of<BGModule, BASE>::value, "type parameter BASE must derive from BGModule");

		BGModule::rightExpander.producerMessage = &_messages[0];
		BGModule::rightExpander.consumerMessage = &_messages[1];
	}

	void setExpanderModelPredicate(std::function<bool(Model*)> p) {
		_expanderModel = p;
	}

	bool expanderConnected() {
		bool connected = BGModule::rightExpander.module && _expanderModel && _expanderModel(BGModule::rightExpander.module->model);
		if (!connected && _wasConnected) {
			_messages[1] = _messages[0] = MSG();
		}
		return _wasConnected = connected;
	}

	inline MSG* toExpander() {
		return (MSG*)BGModule::rightExpander.module->leftExpander.producerMessage;
	}

	inline MSG* fromExpander() {
		return (MSG*)BGModule::rightExpander.consumerMessage;
	}

	void process(const BGModule::ProcessArgs& args) override {
		BASE::process(args);
		if (BGModule::rightExpander.module) {
			BGModule::rightExpander.module->leftExpander.messageFlipRequested = true;
		}
	}
};

// An expander must be to the right of the expanded module to work.
template<class MSG, class BASE>
struct ExpanderModule : BASE {
	std::function<bool(Model*)>  _baseModel;
	MSG _messages[2] {};
	bool _wasConnected = false;

	ExpanderModule() {
		static_assert(std::is_base_of<ExpanderMessage, MSG>::value, "type parameter MSG must derive from ExpanderMessage");
		static_assert(std::is_base_of<BGModule, BASE>::value, "type parameter BASE must derive from BGModule");

		BGModule::leftExpander.producerMessage = &_messages[0];
		BGModule::leftExpander.consumerMessage = &_messages[1];
	}

	void setBaseModelPredicate(std::function<bool(Model*)> p) {
		_baseModel = p;
	}

	bool baseConnected() {
		bool connected = BGModule::leftExpander.module && _baseModel && _baseModel(BGModule::leftExpander.module->model);
		if (!connected && _wasConnected) {
			_messages[1] = _messages[0] = MSG();
		}
		return _wasConnected = connected;
	}

	inline MSG* fromBase() {
		return (MSG*)BGModule::leftExpander.consumerMessage;
	}

	inline MSG* toBase() {
		return (MSG*)BGModule::leftExpander.module->rightExpander.producerMessage;
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
