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

			for (auto& node : nodes) {
				std::stringstream dataStream;
				dataStream << "NODE>" << node->nodeId << "|" << node->nodeClass << "|" << node->nodePos.x << "," << node->nodePos.y;
				lines.push_back(dataStream.str());
				for (auto& pin : node->pins) {
					dataStream.str("");
					dataStream << "PIN>" << pin.pinIndex << "|" << pin.inNodeId << "~" << pin.inPinIndex << "|";
					for (int i = 0; i < pin.outPinIndexes.size(); i++) {
						dataStream << (i != 0 ? "," : "") << pin.outNodeIds[i] << "~" << pin.outPinIndexes[i];
					}
					dataStream << "|" << pin.displayName;
					lines.push_back(dataStream.str());
				}
				for (auto& luavar : node->paramLuaVars)
				{
					dataStream.str("");
					dataStream << "PARAM>" << luavar.second << "|" << node->luaVarData[luavar.second].AsString();
					lines.push_back(dataStream.str());
				}
			}

			std::ofstream graphFile;
			graphFile.open(file);
			for (auto line : lines) {
				graphFile << line << "\n";
			}
			graphFile.close();

			LOG_INFO("Successfully saved graph to '{0}' in {1}s", file, tmr.Elapsed());
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

		SerializationStatus LoadGraphFromFile(GraphEditorView& graph, std::string file) {
			if (!std::filesystem::exists(file)) { LOG_CRITICAL("Graph File '{0}' could not be found.", file); }
			std::ifstream f(file);
			graph.Clear();

			Utility::Timer tmr;

			GraphNode* currentNode = nullptr;

			std::string line;
			while (std::getline(f, line)) {
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
					currentNode->nodeId = id;
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

			LOG_INFO("Successfully loaded graph from '{0}' in {1}s", file, tmr.Elapsed());
			return SerializationStatus::Successful;
		}
	}
}
