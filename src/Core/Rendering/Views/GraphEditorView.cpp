#include "GraphEditorView.h"

GraphEditorView::GraphEditorView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_)
	: SFMLWindow(main_, rt_, name_, flags_)
{
	ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	float mul = 0.6f;
	SetBGColor(ImVec4(bgColor.x * mul, bgColor.y * mul, bgColor.z * mul, 1));
	initialRect = sf::FloatRect(view.getCenter() - view.getSize() / 2.f, view.getSize());
	selectedNode = -1;
	texSize = sf::Vector2i(512, 512);
	dirty = false;
} // JUST CALLS CONSTRUCTOR FOR SUBWINDOW

void GraphEditorView::Clear() {
	for (auto* node : nodes) {
		if (node == nullptr) { continue; }
		delete node;
	}
	nodes.clear();
	dirty = false;
	zoom = 1.0f;
	view.reset(initialRect);
	vcenter = sf::Vector2f(0, 0);
}

void GraphEditorView::Grid() {
	if (prevSize.x == 0) { return; }

	int lineSpacing = 40;
	ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	float mul = 0.4f;
	ImVec4 col = ImVec4(bgColor.x * mul, bgColor.y * mul, bgColor.z * mul, 1);
	sf::Color lineColor = sf::Color((sf::Uint8)(col.x * 255), (sf::Uint8)(col.y * 255), (sf::Uint8)(col.z * 255), (sf::Uint8)(col.w * 255));

	int xSpace = (int)std::floor((float)rt.mapCoordsToPixel(sf::Vector2f((float)lineSpacing, 0)).x - (float)rt.mapCoordsToPixel(sf::Vector2f(0, 0)).x);
	int ySpace = (int)std::floor((float)rt.mapCoordsToPixel(sf::Vector2f(0, (float)lineSpacing)).y - (float)rt.mapCoordsToPixel(sf::Vector2f(0, 0)).y);

	sf::Vector2f offset = sf::Vector2f((float)((int)(vcenter.x * prevSize.x * (1 / zoom)) % xSpace), (float)((int)(vcenter.y * prevSize.y * (1 / zoom)) % ySpace)); // -sf::Vector2f(lineSpacingX * std::floor(((vcenter.x * prevSize.x) - (prevSize.x / 2)) / sf::Vector2f(rt.mapCoordsToPixel(sf::Vector2f(lineSpacingX, 0)) - rt.mapCoordsToPixel(sf::Vector2f(0, 0))).length()), lineSpacingY * std::floor(((vcenter.y * prevSize.y) - (prevSize.y / 2)) / sf::Vector2f(rt.mapCoordsToPixel(sf::Vector2f(0, lineSpacingY)) - rt.mapCoordsToPixel(sf::Vector2f(0, 0))).length()));

	int totalLinesX = (int)prevSize.x / xSpace;
	int totalLinesY = (int)prevSize.y / ySpace;
	lines = sf::VertexArray(sf::Lines, 2 + ((totalLinesX)+(totalLinesY)) * 2);

	for (int x = 0; x <= totalLinesX; x += 1) {
		int i1 = x * 2;
		int i2 = (x * 2) + 1;

		// pixel space
		float xPos = ((float)rt.mapCoordsToPixel(sf::Vector2f((float)lineSpacing, 0)).x - (float)rt.mapCoordsToPixel(sf::Vector2f(0, 0)).x) * (float)x;

		lines[i1].position = sf::Vector2f(rt.mapPixelToCoords(sf::Vector2i(sf::Vector2f(xPos, 0.0f - (float)lineSpacing*2.0f) - offset)));
		lines[i2].position = sf::Vector2f(rt.mapPixelToCoords(sf::Vector2i(sf::Vector2f(xPos, prevSize.y + (float)lineSpacing) - offset)));
		lines[i1].color = lineColor;
		lines[i2].color = lineColor;
	}

	for (int y = 0; y <= totalLinesY; y += 1) {
		int i1 = y * 2;
		int i2 = (y * 2) + 1;

		i1 += totalLinesX * 2;
		i2 += totalLinesX * 2;

		// pixel space
		float yPos = ((float)rt.mapCoordsToPixel(sf::Vector2f(0, (float)lineSpacing)).y - (float)rt.mapCoordsToPixel(sf::Vector2f(0, 0)).y) * (float)y;

		lines[i1].position = sf::Vector2f(rt.mapPixelToCoords(sf::Vector2i(sf::Vector2f(0.0f - (float)lineSpacing*2.0f, yPos) - offset)));
		lines[i2].position = sf::Vector2f(rt.mapPixelToCoords(sf::Vector2i(sf::Vector2f(prevSize.x + lineSpacing, yPos) - offset)));
		lines[i1].color = lineColor;
		lines[i2].color = lineColor;
	}

	rt.draw(lines);
}

