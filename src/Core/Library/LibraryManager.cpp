#include "LibraryManager.h"

namespace LibraryManager {
	std::vector<GraphNode*> compiledNodes;
	std::map<std::string, int> classToNode;
	std::map<std::string, std::map<std::string, ConditionParser::ExpressionBase*>> compiledNodeVarShowExpressions;
	std::function<void(void)> LoadedCallback;

	void Clear() {
		for (auto* node : compiledNodes) {
			if (node == nullptr) { continue; }
			delete node;
		}

		for (std::map<std::string, std::map<std::string, ConditionParser::ExpressionBase*>>::iterator it = compiledNodeVarShowExpressions.begin(); it != compiledNodeVarShowExpressions.end(); ++it) {
			for (std::map<std::string, ConditionParser::ExpressionBase*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
				delete it2->second;
			}
		}
		compiledNodeVarShowExpressions = std::map<std::string, std::map<std::string, ConditionParser::ExpressionBase*>>();
	}

	bool RegisterShowExpression(const std::string& nodeClass, const std::string& varName, ConditionParser::ExpressionBase* expr) {
		if (compiledNodeVarShowExpressions.find(nodeClass) == compiledNodeVarShowExpressions.end()) {
			compiledNodeVarShowExpressions.insert({ nodeClass, std::map<std::string, ConditionParser::ExpressionBase*>() });
		}
		auto& nodeMap = compiledNodeVarShowExpressions.find(nodeClass)->second;
		if (nodeMap.find(varName) != nodeMap.end()) {
			return false;
		}
		else {
			nodeMap.insert({ varName, expr });
			return true;
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
		std::vector<std::string> foundNodeFolders;
		for (const auto& p : std::filesystem::recursive_directory_iterator("library/Nodes/"))
		{
			auto path = p.path().parent_path().string();
			path = Utility::String::replace(path, '\\', '/');
			if (!std::filesystem::exists(path + "/.metadata")) { continue; }
			if (std::find(foundNodeFolders.begin(), foundNodeFolders.end(), path) != foundNodeFolders.end()) { continue; }
			foundNodeFolders.push_back(path);
		}
		LOG_TRACE("Found {0} definitions in library folder", foundNodeFolders.size());
		LOG_INFO("Compiling node library...");

		Utility::Timer tmr; 
		for (auto& nodeFolder : foundNodeFolders) {
			compiledNodes.push_back(GraphNode::LoadFromTGNF(nodeFolder, RegisterShowExpression));
			auto& newNode = compiledNodes[compiledNodes.size() - 1];
			classToNode.insert({ newNode->nodeClass, (int)compiledNodes.size() - 1 });
			LOG_TRACE("Successfully compiled Node Class '{0}'", nodeFolder);
		}
		LOG_INFO("Finished compiling Node library in {0}s", tmr.Elapsed());

		compiledNodes.push_back(new ImageNode());
		 
		if (LoadedCallback) {
			LoadedCallback();
		}
	}
}
