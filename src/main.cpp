#pragma warning( disable : 4244 4267 4996 )

#include <iostream>
#include <sstream>
#include <locale>
#include <filesystem>
#include <functional>

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
#include "Core/Rendering/RenderingGlobals.h"
#include "Core/Rendering/MainWindow.h"
#include "Core/Rendering/CachedImages.h"

#include "Core/Rendering/SubWindow.h"
#include "Core/Rendering/Views/GraphEditorView.h"
#include "Core/Rendering/Views/LibraryView.h"
#include "Core/Rendering/Views/InspectorView.h"
#include "Core/Rendering/Views/Texture2DView.h"
#include "Core/Rendering/Views/BrowserView.h"

#include "Core/Bundle/Node/GraphNode.h"
#include "Core/Bundle/GraphSerializer.h"
#include "Core/Bundle/ResourceManager.h"

#include "Core/Library/LibraryManager.h"

#include "Core/Lua/LuaManager.h"

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

enum WindowType {
    Base,
    GraphEditor,
    LibraryView,
    TextureView,
    Browser,
    Inspector
};

typedef std::tuple<std::string, WindowType, ImGuiWindowFlags> optionalWindow;
std::vector<optionalWindow> optionalViews = {
    optionalWindow("Graph Editor", WindowType::GraphEditor, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
    optionalWindow("Library", WindowType::LibraryView, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
    optionalWindow("Inspector", WindowType::Inspector, 0),
    optionalWindow("Browser", WindowType::Browser, 0),
    optionalWindow("Texture View", WindowType::TextureView, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)
    //optionalWindow("Graph Editor 2", WindowType::GraphEditor, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
};

std::vector<sf::RenderTexture*> textures;
void OnExit() {
    LOG_INFO("Stopping Engine");
    ImGui::SFML::Shutdown();
    NFD_Quit();
    LuaManager::Exit();
    ImageCache::Exit();

    RenderingGlobals::font.~Font();
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

    LOG_INFO("Starting Engine");
    LOG_INFO("");

    RenderingGlobals::Init();
    NFD_Init();
    LuaManager::Init();
    ImageCache::Init();
    Graph::Resources::Init();

    MainWindow primaryWindow("Texture Graph");

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "resources/imgui.ini";

    ImGui::LoadIniSettingsFromDisk("resources/defaultlayout.ini");
    LOG_INFO("Successfully loaded layout from 'resources/defaultlayout.ini'");
      
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
            auto* newView = new GraphEditorView(primaryWindow.window, *tex, name, flags);
            windows.push_back(newView);
            primaryWindow.focusedGraphView = newView;

        }   break;
        case WindowType::LibraryView:
        {
            auto* newView = new LibraryEditorView(primaryWindow.window, name, flags);
            windows.push_back(newView);
            LibraryManager::RegisterLoadCallback(std::bind(&LibraryEditorView::LoadNodes, newView));
        }   break;
        case WindowType::Inspector:
        {
            auto* newView = new InspectorView(primaryWindow.window, name, flags);
            newView->SetGraph(primaryWindow.focusedGraphView);
            windows.push_back(newView);
        }   break;
        case WindowType::TextureView:
        {
            sf::RenderTexture* tex = new sf::RenderTexture();
            textures.push_back(tex);
            auto* newView = new Texture2DView(primaryWindow.window, *tex, name, flags);
            windows.push_back(newView);
            newView->focusedGraph = primaryWindow.focusedGraphView;
        }   break;
        case WindowType::Browser:
        {
            auto* newView = new BrowserView(primaryWindow.window, name, flags);
            windows.push_back(newView);
            newView->focusedGraphView = primaryWindow.focusedGraphView;

            std::function<void(std::string)> f = [&primaryWindow](std::string id) {
                return primaryWindow.OpenPopup(id);
            };
            newView->RegisterPopupCallback(f);
            primaryWindow.mainBrowserView = newView;

        }   break;
        default:
            break;
        }
    } // Generates the toggle windows, such as the graph editor and inspector

    LOG_INFO("Successfully generated optional views");

    LOG_INFO("");
    LibraryManager::LoadNodeLibrary();
    LOG_INFO("");

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

        primaryWindow.EndRender();
    }

    return 0;
}
