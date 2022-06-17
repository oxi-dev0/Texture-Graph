#include "LibraryManager.h"

namespace LibraryManager {
	std::vector<GraphNode*> compiledNodes;
	std::map<std::string, int> classToNode;
	std::function<void(void)> LoadedCallback;

	void LoadNodeLibrary(bool justRedraw) {
		if(justRedraw) {
			if (LoadedCallback) {
				LoadedCallback();
				return;
			}
		}
		  
		compiledNodes.clear();
		 
		if (!std::filesystem::is_directory("temp") || !std::filesystem::exists("temp")) {
			std::filesystem::create_directory("temp");
		}
		else {
			std::filesystem::remove_all("temp");
			std::filesystem::create_directory("temp");
		}

		LOG_INFO("Scanning library folder for node definitions...");
		std::vector<std::string> foundNodes;
		for (const auto& p : std::filesystem::recursive_directory_iterator("library/Nodes/"))
		{
			if (p.path().extension() == ".tgnode")
			{
				auto path = p.path().string(); 
				foundNodes.push_back(Utility::String::replace(path, '\\', '/'));
			}
		}
		LOG_TRACE("Found {0} definitions in library folder", foundNodes.size());
		LOG_INFO("Compiling node library...");

		Utility::Timer tmr;
		for (auto& nodeFile : foundNodes) {
			compiledNodes.push_back(GraphNode::LoadFromTGNF(nodeFile));
			auto& newNode = compiledNodes[compiledNodes.size() - 1];
			classToNode.insert({ newNode->nodeClass, (int)compiledNodes.size() - 1 });
			LOG_TRACE("Successfully compiled Node Class '{0}'", nodeFile);
		}
		LOG_INFO("Finished compiling Node library in {0}s", tmr.Elapsed());

		compiledNodes.push_back(new ImageNode());

		if (LoadedCallback) {
			LoadedCallback();
		}
	}
}
