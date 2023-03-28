#pragma once

#include "../SFMLWindow.h"
#include "../../Bundle/Node/GraphNode.h"
#include "../../Bundle/Node/ImageNode.h"
#include "../../Library/LibraryManager.h"
#include "../../Bundle/ResourceManager.h"

#include "../../Globals.h"

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

	bool cutting = false;
	sf::Vector2f oldCutPos;

public:
	std::vector<GraphNode*> nodes;
	int selectedNode;
	int dragNodeRef;
	std::vector<int> multiSelectNodes;

	//std::atomic<int> currentThreadCount;
	//std::vector<std::thread> evalThreads;
	int maxWorkerCount;
	std::atomic<int> workerPoolAvail;

	int currentLineFromNode;
	int currentLineFromPin;

	bool cyclicalError=false;
	int cyclicalNode;
	int cyclicalPin;

	std::function<void(void)> saveCallback;

	bool dirty;

	sf::Vector2i texSize;

private:
	void Grid();
	bool CyclicalRec(int currentNode, std::vector<int> stack, int prevPin=-1, int prevNode=-1);
	void TopologicalSortRec(int currentNode, std::vector<int>& ordering);

	//void EvalNodeThread(int nodeIndex);
	void NodeWorker(int nodeIndex);
	void NodeScheduler(std::vector<int> evalPool);
protected:
	virtual void ComponentRender();
	virtual void IMGUIRender();
	virtual void InfoBarData();

	virtual void ToolBarButtons();

	virtual void RenderLine(sf::Vector2f start, sf::Vector2f end, sf::Color startCol, sf::Color endCol, float width);

public:
	virtual sf::Vector2f snapPos(sf::Vector2f pos);
	virtual sf::Vector2f pixelToGraph(sf::Vector2i pixel);
public:
	GraphEditorView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);
	void Clear();

	template <class T>
	inline static T* copyNode(T* node) {
		if (dynamic_cast<ImageNode*>(node) != nullptr) {
			return new ImageNode(node);
		}
		else {
			return new GraphNode(node);
		}
	}

	bool CheckCyclical();
	std::vector<int> TopologicalSort();

	void UpdateTexSize(sf::Vector2i size);
	void DeleteNode(int index);

	virtual bool ProcessEvent(sf::Event& event); 
	void EvaluateNodes();
};

