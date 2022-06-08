#pragma once

#include "../Rendering/Views/GraphEditorView.h"
#include "../Utils/Timer.h"
#include "Node/GraphNode.h"
#include "../Library/LibraryManager.h"

#include "imgui.h"

#include <nfd.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Serialization
{
	enum SerializationStatus {
		Failed,
		Successful
	};

	namespace Graph
	{	
		SerializationStatus SaveNodesToData(GraphEditorView& graph, std::vector<int> nodesList, std::string& lines);

		SerializationStatus AskSaveGraphToFile(GraphEditorView& graph);
		SerializationStatus SaveGraphToFile(GraphEditorView& graph, std::string file);

		SerializationStatus AskLoadGraphFromFile(GraphEditorView& graph);
		SerializationStatus LoadGraphFromFile(GraphEditorView& graph, std::string file);

		SerializationStatus AppendNodesFromData(GraphEditorView& graph, std::string data);
	}
}

