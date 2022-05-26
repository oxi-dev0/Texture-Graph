#pragma once

#include "../SFMLWindow.h"
#include "../../Graph/Node/GraphNode.h"

class GraphEditorView : public SFMLWindow
{
private:
	sf::Vector2f oldPos;
	bool moving = false;
	sf::VertexArray lines;

	std::vector<GraphNode> nodes;

private:
	void Grid();
protected:
	virtual void ComponentRender();
public:
	GraphEditorView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);

	virtual bool ProcessEvent(sf::Event& event); 

	void AddNode(std::string nodeClass);
};

