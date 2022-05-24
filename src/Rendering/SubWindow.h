#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include <imgui_internal.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "../Utils/Log.h"
#include "ColourSchemes.h"

#include <string>
#include <sstream>

class SubWindow
{
protected:
	sf::RenderWindow& main;

public:
	bool visible;
	bool enabled;
	ImGuiWindowFlags flags;
	std::string name;

	ImVec2 prevSize;
	ImVec2 prevPos;
	bool updated;

protected:
	ImVec2 GetPos(); // Must be called in render context

protected:
	virtual void ImGuiRender(bool updated);

public:
	SubWindow(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_);
	~SubWindow();

	virtual bool ProcessEvent(sf::Event& event); // Returns true if wants to toggle handling
	virtual void Render();
};

