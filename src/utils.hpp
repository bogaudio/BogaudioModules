#pragma once

// #include <string.h>
// #include <algorithm>

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
	// FIXME.v1
	// const int n = 256;
	// char buf[n];
	// std::string uName = name;
	// for (auto& c: uName) {
	// 	c = toupper(c);
	// }
	// snprintf(buf, n, "%s - %s - %dHP", uName.c_str(), description, TModuleWidget::hp);
	return rack::createModel<TModule, TModuleWidget>(slug);
}

} // namespace bogaudio
