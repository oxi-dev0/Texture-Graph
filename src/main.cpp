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

#include "Rendering/SubWindow.h"
#include "Views/GraphEditorView.h"

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

enum WindowType {
    Base,
    GraphEditor
};

typedef std::tuple<std::string, WindowType, ImGuiWindowFlags> optionalWindow;
std::vector<optionalWindow> optionalViews = {
    optionalWindow("Graph Editor", WindowType::GraphEditor, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
    optionalWindow("Graph Editor 2", WindowType::GraphEditor, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
};

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

    //sf::RenderTexture newT; // trying to fix unique ptrs being deleted

    std::vector<SubWindow*> windows;
    std::vector<sf::RenderTexture*> textures;
    for (auto& view : optionalViews) {
        std::string name = std::get<0>(view);
        WindowType type = std::get<1>(view);
        ImGuiWindowFlags flags = std::get<2>(view);

        switch (type) {
        case WindowType::Base:
        {
            windows.push_back(new SubWindow(primaryWindow.window, name, flags));
        }   break;
        case WindowType::GraphEditor:
        {
            sf::RenderTexture* tex = new sf::RenderTexture();
            textures.push_back(tex);
            windows.push_back(new GraphEditorView(primaryWindow.window, *tex, name, flags));
        }   break;
        default:
            break;
        }
    } // Generates the toggle windows, such as the graph editor and inspector

    primaryWindow.views = &windows;
    
    while (primaryWindow.window.isOpen())
    {
        if (primaryWindow.Update() == 1) {
            break; // Exit app
        };

        primaryWindow.BeginRender();

        for (SubWindow* window : windows) {
            if (window->enabled) {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window->visible = ImGui::Begin(window->name.c_str(), &window->enabled, window->flags);
                if (window->visible)
                    window->Render();
                ImGui::End();
                ImGui::PopStyleVar(2);
            }
        }

        ImGui::Begin("Other");
        ImGui::Button("WOW");
        ImGui::End();

        primaryWindow.EndRender();
    }

    for (auto* tex : textures) {
        delete tex;
    }

    LOG_INFO("Stopping Engine");
    ImGui::SFML::Shutdown();

    return 0;
}