void GraphEditorView::InfoBarData() {
	std::stringstream posStream;
	posStream << "View: (" << round(vcenter.x * 100.0f) / 100.0f << ", " << round(vcenter.y * 100.0f) / 100.0f << ")";
	ImGui::Text(posStream.str().c_str());

	ImGui::SameLine(0.0f, 25.0f);

	std::stringstream zoomStream;
	zoomStream << "Zoom: " << round(zoom * 100.0f) / 100.0f;
	ImGui::Text(zoomStream.str().c_str());

	ImGui::SameLine(0.0f, 25.f);

	std::stringstream threadCStream;
	threadCStream << "Node Thread N#: " << currentThreadCount;
	ImGui::Text(threadCStream.str().c_str());
}

void GraphEditorView::EvalNodeThread(int nodeIndex) {
	std::vector<std::thread> threadList;
	Utility::Timer traceTmr;

	auto& node = nodes[nodeIndex];
	for (auto& pin : node->pins) {
		if (pin.dir == Direction::In) {
			if (pin.inNodeId == -1) { continue; }

			auto& inNode = nodes[pin.inNodeId];
			if (inNode->evaluated) { continue; }

			// Run this function the in node
			threadList.push_back(std::thread(&GraphEditorView::EvalNodeThread, this, pin.inNodeId));

			// Convert and transfer the in node's output
			Utility::Timer convTmr;
			int p = 0;
			for (auto& pin : inNode->pins) {
				if (pin.dir == Direction::Out) {
					int i = 0;
					for (auto& outinNodeId : pin.outNodeIds) {
						auto& targetinNode = nodes[outinNodeId];
						if (targetinNode->luaVarData[targetinNode->pinLuaVars[pin.outPinIndexes[i]]].dataType != inNode->luaVarData[inNode->pinLuaVars[p]].dataType) {
							if (inNode->luaVarData[inNode->pinLuaVars[p]].dataType == Types::DataType_GreyTex) {
								Types::colortex converted(texSize.x, std::vector<sf::Color>(texSize.y, sf::Color::White));
								int x = 0;
								for (auto& xRow : inNode->luaVarData[inNode->pinLuaVars[p]].greytexVar) {
									int yI = 0;
									for (auto& y : xRow) {
										converted[x][yI] = sf::Color(y, y, y, 255);
										yI++;
									}
									x++;
								}

								targetinNode->luaVarData[targetinNode->pinLuaVars[pin.outPinIndexes[i]]].colortexVar = converted;
							}
							else {
								Types::greytex converted(texSize.x, std::vector<int>(texSize.y, 0));
								int x = 0;
								for (auto& xRow : inNode->luaVarData[inNode->pinLuaVars[p]].colortexVar) {
									int yI = 0;
									for (auto& y : xRow) {
										converted[x][yI] = std::floor(y.r * 0.3f + y.g * 0.59f + y.b * 0.11f);
										yI++;
									}
									x++;
								}

								targetinNode->luaVarData[targetinNode->pinLuaVars[pin.outPinIndexes[i]]].greytexVar = converted;
							}
						}
						else {
							if (inNode->luaVarData[inNode->pinLuaVars[p]].dataType == Types::DataType_ColorTex) {
								targetinNode->luaVarData[targetinNode->pinLuaVars[pin.outPinIndexes[i]]].colortexVar = inNode->luaVarData[inNode->pinLuaVars[p]].colortexVar;
							}
							else {
								targetinNode->luaVarData[targetinNode->pinLuaVars[pin.outPinIndexes[i]]].greytexVar = inNode->luaVarData[inNode->pinLuaVars[p]].greytexVar;
							}
						}
						i++;
					}
				}
				p++;
			}
			LOG_TRACE("Conversion and transfer for inNode class '{1}' took {0}ms", convTmr.Elapsed() * 1000.f, inNode->nodeClass);
		}
	}

	for (auto& thread : threadList) {
		thread.join();
	}

	threadList.clear();
	// Execute the main node this function is evaluating
	auto mainEvalThread = std::thread(&GraphNode::Execute, node, &currentThreadCount);
	mainEvalThread.join();

	// Convert and transfer the main node's output
	int p = 0;
	for (auto& pin : node->pins) {
		if (pin.dir == Direction::Out) {
			int i = 0;
			for (auto& outNodeId : pin.outNodeIds) {
				auto& targetNode = nodes[outNodeId];
				if (targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].dataType != node->luaVarData[node->pinLuaVars[p]].dataType) {
					if (node->luaVarData[node->pinLuaVars[p]].dataType == Types::DataType_GreyTex) {
						Types::colortex converted(texSize.x, std::vector<sf::Color>(texSize.y, sf::Color::White));
						int x = 0;
						for (auto& xRow : node->luaVarData[node->pinLuaVars[p]].greytexVar) {
							int yI = 0;
							for (auto& y : xRow) {
								converted[x][yI] = sf::Color(y, y, y, 255);
								yI++;
							}
							x++;
						}

						targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].colortexVar = converted;
					}
					else {
						Types::greytex converted(texSize.x, std::vector<int>(texSize.y, 0));
						int x = 0;
						for (auto& xRow : node->luaVarData[node->pinLuaVars[p]].colortexVar) {
							int yI = 0;
							for (auto& y : xRow) {
								converted[x][yI] = std::floor(y.r * 0.3f + y.g * 0.59f + y.b * 0.11f);
								yI++;
							}
							x++;
						}

						targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].greytexVar = converted;
					}
				}
				else {
					if (node->luaVarData[node->pinLuaVars[p]].dataType == Types::DataType_ColorTex) {
						targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].colortexVar = node->luaVarData[node->pinLuaVars[p]].colortexVar;
					}
					else {
						targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].greytexVar = node->luaVarData[node->pinLuaVars[p]].greytexVar;
					}
				}
				i++;
			}
		}
		p++;
	}
	LOG_TRACE("Reverse split-threaded evaluation for node class '{1}' took{0}ms", traceTmr.Elapsed() * 1000.f, node->nodeClass);
}

