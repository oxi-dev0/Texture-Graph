#pragma once

#include "../Rendering/Views/GraphEditorView.h"
#include "../Utils/Timer.h"
#include "Node/GraphNode.h"
#include "../Library/LibraryManager.h"
#include "ResourceManager.h"

#include "imgui.h"

#include <nfd.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Graph
{
	enum SerializationStatus {
		Failed,
		Successful
	};

	namespace Serialization
	{	
		extern std::string currentGraph;
		extern std::function<void(void)> clearPromptCallback;
		extern std::function<void(std::string)> openPopup;

		SerializationStatus SaveNodesToData(GraphEditorView& graph, std::vector<int> nodesList, std::string& lines);

		void NewGraph(GraphEditorView& graph, std::string name);
		void GraphNewPopup(GraphEditorView& graph, char* nameBuf);

		void SafeClear(std::function<void(void)> callback);
		void ClearPromptPopup(GraphEditorView& graph);

		std::vector<std::string> GetGraphsInBundle();

		SerializationStatus SaveGraphToFile(GraphEditorView& graph, std::string file);
		SerializationStatus LoadGraphFromFile(GraphEditorView& graph, std::string file);

		SerializationStatus AppendNodesFromData(GraphEditorView& graph, std::string data);
	}
}

