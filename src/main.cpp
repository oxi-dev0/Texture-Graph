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
#include "Core/Bundle/BundleSerializer.h"
#include "Core/Bundle/ResourceManager.h"

#include "Core/Library/LibraryManager.h"

#include "Core/Globals.h"

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

typedef std::tuple<std::string, WindowType, ImGuiWindowFlags> viewDef;
std::vector<viewDef> fixedViews = {
    viewDef("Graph Editor", WindowType::GraphEditor, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
    viewDef("Library", WindowType::LibraryView, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse),
    viewDef("Inspector", WindowType::Inspector, 0),
    viewDef("Browser", WindowType::Browser, 0),
    viewDef("Texture Preview", WindowType::TextureView, 0 | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)
};

MainWindow* primaryWindow = nullptr;

std::vector<sf::RenderTexture*> textures;
void OnExit() {
    LOG_INFO("Stopping Engine");
    ImGui::SFML::Shutdown();
    NFD_Quit();
    ImageCache::Exit();

    LibraryManager::Clear();
    Bundle::Resources::ClearPreviews();
    primaryWindow->graphView->Clear();

    std::filesystem::remove_all("temp");

    delete primaryWindow;

    RenderingGlobals::font.~Font();
    for (auto* tex : textures) {
        if (tex == nullptr) { continue; }
        delete tex;
    }
}

void SaveGraph() {
    if (Globals::currentGraph == "") { return; }
    Graph::Serialization::SaveGraphToFile(*primaryWindow->graphView, "temp/bundle/" + Globals::currentGraph + ".graph");
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

    LOG_INFO("Starting Engine");
    LOG_INFO("");

    RenderingGlobals::Init();
    NFD_Init();
    ImageCache::Init();
    Bundle::Resources::Init();
    Globals::Init();
    Bundle::Serialization::NewBundle();

    primaryWindow = new MainWindow("Texture Graph");

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = "resources/imgui.ini";

    ImGui::LoadIniSettingsFromDisk("resources/defaultlayout.ini");
    LOG_INFO("Successfully loaded layout from 'resources/defaultlayout.ini'");

    MainWindow& mC = *primaryWindow;
    std::function<void(std::string)> f = [&mC](std::string id) {
        return mC.OpenPopup(id);
    };
    Graph::Serialization::openPopup = f;
    Bundle::Serialization::openPopup = f;

    std::vector<SubWindow*> views;
    for (auto& view : fixedViews) {
        std::string name = std::get<0>(view);
        WindowType type = std::get<1>(view);
        ImGuiWindowFlags flags = std::get<2>(view);

        switch (type) {
        case WindowType::Base:
        {
            views.push_back(new SubWindow(primaryWindow->window, name, flags));
        }   break;
        case WindowType::GraphEditor:
        {
            sf::RenderTexture* tex = new sf::RenderTexture();
            textures.push_back(tex);
            auto* newView = new GraphEditorView(primaryWindow->window, *tex, name, flags);
            views.push_back(newView);
            primaryWindow->graphView = newView;
            newView->saveCallback = SaveGraph;

        }   break;
        case WindowType::LibraryView:
        {
            auto* newView = new LibraryEditorView(primaryWindow->window, name, flags);
            views.push_back(newView);
            LibraryManager::RegisterLoadCallback(std::bind(&LibraryEditorView::LoadNodes, newView));
        }   break;
        case WindowType::Inspector:
        {
            auto* newView = new InspectorView(primaryWindow->window, name, flags);
            newView->SetGraph(primaryWindow->graphView);
            views.push_back(newView);
        }   break;
        case WindowType::TextureView:
        {
            sf::RenderTexture* tex = new sf::RenderTexture();
            textures.push_back(tex);
            auto* newView = new Texture2DView(primaryWindow->window, *tex, name, flags);
            views.push_back(newView);
            newView->focusedGraph = primaryWindow->graphView;
        }   break;
        case WindowType::Browser:
        {
            auto* newView = new BrowserView(primaryWindow->window, name, flags);
            views.push_back(newView);
            newView->focusedGraphView = primaryWindow->graphView;

            newView->RegisterPopupCallback(f);
            primaryWindow->mainBrowserView = newView;

        }   break;
        default:
            break;
        }
    } // Generates the toggle windows, such as the graph editor and inspector



    LOG_INFO("Successfully generated optional views");

    LOG_INFO("");
    LibraryManager::LoadNodeLibrary();
    LOG_INFO("");

    primaryWindow->views = &views;
    
    while (primaryWindow->window.isOpen())
    {
        if (primaryWindow->Update() == 1) {
            break; // Exit app
        };

        primaryWindow->BeginRender();

        for (SubWindow* view : views) {
            if (view->enabled) {
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                view->visible = ImGui::Begin(view->name.c_str(), &view->enabled, view->flags);
                if (view->visible)
                    view->Render();
                ImGui::End();
                ImGui::PopStyleVar(2);
            }
        }

        primaryWindow->EndRender();
    }

    return 0;
}