void GraphEditorView::EvaluateNodes() {
	Utility::Timer execOrderTmr;

	if (CheckCyclical()) {
		LOG_ERROR("The graph cannot be evaluated when it contains cyclical dependencies.");
		return;
	}

	/*std::vector<int> evalOrder = TopologicalSort();
	LOG_TRACE("--- Eval Order ---");
	int n = 0;
	for (int nodeI : evalOrder) {
		auto* node = nodes[nodeI];
		node->debugEvalIndex = n;
		LOG_TRACE("{0} at pos ({1}, {2})", node->nodeClass, node->nodePos.x, node->nodePos.y);

		node->SetDirty();
		n++;
	}

	LOG_INFO("Calculated execution order in {0}ms", execOrderTmr.Elapsed()*1000.f);*/

	Utility::Timer execTmr;

	// NEW METHOD: USE IN PINS TO SEPERATE EVAL INTO THREADS WORKING BACKWARDS FROM END NODE (POTENTIAL MEMORY ISSUE IF SAVED GRAPH HAS MUTATED CYCLICAL DEPENDANCY (serialised incorrectly))
	std::vector<int> endNodes;
	int n = 0;
	for (auto* node : nodes) {
		// Set all nodes to be unevaluated. In future, trace forwards from changed nodes to find nodes that need to be reevaluated so that the whole graph isnt processed for no reason
		node->SetDirty();

		// determine if this is an "end" node (any nodes with 0 connected output pins)
		bool end = true;
		for (auto& pin : node->pins) {
			if (pin.outNodeIds.size() > 0) { end = false; }
		}
		if (end) {
			endNodes.push_back(n);
		}
		n++;
	}
	
	std::vector<std::thread> evalThreads;
	for (int endNode : endNodes) {
		evalThreads.push_back(std::thread(&GraphEditorView::EvalNodeThread, this, endNode));
	}
	for (auto& thread : evalThreads) {
		thread.join();
	}

	// OLD METHOD: EVALUATE ALL NODES CONSECUTIVELY USING TOPOLOGICAL SORT
	//for (int nodeIndex : evalOrder) {
	//	auto& node = nodes[nodeIndex];
	//	node->Execute();
	//	
	//	// Pass data to connected nodes
	//	Utility::Timer convTmr;
	//	int p = 0;
	//	for (auto& pin : node->pins) {
	//		if (pin.dir == Direction::Out) {
	//			int i = 0;
	//			for (auto& outNodeId : pin.outNodeIds) {
	//				auto& targetNode = nodes[outNodeId];
	//				if (targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].dataType != node->luaVarData[node->pinLuaVars[p]].dataType) {
	//					if (node->luaVarData[node->pinLuaVars[p]].dataType == Types::DataType_GreyTex) {
	//						Types::colortex converted(texSize.x, std::vector<sf::Color>(texSize.y, sf::Color::White));
	//						int x = 0;
	//						for (auto& xRow : node->luaVarData[node->pinLuaVars[p]].greytexVar) {
	//							int yI = 0;
	//							for (auto& y : xRow) {
	//								converted[x][yI] = sf::Color(y, y, y, 255);
	//								yI++;
	//							}
	//							x++;
	//						}
	//						targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].colortexVar = converted;
	//					}
	//					else {
	//						Types::greytex converted(texSize.x, std::vector<int>(texSize.y, 0));
	//						int x = 0;
	//						for (auto& xRow : node->luaVarData[node->pinLuaVars[p]].colortexVar) {
	//							int yI = 0;
	//							for (auto& y : xRow) {
	//								converted[x][yI] = std::floor(y.r * 0.3f + y.g * 0.59f + y.b * 0.11f);
	//								yI++;
	//							}
	//							x++;
	//						}
	//						targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].greytexVar = converted;
	//					}
	//				}
	//				else {
	//					if (node->luaVarData[node->pinLuaVars[p]].dataType == Types::DataType_ColorTex) {
	//						targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].colortexVar = node->luaVarData[node->pinLuaVars[p]].colortexVar;
	//					}
	//					else {
	//						targetNode->luaVarData[targetNode->pinLuaVars[pin.outPinIndexes[i]]].greytexVar = node->luaVarData[node->pinLuaVars[p]].greytexVar;
	//					}
	//				}
	//				i++;
	//			}
	//		}
	//		p++;
	//	}
	//	LOG_TRACE("Conversion and transfer for node class '{1}' took {0}ms", convTmr.Elapsed() * 1000.f, node->nodeClass);
	//}

	LOG_INFO("Executed graph in {0}ms", execTmr.Elapsed()*1000.f);
}

