#include "GraphSerializer.h"

namespace Serialization
{
	namespace Graph 
	{
		SerializationStatus AskSaveGraphToFile(GraphEditorView& graph) {
			nfdchar_t* outPath;
			nfdfilteritem_t filterItem[1] = { { "Texture Graph", "tgraph" } };
			nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, "graphs/", "");
			if (result == NFD_OKAY) {
				return SaveGraphToFile(graph, std::string(outPath));
			}
			else {
				if (result == NFD_CANCEL) {
					LOG_WARN("User canceled saving graph");
					return SerializationStatus::Failed;
				}
				else {
					LOG_ERROR("Error saving graph");
					return SerializationStatus::Failed;
				}
			}
		}

		SerializationStatus SaveGraphToFile(GraphEditorView& graph, std::string file) {
			auto& nodes = graph.nodes;
			std::vector<std::string> lines;

			Utility::Timer tmr;

			std::vector<int> nodesList;
			int n = 0;
			for (auto& node : nodes) {
				nodesList.push_back(n);
				n++;
			}

			std::string newData;
			SaveNodesToData(graph, nodesList, newData);
			lines = Utility::String::split(newData, '\n');

			std::ofstream graphFile;
			graphFile.open(file);
			for (auto line : lines) {
				graphFile << line << "\n";
			}
			graphFile.close();

			LOG_INFO("Successfully saved graph to '{0}' in {1}s", file, tmr.Elapsed());
			return SerializationStatus::Successful;
		}

		SerializationStatus SaveNodesToData(GraphEditorView& graph, std::vector<int> nodesList, std::string& lines) {
			auto& nodes = graph.nodes;

			Utility::Timer tmr;

			int n = 0;
			for (auto& node : nodes) {
				if (std::find(nodesList.begin(), nodesList.end(), n) == nodesList.end()) { n++; continue; }
				std::stringstream dataStream;
				dataStream << "NODE>" << node->nodeId << "|" << node->nodeClass << "|" << node->nodePos.x << "," << node->nodePos.y;
				lines += dataStream.str() + "\n";
				for (auto& pin : node->pins) {
					dataStream.str("");
					int newInNodeId = std::find(nodesList.begin(), nodesList.end(), pin.inNodeId) == nodesList.end() ? -1 : pin.inNodeId;
					dataStream << "PIN>" << pin.pinIndex << "|" << newInNodeId << "~" << (newInNodeId == -1 ? -1 :pin.inPinIndex) << "|";
					for (int i = 0; i < pin.outPinIndexes.size(); i++) {
						int newOutNodeId = std::find(nodesList.begin(), nodesList.end(), pin.outNodeIds[i]) == nodesList.end() ? -1 : pin.outNodeIds[i];
						dataStream << (i != 0 ? "," : "") << newOutNodeId << "~" << (newOutNodeId == -1 ? -1 :pin.outPinIndexes[i]);
					}
					dataStream << "|" << pin.displayName;
					lines += dataStream.str() + "\n";
				}
				for (auto& luavar : node->paramLuaVars)
				{
					dataStream.str("");
					dataStream << "PARAM>" << luavar.second << "|" << node->luaVarData[luavar.second].AsString();
					lines += dataStream.str() + "\n";
				}
				n++;
			}

			return SerializationStatus::Successful;
		}

