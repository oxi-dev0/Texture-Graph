#pragma warning( disable : 4244 4267 4996 )

#include <iostream>
#include <sstream>
#include <locale>
#include <filesystem>

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "Utils/Log.h"
#include "Utils/Timer.h"
#include <imgui_internal.h>

#include "ImGui/ColourSchemes.h"

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

bool fullscreen = false;
bool demoOpen = false;

void UI_Dockspace()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y));
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Master DockSpace", NULL, window_flags);
    ImGuiID dockMain = ImGui::GetID("MyDockspace");
    ImGui::PopStyleVar(3);


    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New"))
            {
                //Do something
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
            ImGui::MenuItem("Fullscreen", "", &fullscreen);
            ImGui::PopItemFlag();

            ImGui::Separator();

            ImGui::MenuItem("ImGui Demo", "", &demoOpen);

            if (ImGui::BeginMenu("Layout")) {
                if (ImGui::BeginMenu("Debug")) {
                    if (ImGui::MenuItem("Save Default Layout"))
                        ImGui::SaveIniSettingsToDisk("resources/defaultlayout.ini");
                    if (ImGui::MenuItem("Load Default Layout"))
                        ImGui::LoadIniSettingsFromDisk("resources/defaultlayout.ini");

                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::DockSpace(dockMain);
    ImGui::End();
    //ImGui::PopStyleVar(3);
}

int main(int argc, char** argv)
{
#ifdef DIST
    FreeConsole();
#else
    Utility::Log::Init();
#endif

    LOG_INFO("Started Engine");


    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Texture Graph"); // Main editor window
    ImGui::SFML::Init(window, false);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    EditorColourScheme::SetColors(0x3f3f3fff, 0xffffffff, 0x70707059, 0x515151ff, 0xa0a0a059);
    EditorColourScheme::ApplyTheme();
    
    io.Fonts->Clear();
    ImFont* robotoFont = io.Fonts->AddFontFromFileTTF("resources/NotoSans/NotoSans-Regular.ttf", 20.0f);
    io.FontDefault = robotoFont;
    
    ImGui::SFML::UpdateFontTexture();

    sf::RenderTexture nodeGraph;
    if (!nodeGraph.create(640, 420, settings)) {
        LOG_CRITICAL("Could not initialise the graph texture.");
        return 0;
    }

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::RectangleShape shape2(sf::Vector2f(50.0f, 20.0f));
    shape2.setFillColor(sf::Color::Red);
    
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        bool br = false;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) {
                br = true;
            }
        }
        if (br) {
            break;
        }

        nodeGraph.clear();
        nodeGraph.draw(shape2);
        nodeGraph.display();

        ImGui::SFML::Update(window, deltaClock.restart());

        UI_Dockspace();

        if (demoOpen) {
            ImGui::ShowDemoWindow(&demoOpen);
        }

        ImGui::Begin("Graph Editor");
        ImGui::Image(nodeGraph, sf::Vector2f(nodeGraph.getSize()));
        ImGui::End();

        ImGui::Begin("Other");
        ImGui::Button("UWU");
        ImGui::End();

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    LOG_INFO("Shutting Down...");
    ImGui::SFML::Shutdown();

    return 0;
}