sf::Color lerpColor(sf::Color a, sf::Color b, float t) {
	sf::Color c;
	c.r = std::lerp(a.r, b.r, t);
	c.g = std::lerp(a.g, b.g, t);
	c.b = std::lerp(a.b, b.b, t);
	c.a = std::lerp(a.a, b.a, t);
	return c;
}

void GraphEditorView::RenderLine(sf::Vector2f start, sf::Vector2f end, sf::Color startCol, sf::Color endCol, float width) {
	// 4 Point Bezier =>| P = (1-t)^3 * P0 + 3 * (1-t)^2 * t * P1 + 3(1-t) * t^2 * P2 + t^3 * P3 |
	if (start == end) { return; }

	sf::Vector2f P0 = start;
	sf::Vector2f P3 = end;
	sf::Vector2f P1 = sf::Vector2f((start.x + end.x) / 2, start.y);
	sf::Vector2f P2 = sf::Vector2f((start.x + end.x) / 2, end.y);

	std::vector<sf::Vector2f> points;
	for (float t = 0; t <= 1.01f; t += 0.01f) {
		sf::Vector2f newPoint;
		newPoint = (float)pow(1 - t, 3) * P0 + 3.f * (float)pow(1 - t, 2) * t * P1 + 3.f * (1 - t) * (float)pow(t, 2) * P2 + (float)pow(t, 3) * P3;
		points.push_back(newPoint);
	}

	sf::Vector2f prevPos[2] = { sf::Vector2f(0,0), sf::Vector2f(0,0) };
	sf::Color prevColor = sf::Color(0, 0, 0, 0);

	for (int p = 1; p < points.size(); p++) {
		sf::VertexArray line(sf::TriangleStrip, 4);

		sf::Vector2f dir = (points[p] - points[p - 1]).normalized();
		sf::Vector2f widthDir = sf::Vector2f(dir.y, -dir.x); // Clockwise perpendicular

		if (prevPos[0] == sf::Vector2f(0, 0)) {
			line[0].position = points[p - 1] + (widthDir * std::abs(width / 2));
			line[1].position = points[p - 1] - (widthDir * std::abs(width / 2));
		}
		else {
			line[0].position = prevPos[0];
			line[1].position = prevPos[1];
		}
		line[2].position = points[p] + (widthDir * std::abs(width / 2));
		line[3].position = points[p] - (widthDir * std::abs(width / 2));
		prevPos[0] = line[2].position;
		prevPos[1] = line[3].position;

		sf::Color col = lerpColor(startCol, endCol, (float)p / (float)points.size());

		if (prevColor.a == 0) {
			line[0].color = startCol;
			line[1].color = startCol;
		}
		else {
			line[0].color = prevColor;
			line[1].color = prevColor;
		}
		line[2].color = col;
		line[3].color = col;

		prevColor = line[2].color;

		rt.draw(line);
	}
}

void ToolBarSeparator() {
	ImGui::Image(*ImageCache::images["detail-separator"], sf::Vector2f(15, 35));
}

