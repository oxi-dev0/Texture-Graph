#include "LibraryManager.h"

namespace LibraryManager {
	std::vector<GraphNode> compiledNodes;
	std::function<void(void)> LoadedCallback;

	void LoadNodeLibrary() {
		compiledNodes.clear();
		LOG_INFO("Scanning library folder for node definitions...");
		std::vector<std::string> foundNodes;
		for (const auto& p : std::filesystem::directory_iterator("library/Nodes/"))
		{
			if (p.path().extension() == ".tgnode")
			{
				foundNodes.push_back(p.path().string());
			}
		}
		LOG_TRACE("Found {0} definitions in library folder", foundNodes.size());
		LOG_INFO("Compiling node library...");

		Utility::Timer tmr;
		for (auto& nodeFile : foundNodes) {
			compiledNodes.push_back(GraphNode::LoadFromTGNF(nodeFile));
			LOG_TRACE("Successfully compiled Node Class '{0}'", nodeFile);
		}
		LOG_INFO("Finished compiling Node library in {0}s", tmr.Elapsed());

		if (LoadedCallback) {
			LoadedCallback();
		}
	}
}
