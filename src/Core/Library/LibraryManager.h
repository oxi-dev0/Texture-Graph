#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <thread>

#include "../Bundle/Node/GraphNode.h"
#include "../Bundle/Node/ImageNode.h"
#include "../Utils/Log.h"
#include "../Utils/Timer.h"
#include "../Utils/Strings.h"

namespace LibraryManager {
	extern std::vector<GraphNode*> compiledNodes;
	extern std::map<std::string, int> classToNode;
	extern std::function<void(void)> LoadedCallback;

	template <typename Callable>
	inline void RegisterLoadCallback(Callable callback) {
		LoadedCallback = callback;
	};

	void Clear();

	extern void LoadNodeLibrary(bool justRedraw = false);
}
