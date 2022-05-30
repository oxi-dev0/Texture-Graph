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
} // JUST CALLS CONSTRUCTOR FOR SUBWINDOW

void GraphEditorView::Clear() {
	for (auto* node : nodes) {
		if (node == nullptr) { continue; }
		delete node;
	}
	nodes.clear();
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

void GraphEditorView::EvaluateNodes() {
	Utility::Timer evalTmr;

	evalThreads.clear();
	for (auto* node : nodes) {
		evalThreads.push_back(std::thread(&GraphNode::Execute, node));
		//node->Execute();
	}

	for (auto& thread : evalThreads) {
		thread.join();
	}

	LOG_INFO("Evaluated graph in {0}ms", evalTmr.Elapsed()*1000.f);
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

// RENDER STEP FOR THIS VIEW
void GraphEditorView::ComponentRender() {
	sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));
	if (moving) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}

	// Draw Grid (NEEDS WORK)
	Grid();

	// Draw Lines
	if (draggingLine) {
		RenderLine(nodes[currentLineFromNode]->pinPosCache[currentLineFromPin], mousePos, Types::typeToColor[nodes[currentLineFromNode]->pins[currentLineFromPin].type], Types::typeToColor[nodes[currentLineFromNode]->pins[currentLineFromPin].type], 5);
	}
	for (auto* node : nodes) {
		int p = 0;
		for (auto& pin : node->pins) {
			if (pin.dir == Direction::Out) {
				int n = 0;
				for (int targetNodeID : pin.outNodeIds) {
					if (targetNodeID > nodes.size() - 1) { pin.outNodeIds.erase(pin.outNodeIds.begin() + n); pin.outPinIndexes.erase(pin.outPinIndexes.begin() + n); n++; continue; }
					RenderLine(node->pinPosCache[p], nodes[targetNodeID]->pinPosCache[pin.outPinIndexes[n]], Types::typeToColor[node->pins[p].type], Types::typeToColor[nodes[targetNodeID]->pins[pin.outPinIndexes[n]].type], 5);
					n++;
				}
			}
			p++;
		}
	}

	
	// Draw Nodes (with culling)
	int n = 0;
	for (auto* node : nodes) {
		sf::FloatRect bounds = node->calcBounds();
		sf::FloatRect viewRect(view.getCenter() - view.getSize() / 2.f, view.getSize());
		if (viewRect.findIntersection(bounds) == std::nullopt) {
			n++;
			continue;
		}
		node->SFMLRender(rt, zoom, selectedNode==n);
		n++;
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
	inFocus = ImGui::IsWindowFocused();
	if (ImGui::BeginDragDropTarget()) 
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NODE_CLASS_INSTANCE"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			int payload_nodeIndex = *(const int*)payload->Data;

			GraphNode& compiledNode = LibraryManager::compiledNodes[payload_nodeIndex];
			sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));

			GraphNode* newNode = new GraphNode(compiledNode);
			newNode->nodePos = snapPos(mousePos);
			newNode->nodeId = nodes.size(); // temp id assignment
			nodes.push_back(newNode); 

			selectedNode = (int)nodes.size() - 1;

			LOG_TRACE("Instantiated Node (Node Class '{0}')", newNode->nodeClass);
			LOG_TRACE("Position: ({0}, {1})", newNode->nodePos.x, newNode->nodePos.y);
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