void GraphEditorView::ToolBarButtons() {
	if (ImGui::ImageButton(*ImageCache::images["icon-eval"], sf::Vector2f(25, 25), 5)) {
		auto evalThread = std::thread(&GraphEditorView::EvaluateNodes, this);
		evalThread.detach();
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Evaluate Graph");

	ImGui::SameLine();

	ToolBarSeparator();

	ImGui::SameLine();

	if (ImGui::ImageButton(*ImageCache::images["icon-save-1"], sf::Vector2f(25, 25), 5)) {
		saveCallback();
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Save Graph");

}

// RENDER STEP FOR THIS VIEW
void GraphEditorView::ComponentRender() {
	if (Globals::currentGraph == "") {
		return;
	}

	sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));
	if (moving) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}

	// Draw Grid (NEEDS WORK)
	Grid();

	// Draw Lines
	if (draggingLine) {
		RenderLine(nodes[currentLineFromNode]->pinPosCache[currentLineFromPin], mousePos, Types::typeToColor[nodes[currentLineFromNode]->pins[currentLineFromPin].type], Types::typeToColor[nodes[currentLineFromNode]->pins[currentLineFromPin].type], 5);
	}
	int cn = 0;
	for (auto* node : nodes) {
		int p = 0;
		for (auto& pin : node->pins) {
			if (pin.dir == Direction::Out) {
				int n = 0;
				for (int targetNodeID : pin.outNodeIds) {
					if (targetNodeID > nodes.size() - 1) { pin.outNodeIds.erase(pin.outNodeIds.begin() + n); pin.outPinIndexes.erase(pin.outPinIndexes.begin() + n); n++; continue; }
					sf::Color startColor = Types::typeToColor[node->pins[p].type];
					sf::Color endColor = Types::typeToColor[nodes[targetNodeID]->pins[pin.outPinIndexes[n]].type];

					if (cyclicalError && cyclicalNode == cn && cyclicalPin == p) {
						startColor = sf::Color(255, 0, 0, 255);
						endColor = startColor;
					}
					RenderLine(node->pinPosCache[p], nodes[targetNodeID]->pinPosCache[pin.outPinIndexes[n]], startColor, endColor, 5);
					n++;
				}
			}
			p++;
		}
		cn++;
	}

	
	// Draw Nodes (with culling)
	int n = 0;
	for (auto* node : nodes) {
		sf::FloatRect bounds = node->calcBounds();
		sf::FloatRect viewRect(view.getCenter() - view.getSize() / 2.f, view.getSize());
		if (viewRect.findIntersection(bounds) == std::nullopt && node->renderedPins) {
			n++;
			continue;
		}
		node->SFMLRender(rt, zoom, selectedNode==n || std::find(multiSelectNodes.begin(), multiSelectNodes.end(), n) != multiSelectNodes.end(), cyclicalError ? 127 : 255);
		n++;
	}

	if (selectBoxStart != sf::Vector2f(0, 0)) {
		sf::Vector2f pos = sf::Vector2f(std::min(selectBoxStart.x, selectBoxEnd.x), std::min(selectBoxStart.y, selectBoxEnd.y));
		sf::Vector2f posMax = sf::Vector2f(std::max(selectBoxStart.x, selectBoxEnd.x), std::max(selectBoxStart.y, selectBoxEnd.y));
		sf::Vector2f size = posMax - pos;
		sf::FloatRect selectRect = sf::FloatRect(pos, size);

		sf::RectangleShape selectBox(size);
		selectBox.setPosition(pos);
		selectBox.setFillColor(sf::Color(122, 193, 255, 90));
		selectBox.setOutlineColor(sf::Color(122, 193, 255, 200));
		selectBox.setOutlineThickness(2.f);
		rt.draw(selectBox);
	}
}

sf::Vector2f GraphEditorView::pixelToGraph(sf::Vector2i pixel) {
	return rt.mapPixelToCoords(sf::Vector2i(pixel.x - (int)prevPos.x, pixel.y - (int)prevPos.y));
}

sf::Vector2f GraphEditorView::snapPos(sf::Vector2f pos) {
	float snapSize = 16.f;
	return sf::Vector2f(std::floor(pos.x / snapSize) * snapSize, std::floor(pos.y / snapSize) * snapSize);
}

void GraphEditorView::IMGUIRender() {
	if (Globals::currentGraph == "") {
		if (ImGui::IsWindowHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
			ImGui::SetTooltip("No graph opened");
		}
		return;
	}

	inFocus = ImGui::IsWindowFocused();
	if (ImGui::BeginDragDropTarget()) 
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE_CLASS_INSTANCE"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			int payload_nodeIndex = *(const int*)payload->Data;

			GraphNode* compiledNode = LibraryManager::compiledNodes[payload_nodeIndex];
			sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));

			GraphNode* newNode = copyNode(compiledNode); // copy function makes sure that if its a subclass it doesnt get converted down
			newNode->nodePos = snapPos(mousePos);
			newNode->nodeId = nodes.size(); // temp id assignment
			newNode->SetTextureSize(texSize);
			nodes.push_back(newNode); 

			selectedNode = (int)nodes.size() - 1;

			dirty = true;

			LOG_TRACE("Instantiated Node (Node Class '{0}')", newNode->nodeClass);
			LOG_TRACE("Position: ({0}, {1})", newNode->nodePos.x, newNode->nodePos.y);
		}

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_RESOURCE_INSTANCE"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string payload_resource = *(const std::string*)payload->Data;

			ImageNode* newNode = new ImageNode();
			sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));

			newNode->nodePos = snapPos(mousePos);
			newNode->nodeId = nodes.size(); // temp id assignment
			newNode->SetTextureSize(texSize);
			newNode->resourceName = payload_resource;
			nodes.push_back(newNode);

			dirty = true;

			selectedNode = (int)nodes.size() - 1;
		}

		ImGui::EndDragDropTarget();
	}
}

