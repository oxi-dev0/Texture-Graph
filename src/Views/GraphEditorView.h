#pragma once

#include "../Rendering/SFMLWindow.h"

class GraphEditorView : public SFMLWindow
{
private:
	sf::Vector2f oldPos;
	bool moving = false;
	sf::VertexArray lines;

private:
	void Grid(bool updated);
protected:
	virtual void ImGuiRender(bool updated);
public:
	GraphEditorView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);

	virtual bool ProcessEvent(sf::Event& event); 

};