// SFML EVENTS FOR THIS VIEW
bool GraphEditorView::ProcessEvent(sf::Event& event) {
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

			if (selectedNode != -1) {
				draggingNode = true;
				dragNodeOffset = nodes[selectedNode]->nodePos - mousePos;
				return true; // gain handling priority
			}

			// Detect closest pin for line drag ( no node selected for now )
			int closestNode1 = -1;
			int closestNode2 = -1;
			float closestDist1 = 100000;
			float closestDist2 = 100000;
			int n = 0;
			for (auto* node : nodes) {
				float dist = (node->nodePos - mousePos).length();
				if (dist < closestDist1) {
					closestDist1 = dist;
					closestNode1 = n;
				}
				else {
					if (dist < closestDist2) {
						closestDist2 = dist;
						closestNode2 = n;
					}
				}
				n++;
			}

			int closestPin1 = -1;
			float closestPinDist1 = 1000;
			int closestPin2 = -1;
			float closestPinDist2 = 1000;
			if (closestNode1 > -1) {
				int p = 0;
				for (auto& pin : nodes[closestNode1]->pins) {
					float dist = (nodes[closestNode1]->pinPosCache[p] - mousePos).length();
					if (dist < closestPinDist1) {
						closestPinDist1 = dist;
						closestPin1 = p;
					}
					p++;
				}
			}
			if (closestNode2 > -1) {
				int p = 0;
				for (auto& pin : nodes[closestNode2]->pins) {
					float dist = (nodes[closestNode2]->pinPosCache[p] - mousePos).length();
					if (dist < closestPinDist2) {
						closestPinDist2 = dist;
						closestPin2 = p;
					}
					p++;
				}
			}

			if (closestPinDist1 > 40)
			{
				closestNode1 = -1;
				closestPin1 = -1;
			}
			if (closestPinDist2 > 40)
			{
				closestNode2 = -1;
				closestPin2 = -1;
			}

			int targetNode = closestNode1;
			int targetPin = closestPin1;
			if (closestPinDist2 < closestPinDist1)
			{
				targetNode = closestNode2;
				targetPin = closestPin2;
			}

			if (targetNode > -1 && targetPin > -1) {
				currentLineFromNode = targetNode;
				currentLineFromPin = targetPin;
				draggingLine = true;
				return true; // gain handling priority
			}
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
			if (draggingLine) {
				draggingLine = false;
				auto& fromNode = nodes[currentLineFromNode];
				auto& fromPin = fromNode->pins[currentLineFromPin];

				sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));

				// Detect closest pin for line drag
				int closestNode1 = -1;
				int closestNode2 = -1;
				float closestDist1 = 100000;
				float closestDist2 = 100000;
				int n = 0;
				for (auto* node : nodes) {
					float dist = (node->nodePos - mousePos).length();
					if (dist < closestDist1) {
						closestDist1 = dist;
						closestNode1 = n;
					}
					else {
						if (dist < closestDist2) {
							closestDist2 = dist;
							closestNode2 = n;
						}
					}
					n++;
				}

				if (closestDist1 > 150) {
					closestNode1 = -1;
				}
				if (closestDist2 > 150) {
					closestNode2 = -1;
				}

				int closestPin1 = -1;
				float closestPinDist1 = 1000;
				int closestPin2 = -1;
				float closestPinDist2 = 1000;
				if (closestNode1 > -1) {
					int p = 0;
					for (auto& pin : nodes[closestNode1]->pins) {
						float dist = (nodes[closestNode1]->pinPosCache[p] - mousePos).length();
						if (dist < closestPinDist1) {
							closestPinDist1 = dist;
							closestPin1 = p;
						}
						p++;
					}
				}
				if (closestNode2 > -1) {
					int p = 0;
					for (auto& pin : nodes[closestNode2]->pins) {
						float dist = (nodes[closestNode2]->pinPosCache[p] - mousePos).length();
						if (dist < closestPinDist2) {
							closestPinDist2 = dist;
							closestPin2 = p;
						}
						p++;
					}
				}

				int targetNode = closestNode1;
				int targetPin = closestPin1;
				if (closestPinDist2 < closestPinDist2)
				{
					targetNode = closestNode2;
					targetPin = closestPin2;
				}

				if (targetNode > -1) {
					// there is a pin to connect to
					if (fromPin.dir == Direction::Out) {
						if (nodes[targetNode]->pins[targetPin].dir == Direction::In) {
							// valid out -> in
							fromPin.outNodeIds.push_back(targetNode);
							fromPin.outPinIndexes.push_back(targetPin);

							nodes[targetNode]->pins[targetPin].inNodeId = currentLineFromNode;
							nodes[targetNode]->pins[targetPin].inPinIndex = currentLineFromPin;
						}
					}
					else {
						if (nodes[targetNode]->pins[targetPin].dir == Direction::Out) {
							// valid in -> out
							nodes[targetNode]->pins[targetPin].outNodeIds.push_back(currentLineFromNode);
							nodes[targetNode]->pins[targetPin].outPinIndexes.push_back(currentLineFromPin);

							fromPin.inNodeId = targetNode;
							fromPin.inPinIndex = targetPin;
						}
					}
				}
			}
		}
	} break;
	case sf::Event::MouseMoved:
	{
		if (draggingNode)
		{
			sf::Vector2f mousePos = pixelToGraph(sf::Vector2i((int)ImGui::GetMousePos().x, (int)ImGui::GetMousePos().y));
			nodes[selectedNode]->nodePos = snapPos(mousePos + dragNodeOffset);
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
			nodes.erase(nodes.begin() + selectedNode);
			selectedNode = -1;
		} break;
		case sf::Keyboard::Backspace:
		{
			if (!inFocus) { break; }
			if (selectedNode == -1) { break; }
			nodes.erase(nodes.begin() + selectedNode);
			selectedNode = -1;
		} break;
		default:
			break;
		}
	} break;
	}

	return false;
}