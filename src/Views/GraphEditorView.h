#pragma once

#include "../Rendering/SFMLWindow.h"

class GraphEditorView : public SFMLWindow
{
private:
	sf::Vector2f oldPos;
	bool moving = false;

private:
	void Grid();
protected:
	virtual void ImGuiRender();
public:
	GraphEditorView(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);

	virtual bool ProcessEvent(sf::Event& event); 

};