float remapRange2(float value, float low1, float high1, float low2, float high2) {
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

void GraphEditorView::UpdateTexSize(sf::Vector2i size)
{
	texSize = size;
	for (auto* node : nodes) {
		node->SetTextureSize(texSize);
	}
}

void GraphEditorView::DeleteNode(int index)
{
	for (auto& pin : nodes[index]->pins) {
		if (pin.inNodeId != -1) {
			auto& targetOutNodeIds = nodes[pin.inNodeId]->pins[pin.inPinIndex].outNodeIds;
			auto& targetOutPinIndexes = nodes[pin.inNodeId]->pins[pin.inPinIndex].outPinIndexes;
			auto ref = std::find(targetOutNodeIds.begin(), targetOutNodeIds.end(), index);
			if (ref != targetOutNodeIds.end()) {
				int refIndex = std::distance(targetOutNodeIds.begin(), ref);
				targetOutNodeIds.erase(targetOutNodeIds.begin() + refIndex);
				targetOutPinIndexes.erase(targetOutPinIndexes.begin() + refIndex);
			}
		}
		int n = 0;
		for (auto outNode : pin.outNodeIds) {
			nodes[outNode]->pins[pin.outPinIndexes[n]].inNodeId = -1;
			nodes[outNode]->pins[pin.outPinIndexes[n]].inPinIndex = -1;
			n++;
		}
	}

	// update all other node references (shift any node indexes above the deleted node index down by 1)
	for (int n = 0; n < nodes.size(); n++) {  
		auto* node = nodes[n];
		if (n > index) {
			for (auto& pin : nodes[n]->pins) {
				pin.nodeId -= 1;
				if (pin.inNodeId < 0) { continue; }
				if (pin.inNodeId >= nodes.size()) { continue; }
				auto& targetOutNodeIds = nodes[pin.inNodeId]->pins[pin.inPinIndex].outNodeIds;
				auto ref = std::find(targetOutNodeIds.begin(), targetOutNodeIds.end(), n);
				if (ref != targetOutNodeIds.end()) {
					int refIndex = std::distance(targetOutNodeIds.begin(), ref);
					targetOutNodeIds[refIndex] -= 1;
				}
			}
			node->nodeId -= 1;
		}
		else { 
			for (auto& pin : nodes[n]->pins) {
				if (pin.inNodeId <= index) { continue; }
				pin.inNodeId -= 1;
			}
		}
	}

	delete nodes[index];
	nodes.erase(nodes.begin() + index);
	dirty = true;
}

// returns true if found cyclical reference
bool GraphEditorView::CyclicalRec(int currentNode, std::vector<int> stack, int prevPin, int prevNode) {
	if (std::find(stack.begin(), stack.end(), currentNode) != stack.end()) { 
		cyclicalError = true;
		cyclicalNode = prevNode;
		cyclicalPin = prevPin;
		return true;
	}
	stack.push_back(currentNode);
	bool cyclical = false;
	int p = 0;
	for (auto& pin : nodes[currentNode]->pins)
	{
		for (int outIndex : pin.outNodeIds)
		{
			if(outIndex >= nodes.size()) {
				int index = std::find(pin.outNodeIds.begin(), pin.outNodeIds.end(), outIndex) - pin.outNodeIds.begin();
				pin.outNodeIds.erase(pin.outNodeIds.begin() + index);
				pin.outPinIndexes.erase(pin.outPinIndexes.begin() + index);
				p++;
				continue;
			}
			cyclical = cyclical || CyclicalRec(outIndex, stack, p, currentNode);
		}
		p++;
	}
	return cyclical;
}

template <typename I>
I random_element(I begin, I end)
{
	const unsigned long n = std::distance(begin, end);
	const unsigned long divisor = (RAND_MAX + 1) / n;

	unsigned long k;
	do { k = std::rand() / divisor; } while (k >= n);

	std::advance(begin, k);
	return begin;
}

bool GraphEditorView::CheckCyclical() {
	bool cyclical = false;
	for (int n = 0; n < nodes.size(); n++) {
		cyclical = cyclical || CyclicalRec(n, std::vector<int>());
	}
	if (!cyclical) {
		cyclicalError = false;
	}
	else {
		LOG_WARN("Cyclical graph dependancy");
		/*sf::Vector2f windowPos = sf::Vector2f(rt.mapCoordsToPixel(nodes[cyclicalNode]->nodePos)) - sf::Vector2f(prevPos) - sf::Vector2f(prevSize.x/2, prevSize.y/2);
		vcenter = sf::Vector2f(windowPos.x / prevSize.x, windowPos.y / prevSize.y);
		zoom = 0.6f;*/
		// ATTEMPT TO ZOOM CAMERA ON ERROR - NEEDS WORK
	}
	LOG_TRACE("Graph Cyclical: {0}", cyclical ? "TRUE" : "FALSE");

	return cyclical;
}

void GraphEditorView::TopologicalSortRec(int currentNode, std::vector<int>& ordering) {
	auto* cNode = nodes[currentNode];
	for (auto& pin : cNode->pins) {
		for (auto outNodeI : pin.outNodeIds) {
			if (std::find(ordering.begin(), ordering.end(), outNodeI) == ordering.end()) {
				TopologicalSortRec(outNodeI, ordering);
			}
		}
	}

	// no more nodes, add self to list
	ordering.push_back(currentNode);
}

std::vector<int> GraphEditorView::TopologicalSort() {
	if (nodes.size() == 0) { return std::vector<int>(); }
	std::vector<int> orderingList;
	while (orderingList.size() != nodes.size()) {
		std::vector<int> options;
		for (int n = 0; n < nodes.size(); n++) {
			if (std::find(orderingList.begin(), orderingList.end(), n) == orderingList.end()) {
				options.push_back(n);
			}
		}

		TopologicalSortRec(*random_element(options.begin(), options.end()), orderingList);
	}
	std::reverse(orderingList.begin(), orderingList.end());
	return orderingList;
}

// SFML EVENTS FOR THIS VIEW
bool GraphEditorView::ProcessEvent(sf::Event& event) {
	if (Globals::currentGraph == "") {
		return false;
	}

	switch (event.type) {
	case sf::Event::MouseButtonPressed:
	{
		if (event.mouseButton.button == sf::Mouse::Button::Middle) {
			moving = true;
			sf::Vector2i pos = sf::Mouse::getPosition();
			oldPos = sf::Vector2f((float)pos.x, (float)pos.y);
			return true; // gain handling priority
		}
		else if (event.mouseButton.button == sf::Mouse::Button::Left) {
			dirty = true;
			sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));

			selectedNode = -1;
			int n1 = 0;
			for (auto* node : nodes) {
				auto rect = node->calcBounds();
				if (rect.contains(mousePos)) {
					selectedNode = n1;
				}
				n1++;
			}

			if (selectedNode != -1 && multiSelectNodes.size() == 0) {
				draggingNode = true;
				dragNodeOffset = nodes[selectedNode]->nodePos - mousePos;
				return true; // gain handling priority
			}

			if (multiSelectNodes.size() > 0 && selectedNode != -1) {
				draggingNodeMultiple = true;
				dragNodeOffset = nodes[selectedNode]->nodePos - mousePos;
				dragNodeRef = selectedNode;
				selectedNode = -1;
				return true; // gain handling priority
			}

			// Detect closest pin for line drag
			int closestNode = -1;
			int closestPin = -1;
			float closestDist = 100000;
			int n = 0;
			for (auto* node : nodes) {
				float dist = (node->nodePos - mousePos).length();
				if (dist > 500) { n++; continue; }
				int p = 0;
				for (auto& pin : node->pins) {
					float pDist = (node->pinPosCache[p] - mousePos).length();
					if (pDist < closestDist) {
						closestNode = n;
						closestPin = p;
						closestDist = pDist;
					}
					p++;
				}
				n++;
			}

			if (closestPin > -1 && closestDist < 15) {
				currentLineFromNode = closestNode;
				currentLineFromPin = closestPin;
				draggingLine = true;
				return true; // gain handling priority
			}

			selectBoxStart = mousePos;
			selectBoxEnd = mousePos;
			return true; // gain handling priority
		}
	} break;
	case  sf::Event::MouseButtonReleased:
	{
		if (event.mouseButton.button == sf::Mouse::Button::Middle) {
			moving = false;
			return true; // lose handling priority
		}
		else if (event.mouseButton.button == sf::Mouse::Button::Left) {
			if (draggingNode) {
				draggingNode = false;
				return true; // lose handling priority
			}
			if (draggingNodeMultiple) {
				draggingNodeMultiple = false;
				return true; // lose handling priority
			}
			if (draggingLine) {
				draggingLine = false;
				auto& fromNode = nodes[currentLineFromNode];
				auto& fromPin = fromNode->pins[currentLineFromPin];

				sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));

				// Detect closest pin for line drag
				int closestNode = -1;
				int closestPin = -1;
				float closestDist = 100000;
				int n = 0;
				for (auto* node : nodes) {
					float dist = (node->nodePos - mousePos).length();
					if (dist > 200) { n++; continue; }
					int p = 0;
					for (auto& pin : node->pins) {
						float pDist = (node->pinPosCache[p] - mousePos).length();
						if (pDist < closestDist) {
							closestNode = n;
							closestPin = p;
							closestDist = pDist;
						}
						p++;
					}
					n++;
				}

				if (closestPin > -1 && closestDist < 15) {
					// there is a pin to connect to
					if (closestNode == currentLineFromNode) { break; }
					if (fromPin.dir == Direction::Out) {
						if (nodes[closestNode]->pins[closestPin].dir == Direction::In) {
							// valid out -> in
							fromPin.outNodeIds.push_back(closestNode);
							fromPin.outPinIndexes.push_back(closestPin);

							if (nodes[closestNode]->pins[closestPin].inNodeId != -1) {
								// update old reference
								auto& oldNode = nodes[nodes[closestNode]->pins[closestPin].inNodeId];
								auto& oldPin = oldNode->pins[nodes[closestNode]->pins[closestPin].inPinIndex];
								int index = std::find(oldPin.outNodeIds.begin(), oldPin.outNodeIds.end(), closestNode) - oldPin.outNodeIds.begin();
								oldPin.outNodeIds.erase(oldPin.outNodeIds.begin() + index);
								oldPin.outPinIndexes.erase(oldPin.outPinIndexes.begin() + index);
							}

							nodes[closestNode]->pins[closestPin].inNodeId = currentLineFromNode;
							nodes[closestNode]->pins[closestPin].inPinIndex = currentLineFromPin;
						}
					}
					else {
						if (nodes[closestNode]->pins[closestPin].dir == Direction::Out) {
							// valid in -> out
							nodes[closestNode]->pins[closestPin].outNodeIds.push_back(currentLineFromNode);
							nodes[closestNode]->pins[closestPin].outPinIndexes.push_back(currentLineFromPin);

							fromPin.inNodeId = closestPin;
							fromPin.inPinIndex = closestPin;
						}
					}
				}

				CheckCyclical();
				return true; // lose handling priority
			}

			if (selectBoxStart != sf::Vector2f(0, 0))
			{
				multiSelectNodes.clear();

				sf::Vector2f pos = sf::Vector2f(std::min(selectBoxStart.x, selectBoxEnd.x), std::min(selectBoxStart.y, selectBoxEnd.y));
				sf::Vector2f posMax = sf::Vector2f(std::max(selectBoxStart.x, selectBoxEnd.x), std::max(selectBoxStart.y, selectBoxEnd.y));
				sf::Vector2f size = posMax - pos;
				sf::FloatRect selectRect = sf::FloatRect(pos, size);

				for (int n = 0; n < nodes.size(); n++) {
					auto* node = nodes[n];
					auto rect = node->calcBounds();
					if (rect.findIntersection(selectRect)) {
						multiSelectNodes.push_back(n);
					}
				}

				selectBoxStart = sf::Vector2f(0, 0);
				selectBoxEnd = selectBoxStart;
				return true; // lose handling priority
			}
		}
	} break;
	case sf::Event::MouseMoved:
	{
		if (draggingNode)
		{
			if (selectedNode == -1) { draggingNode = false; break; }
			sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));
			nodes[selectedNode]->nodePos = snapPos(mousePos + dragNodeOffset);
		}
		if (draggingNodeMultiple)
		{
			if (multiSelectNodes.size() == 0) { draggingNodeMultiple = false; break; }
			sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));
			auto oldPos = nodes[dragNodeRef]->nodePos;
			auto dif = (mousePos + dragNodeOffset) - oldPos;
			for (int nodeIndex : multiSelectNodes) {
				nodes[nodeIndex]->nodePos = snapPos(nodes[nodeIndex]->nodePos + dif);
			}
		}

		if (moving)
		{
			sf::Vector2i pos = sf::Mouse::getPosition();
			sf::Vector2f newPos = sf::Vector2f((float)pos.x, (float)pos.y);
			sf::Vector2f deltaPos = oldPos - newPos;
			deltaPos = sf::Vector2f(deltaPos.x * zoom, deltaPos.y * zoom);
			vcenter += sf::Vector2f(deltaPos.x / prevSize.x, deltaPos.y / prevSize.y);

			oldPos = sf::Vector2f((float)pos.x, (float)pos.y);
			
		}

		if (selectBoxStart != sf::Vector2f(0, 0))
		{
			sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));
			selectBoxEnd = mousePos;
		}
	} break;
	case sf::Event::MouseWheelScrolled:
	{
		if (moving)
			break;

		float zoomSens;
		float sensMin = 0.05f;
		float sensMax = 0.6f;
		zoomSens = remapRange2(zoom, 0.4f, 1.0f, sensMin, sensMax);
		zoomSens = std::clamp(zoomSens, sensMin, sensMax);

		if (event.mouseWheelScroll.delta <= -1)
			zoom = std::min(4.f, zoom + zoomSens);
		else if (event.mouseWheelScroll.delta >= 1)
			zoom = std::max(.2f, zoom - zoomSens);
	} break;
	case sf::Event::KeyPressed:
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Delete: 
		{
			if (!inFocus) { break; }
			if (selectedNode == -1) { break; }
			DeleteNode(selectedNode);
			selectedNode = -1;
			CheckCyclical();
		} break;
		case sf::Keyboard::Backspace:
		{
			if (!inFocus) { break; }
			if (selectedNode == -1) { break; }
			DeleteNode(selectedNode);
			selectedNode = -1;
			CheckCyclical();
		} break;
		default:
			break;
		}
	} break;
	}

	return false;
}