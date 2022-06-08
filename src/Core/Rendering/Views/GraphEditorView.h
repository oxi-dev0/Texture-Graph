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
	bool draggingNodeMultiple = false;
	bool inFocus = false;
	bool draggingLine = false;
	sf::Vector2f dragNodeOffset;
	sf::VertexArray lines;
	sf::FloatRect initialRect;
	
	sf::Vector2f selectBoxStart;
	sf::Vector2f selectBoxEnd;

public:
	std::vector<GraphNode*> nodes;
	std::vector<std::thread> evalThreads;
	int selectedNode;
	int dragNodeRef;
	std::vector<int> multiSelectNodes;

	int currentLineFromNode;
	int currentLineFromPin;

	bool cyclicalError=false;
	int cyclicalNode;
	int cyclicalPin;

	sf::Vector2i texSize;

private:
	void Grid();
	bool CyclicalRec(int currentNode, std::vector<int> stack, int prevPin=-1, int prevNode=-1);
	void TopologicalSortRec(int currentNode, std::vector<int>& ordering);
	void EvalNodeThread(int nodeIndex);
protected:
	virtual void ComponentRender();
	virtual void IMGUIRender();

	virtual void ToolBarButtons();

	virtual void RenderLine(sf::Vector2f start, sf::Vector2f end, sf::Color startCol, sf::Color endCol, float width);

public:
	virtual sf::Vector2f snapPos(sf::Vector2f pos);
	virtual sf::Vector2f pixelToGraph(sf::Vector2i pixel);
public:
	GraphEditorView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);
	void Clear();

	bool CheckCyclical();
	std::vector<int> TopologicalSort();

	void UpdateTexSize(sf::Vector2i size);
	void DeleteNode(int index);

	virtual bool ProcessEvent(sf::Event& event); 
	void EvaluateNodes();
};

