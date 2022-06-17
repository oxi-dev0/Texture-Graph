#include "LibraryManager.h"

namespace LibraryManager {
	std::vector<GraphNode*> compiledNodes;
	std::map<std::string, int> classToNode;
	std::function<void(void)> LoadedCallback;

	void Clear() {
		for (auto* node : compiledNodes) {
			if (node == nullptr) { continue; }
			delete node;
		}
	}

	void LoadNodeLibrary(bool justRedraw) {
		Clear();
		if(justRedraw) {
			if (LoadedCallback) {
				LoadedCallback();
				return;
			}
		}
		  
		compiledNodes.clear();
		 
		if (!std::filesystem::is_directory("temp/node-exec") || !std::filesystem::exists("temp/node-exec")) {
			std::filesystem::create_directory("temp/node-exec");
		}
		else {
			std::filesystem::remove_all("temp/node-exec");
			std::filesystem::create_directory("temp/node-exec");
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
