#pragma once

#include "../SFMLWindow.h"
#include "../../Graph/Node/GraphNode.h"
#include "../../Library/LibraryManager.h"

class GraphEditorView : public SFMLWindow
{
private:
	sf::Vector2f oldPos;
	bool moving = false;
	bool draggingNode = false;
	bool inFocus = false;
	bool draggingLine = false;
	sf::Vector2f dragNodeOffset;
	sf::VertexArray lines;
	sf::FloatRect initialRect;
public:
	std::vector<GraphNode*> nodes;
	std::vector<std::thread> evalThreads;
	int selectedNode;

	int currentLineFromNode;
	int currentLineFromPin;

	sf::Vector2i texSize;

private:
	void Grid();
protected:
	virtual void ComponentRender();
	virtual void IMGUIRender();
	virtual void RenderLine(sf::Vector2f start, sf::Vector2f end, sf::Color startCol, sf::Color endCol, float width);

	virtual sf::Vector2f snapPos(sf::Vector2f pos);
	virtual sf::Vector2f pixelToGraph(sf::Vector2i pixel);
public:
	GraphEditorView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);
	void Clear();

	void UpdateTexSize(sf::Vector2i size);
	void DeleteNode(int index);

	virtual bool ProcessEvent(sf::Event& event); 
	void EvaluateNodes();
};

