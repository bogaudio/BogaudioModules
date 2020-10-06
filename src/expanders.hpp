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

template<class E, int N>
struct ChainableRegistry {
public:
	struct Chainable {
		E* _localElements[N] {};

		virtual ~Chainable() {
			for (int i = 0; i < N; ++i) {
				if (_localElements[i]) {
					delete _localElements[i];
				}
			}
		}

		void setLocalElements(std::vector<E*> es) {
			assert(es.size() == N);
			for (int i = 0; i < N; ++i) {
				_localElements[i] = es[i];
			}
		}
	};

	struct ChainableBase : Chainable {
		SpinLock _elementsLock;
		std::vector<E*> _elements;

		void setElements(const std::vector<E*>& elements) {
			std::lock_guard<SpinLock> lock(_elementsLock);
			_elements = elements;
			elementsChanged();
		}

		virtual void elementsChanged() {}
	};

	typedef Chainable ChainableExpander;

private:
	struct Base {
		ChainableBase& module;
		std::vector<E*> elements;

		Base(ChainableBase& b) : module(b) {
			std::copy(b._localElements, b._localElements + N, std::back_inserter(elements));
		}
	};

	std::mutex _lock;
	int _nextID = 1;
	std::unordered_map<int, Base> _bases;

public:
	int registerBase(ChainableBase& b) {
		std::lock_guard<std::mutex> lock(_lock);

		int id = _nextID;
		++_nextID;
		auto p = _bases.emplace(id, Base(b));
		b.setElements(p.first->second.elements);
		return id;
	}

	void deregisterBase(int id) {
		std::lock_guard<std::mutex> lock(_lock);
		_bases.erase(id);
	}

	void registerExpander(int baseID, int position, ChainableExpander& x) {
		std::lock_guard<std::mutex> lock(_lock);

		assert(position > 0);
		auto base = _bases.find(baseID);
		if (base != _bases.end()) {
			int i = N * position;
			if (i < (int)base->second.elements.size()) {
				assert(!base->second.elements[i]);
				std::copy(x._localElements, x._localElements + N, base->second.elements.begin() + i);
			}
			else {
				base->second.elements.resize(i + N, NULL);
				std::copy(x._localElements, x._localElements + N, base->second.elements.begin() + i);
			}
			for (auto i = base->second.elements.begin(), n = base->second.elements.end(); i != n; ++i) {
				if (!*i) {
					return;
				}
			}
			base->second.module.setElements(base->second.elements);
		}
	}

	void deregisterExpander(int baseID, int position) {
		std::lock_guard<std::mutex> lock(_lock);

		auto base = _bases.find(baseID);
		if (base != _bases.end()) {
			int n = N * position;
			if (n < (int)base->second.elements.size()) {
				int i = 0;
				for (; i < n; ++i) {
					if (!base->second.elements[i]) {
						break;
					}
				}
				base->second.elements.resize(i);
				base->second.module.setElements(base->second.elements);
			}
		}
	}

	static ChainableRegistry& registry() {
		static ChainableRegistry<E, N> instance;
		return instance;
	}
};

struct ChainableExpanderMessage : ExpanderMessage {
	int baseID = -1;
	int position = -1;
};

template<class MESSAGE, class ELEMENT, int N, class BASE>
struct ChainableExpandableModule
: ExpandableModule<MESSAGE, BASE>
, ChainableRegistry<ELEMENT, N>::ChainableBase
{
	ChainableRegistry<ELEMENT, N>& _registry;
	int _id = -1;

	ChainableExpandableModule()
	: _registry(ChainableRegistry<ELEMENT, N>::registry())
	{}
	virtual ~ChainableExpandableModule() {
		_registry.deregisterBase(_id);
	}

	void registerBase() {
		_id = _registry.registerBase(*this);
	}
};

template<class MESSAGE, class ELEMENT, int N, class BASE>
struct ChainableExpanderModule
: ExpanderModule<MESSAGE, ExpandableModule<MESSAGE, BASE>>
, ChainableRegistry<ELEMENT, N>::ChainableExpander
{
	ChainableRegistry<ELEMENT, N>& _registry;
	bool _registered = false;
	int _baseID = -1;
	int _position = -1;

	ChainableExpanderModule()
	: _registry(ChainableRegistry<ELEMENT, N>::registry())
	{}
	virtual ~ChainableExpanderModule() {
		_registry.deregisterExpander(_baseID, _position);
	}

	void setBaseIDAndPosition(int baseID, int position) {
		if (_registered && (position <= 0 || position != _position)) {
			_registry.deregisterExpander(_baseID, _position);
			_registered = false;
			_baseID = 0;
			_position = 0;
		}
		else if (!_registered && position > 0) {
			_registered = true;
			_baseID = baseID;
			_position = position;
			_registry.registerExpander(_baseID, _position, *this);
		}
	}
};

} // namespace bogaudio
