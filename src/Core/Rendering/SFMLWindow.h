#pragma once

#include "../Rendering/SubWindow.h"

class SFMLWindow : public SubWindow
{
protected:
	sf::RenderTexture& rt;
	sf::View& view;

	sf::Color bgCol;

	float zoom;
	float prevZoom;
	sf::Vector2f vcenter; // events have to be implemented in child window

protected:
	virtual void InfoBar(float height);
	virtual void ImGuiRender();

	void SetBGColor(ImVec4 color);

public:

	SFMLWindow(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);

	virtual bool ProcessEvent(sf::Event& event);
	virtual void Render();
};

