#pragma once

#include "../SFMLWindow.h"
#include "GraphEditorView.h"
#include "nfd.h"

class Texture2DView : public SFMLWindow
{
private:
	sf::Vector2f oldPos;
	bool moving = false;
	bool inFocus = false;
	sf::FloatRect initialRect;
public:
	GraphEditorView* focusedGraph;
	int selectedNode;

protected:
	virtual void ComponentRender();
	virtual void IMGUIRender();

	virtual void InfoBarData();
	virtual void ToolBarButtons();

	virtual sf::Vector2f pixelToGraph(sf::Vector2i pixel);
public:
	Texture2DView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);

	virtual bool ProcessEvent(sf::Event& event);

};

