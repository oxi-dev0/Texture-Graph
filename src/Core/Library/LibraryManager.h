#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <functional>

#include "../Graph/Node/GraphNode.h"
#include "../Utils/Log.h"
#include "../Utils/Timer.h"

namespace LibraryManager {
	extern std::vector<GraphNode> compiledNodes;
	extern std::function<void(void)> LoadedCallback;

	template <typename Callable>
	inline void RegisterLoadCallback(Callable callback) {
		LoadedCallback = callback;
	};

	extern void LoadNodeLibrary();
}
