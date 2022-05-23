#pragma warning( disable : 4244 4267 4996 )

#include <iostream>
#include <sstream>
#include <locale>
#include <filesystem>

#include <dwmapi.h>

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "Utils/Log.h"
#include "Utils/Timer.h"
#include <imgui_internal.h>

#include "Rendering/ColourSchemes.h"
#include "Rendering/MainWindow.h"

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING


int main(int argc, char** argv)
{
#ifdef DIST
    FreeConsole();
#else
    Utility::Log::Init();
#endif

    LOG_INFO("Starting Engine\n");

    MainWindow primaryWindow("Texture Graph");

    ImGui::LoadIniSettingsFromDisk("resources/defaultlayout.ini");
    LOG_INFO("Successfully loaded layout from 'resources/defaultlayout.ini'");

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderTexture nodeGraph;
    if (!nodeGraph.create(640, 420, settings)) {
        LOG_CRITICAL("Could not initialise the graph texture.");
        return 0;
    }

    sf::RectangleShape shape2(sf::Vector2f(50.0f, 20.0f));
    shape2.setFillColor(sf::Color::Red);

    
    while (primaryWindow.window.isOpen())
    {
        if (primaryWindow.Update() == 1) {
            break; // Exit app
        };

        nodeGraph.clear();
        nodeGraph.draw(shape2);
        nodeGraph.display();

        primaryWindow.BeginRender();

        ImGui::Begin("Graph Editor");
        ImGui::Image(nodeGraph, sf::Vector2f(nodeGraph.getSize()));
        ImGui::End();

        ImGui::Begin("Other");
        ImGui::Button("UWU");
        ImGui::End();

        primaryWindow.EndRender();
    }

    LOG_INFO("Stopping Engine");
    ImGui::SFML::Shutdown();

    return 0;
}
