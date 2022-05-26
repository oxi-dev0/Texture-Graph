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

#include "Core/Utils/Log.h"
#include "Core/Utils/Timer.h"
#include <imgui_internal.h>

#include "Core/Rendering/ColourSchemes.h"
#include "Core/Rendering/MainWindow.h"

#include "Core/Rendering/SubWindow.h"
#include "Core/Rendering/Views/GraphEditorView.h"

#include "Core/Graph/Node/GraphNode.h"

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

enum WindowType {
    Base,
    GraphEditor
};

typedef std::tuple<std::string, WindowType, ImGuiWindowFlags> optionalWindow;
std::vector<optionalWindow> optionalViews = {
    optionalWindow("Graph Editor", WindowType::GraphEditor, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
    //optionalWindow("Graph Editor 2", WindowType::GraphEditor, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
};

std::vector<sf::RenderTexture*> textures;
void OnExit() {
    for (auto* tex : textures) {
        if (tex == nullptr) { continue; }
        delete tex;
    }
}


int main(int argc, char** argv)
{
#ifdef DIST
    FreeConsole();
#else
    Utility::Log::Init(); 
#endif

    // Exit handler
    std::atexit(OnExit);
    std::at_quick_exit(OnExit);
    set_terminate(OnExit);
    set_unexpected(OnExit);

    LOG_INFO("Starting Engine\n");

    MainWindow primaryWindow("Texture Graph");

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "resources/imgui.ini";

    ImGui::LoadIniSettingsFromDisk("resources/defaultlayout.ini");
    LOG_INFO("Successfully loaded layout from 'resources/defaultlayout.ini'");

    LOG_INFO("TESTING TGNF LOADER");
    Utility::Timer tmr;
    GraphNode newNode = GraphNode::LoadFromTGNF("library/Nodes/SimpleSC.tgnode");
    LOG_INFO("Successfully compiled {0} in {1}s", "library/Nodes/SimpleSC.tgnode", tmr.Elapsed());
    for (int i = 0; i < 10000; i++) {
        tmr.Reset();
        GraphNode newNode2 = GraphNode::LoadFromTGNF("library/Nodes/TestOverflow.tgnode");
        LOG_INFO("Successfully compiled {0} in {1}s", "library/Nodes/TestOverflow.tgnode", tmr.Elapsed());
    }
      
    //sf::RenderTexture newT; // trying to fix unique ptrs being deleted

    std::vector<SubWindow*> windows;
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

    LOG_INFO("Stopping Engine");
    ImGui::SFML::Shutdown();

    return 0;
}
