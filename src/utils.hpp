#pragma once

#include <atomic>

#include "rack.hpp"

using namespace rack;

namespace bogaudio {

template <typename TModule, typename TModuleWidget, typename... Tags>
Model* createModel(
	const char* slug,
	const char* name,
	const char* description,
	Tags... tags
) {
	// params besides slug are obsolete here, but see scripts/modules_to_plugin_json.rb
	return rack::createModel<TModule, TModuleWidget>(slug);
}

// https://stackoverflow.com/questions/26583433/c11-implementation-of-spinlock-using-atomic
struct SpinLock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT ;

    inline void lock() {
        while (locked.test_and_set(std::memory_order_acquire)) {}
    }

    inline void unlock() {
        locked.clear(std::memory_order_release);
    }
};

} // namespace bogaudio
