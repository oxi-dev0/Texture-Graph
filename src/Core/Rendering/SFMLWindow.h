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
	sf::Vector2f prevVCenter;

protected:
	virtual void InfoBarData();
	virtual void ToolBarButtons();

	virtual void ComponentRender();
	virtual void IMGUIRender();

	void SetBGColor(ImVec4 color);

public:

	SFMLWindow(sf::RenderWindow& main_, sf::RenderTexture& rt_, std::string name_, ImGuiWindowFlags flags_);

	virtual bool ProcessEvent(sf::Event& event);
	virtual void Render();
};

