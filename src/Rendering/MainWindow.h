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

class MainWindow
{
private:
	std::string title;
	bool fullscreen;
	bool demoOpen;

	sf::Time prevDeltaTime;

	sf::Vector2u prevSize;
	sf::Vector2i prevPos;

	sf::Clock deltaClock;

public:
	sf::RenderWindow window;

private:
	void MenuBar();
	void InfoBar(float height = 20.0f);
	void Dockspace();

public:
	MainWindow();
	MainWindow(std::string windowTitle);

	void Init(std::string windowTitle);

	void SetFullscreen(bool newFullscreen);

	int Update(); // 0: Fine, 1: Exit
	void BeginRender();
	void EndRender();
};

