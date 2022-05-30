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
		SerializationStatus AskSaveGraphToFile(GraphEditorView& graph);
		SerializationStatus SaveGraphToFile(GraphEditorView& graph, std::string file);

		SerializationStatus AskLoadGraphFromFile(GraphEditorView& graph);
		SerializationStatus LoadGraphFromFile(GraphEditorView& graph, std::string file);
	}
}

