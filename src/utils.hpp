#pragma once

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

} // namespace bogaudio