		SerializationStatus AskLoadGraphFromFile(GraphEditorView& graph) {
			nfdchar_t* outPath;
			nfdfilteritem_t filterItem[1] = { { "Texture Graph", "tgraph" } };
			nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, "graphs/");
			if (result == NFD_OKAY) {
				return LoadGraphFromFile(graph, std::string(outPath));
			}
			else {
				if (result == NFD_CANCEL) {
					LOG_WARN("User canceled loading graph");
					return SerializationStatus::Failed;
				}
				else {
					LOG_ERROR("Error loading graph");
					return SerializationStatus::Failed;
				}
			}
		}

		SerializationStatus AppendNodesFromData(GraphEditorView& graph, std::string data) {
			std::vector<std::string> lines = Utility::String::split(data, '\n');

			Utility::Timer tmr;

			std::unordered_map<int, int> remappedIndices;
			std::vector<int> newNodes;

			GraphNode* currentNode = nullptr;
			for (auto& line : lines) {
				Utility::String::ltrim(line); // Trim Whitespace left
				Utility::String::rtrim(line); // Trim Whitespace right
				if (line == "") { continue; } // Ignore empty lines

				auto type = Utility::String::split(line, '>')[0];
				auto data = Utility::String::split(Utility::String::split(line, '>')[1], '|');
				if (type == "NODE") {
					// Push old node
					if (currentNode != nullptr) {
						if (currentNode->displayName != "") {
							currentNode->SetTextureSize(graph.texSize);
							graph.nodes.push_back(currentNode);
						}
					}
					// New node definition
					int id = std::stoi(data[0]);
					auto posData = Utility::String::split(data[2], ',');
					std::string nodeClass = data[1];
					if (LibraryManager::classToNode.find(nodeClass) == LibraryManager::classToNode.end()) { LOG_ERROR("Node Class '{0}' was not found in Library. It may have been deleted", nodeClass); }
					currentNode = new GraphNode(LibraryManager::compiledNodes[LibraryManager::classToNode[nodeClass]]);
					int newIndex = graph.nodes.size();
					remappedIndices.insert({ id, newIndex });
					newNodes.push_back(newIndex);
					currentNode->nodeId = newIndex;
					float posX = std::stof(posData[0]);
					float posY = std::stof(posData[1]);
					currentNode->nodePos = sf::Vector2f(posX, posY);
					continue;
				}
				if (type == "PIN") {
					// node pin definition
					int id = std::stoi(data[0]);
					std::string inDef = data[1];
					int inNodeId = std::stoi(Utility::String::split(inDef, '~')[0]);
					int inPinIndex = std::stoi(Utility::String::split(inDef, '~')[1]);
					std::vector<std::string> outDefs = Utility::String::split(data[2], ',');
					std::vector<int> outNodeIds;
					std::vector<int> outPinIndexes;
					for (auto def : outDefs) {
						if (def == "") { continue; }
						outNodeIds.push_back(std::stoi(Utility::String::split(def, '~')[0]));
						outPinIndexes.push_back(std::stoi(Utility::String::split(def, '~')[1]));
					}
					currentNode->pins[id].inNodeId = inNodeId;
					currentNode->pins[id].inPinIndex = inPinIndex;
					currentNode->pins[id].outNodeIds = outNodeIds;
					currentNode->pins[id].outPinIndexes = outPinIndexes;
				}
				if (type == "PARAM") {
					// param definition
					std::string var = data[0];
					std::string val = data[1];
					currentNode->luaVarData[var].FromString(val);
				}
			}

			currentNode->SetTextureSize(graph.texSize);

			// Push final node
			graph.nodes.push_back(currentNode);

			for (auto newNodeIndex : newNodes)
			{
				for (auto& pin : graph.nodes[newNodeIndex]->pins) {
					if (pin.inNodeId != -1) {
						pin.inNodeId = remappedIndices[pin.inNodeId];
					}
					int o = 0;
					for (int outIndex : pin.outNodeIds) {
						if (outIndex == -1) { continue; }
						pin.outNodeIds[o] = remappedIndices[outIndex];
						o++;
					}
				}
			}

			LOG_INFO("Successfully loaded nodes from data in {0}s", tmr.Elapsed());
			return SerializationStatus::Successful;
		}

		SerializationStatus LoadGraphFromFile(GraphEditorView& graph, std::string file) {
			if (!std::filesystem::exists(file)) { LOG_CRITICAL("Graph File '{0}' could not be found.", file); }
			std::ifstream f(file);
			graph.Clear();

			Utility::Timer tmr;

			std::string lines;
			std::string line;
			while (std::getline(f, line)) {
				lines += line + "\n";
			}
			SerializationStatus status = AppendNodesFromData(graph, lines);

			LOG_INFO("Successfully loaded graph from '{0}' in {1}s", file, tmr.Elapsed());
			if (graph.CheckCyclical()) {
				LOG_ERROR("Loaded graph contains cyclical dependancy, the file may have corrupted.");
			}
			return status;
		}
	}
}
