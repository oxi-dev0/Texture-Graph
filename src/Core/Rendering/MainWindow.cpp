#include "MainWindow.h"

void MainWindow::MenuBar() {
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Bundle"))
            {
                GraphEditorView& c = *graphView;
                std::function<void(void)> f = [&c]() {
                    Bundle::Serialization::NewBundle();
                    Globals::currentGraph = "";
                    c.Clear();
                };
                if (Bundle::Serialization::dirty && Globals::currentBundle != "") {
                    Bundle::Serialization::SafeNew(f);
                }
                else {
                    f();
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Open Bundle")) {
                GraphEditorView& c = *graphView;
                std::function<void(void)> f = [&c]() {
                    if (Bundle::Serialization::AskLoadBundleFromFile()) {
                        c.Clear();
                        Globals::currentGraph = "";
                    }
                };
                if (Bundle::Serialization::dirty && Globals::currentBundle != "") {
                    Bundle::Serialization::SafeNew(f);
                }
                else {
                    f();
                }
            }

            if (ImGui::MenuItem("Save Bundle As..")) {
                Bundle::Serialization::AskSaveBundleToFile();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Save Graph")) {
                Graph::Serialization::SaveGraphToFile(*graphView, "temp/bundle/" + Globals::currentGraph + ".graph");
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Quit")) {
                exit = true;
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(Ctrl+Q)");

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            //ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
            //ImGui::PopItemFlag();

            if (ImGui::MenuItem("Fullscreen", "", &fullscreen)) {
                SetFullscreen(fullscreen);
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(F11)");
            ImGui::SameLine();
            ImGui::Text("    "); // Make space for menu item checkmark

            ImGui::Separator();

            if (ImGui::BeginMenu("Views")) {
                for (SubWindow* window : *views) {
                    ImGui::MenuItem(window->name.c_str(), "", &window->enabled);
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            ImGui::MenuItem("ImGui Demo", "", &demoOpen);
            if (ImGui::BeginMenu("Layout")) {
                if (ImGui::BeginMenu("Debug")) {
                    if (ImGui::MenuItem("Save Default Layout")) {
                        ImGui::SaveIniSettingsToDisk("resources/defaultlayout.ini");
                        LOG_INFO("Successfully saved layout to 'resources/defaultlayout.ini");
                    }
                    if (ImGui::MenuItem("Load Default Layout")) {
                        ImGui::LoadIniSettingsFromDisk("resources/defaultlayout.ini");
                        LOG_INFO("Successfully loaded layout from 'resources/defaultlayout.ini'");
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Engine")) {
            if (ImGui::MenuItem("Recompile Node Library")) {
                LibraryManager::LoadNodeLibrary();
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(CTRL+R)");

            if (ImGui::MenuItem("Regenerate Thumbnails")) {
                LibraryManager::LoadNodeLibrary(true);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Evaluate Nodes")) {
                auto evalThread = std::thread(&GraphEditorView::EvaluateNodes, graphView);
                evalThread.detach();
            }
            
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void MainWindow::InfoBar(float height) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - height));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, height));
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("INFOBAR", NULL, window_flags);

    std::stringstream mouseStream;
    sf::Vector2f mousePos = Utility::Mapping::pixelToWindowLoc(sf::Mouse::getPosition(), window);
    mouseStream << "Mouse: (" << mousePos.x << ", " << mousePos.y << ")";
    ImGui::Text(mouseStream.str().c_str());
     
    ImGui::SameLine(0.0f, 25.0f);

    std::stringstream fpsStream;
    fpsStream << "FPS: " << (round(1.0f/prevDeltaTime.asSeconds()));
    ImGui::Text(fpsStream.str().c_str());

    ImGui::End();
    ImGui::PopStyleVar(2);
}

void MainWindow::Dockspace()
{
    float infobarHeight = 25.0f;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - infobarHeight));
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 0
        | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Master DockSpace", NULL, window_flags);
    ImGuiID dockMain = ImGui::GetID("MyDockspace");
    ImGui::PopStyleVar(3);

    MenuBar();
    InfoBar(infobarHeight);

    ImGui::DockSpace(dockMain, ImVec2(0,0), 0 | ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
    //ImGui::PopStyleVar(3);
}

MainWindow::MainWindow() {
    fullscreen = false;
    demoOpen = false;
    exit = false;
    handlingView = nullptr;
    views = nullptr;
}

MainWindow::~MainWindow() {
    //delete[] buf;
}

MainWindow::MainWindow(std::string windowTitle) {
    exit = false;
    Init(windowTitle);
}

void MainWindow::Init(std::string windowTitle) {
    title = windowTitle;
    fullscreen = false;
    demoOpen = false;
    handlingView = nullptr;

    // INIT WINDOW
    window.create(sf::VideoMode(1920, 1080), windowTitle.c_str()); // Main editor window
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window, false); // Init IMGUI with main window

    LOG_INFO("Initialised Window");



    // IMGUI FLAGS
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // IMGUI STYLE AND THEME
    ImGui::StyleColorsDark();
    EditorColourScheme::SetColors(0x3f3f3fff, 0xffffffff, 0x70707059, 0x515151ff, 0xa0a0a059);
    EditorColourScheme::ApplyTheme();
    ImGui::GetStyle().WindowBorderSize = 0.0f;

    LOG_TRACE("Applied theme 'Greyscale'");

    // IMGUI FONT
    io.Fonts->Clear();
    ImFont* robotoFont = io.Fonts->AddFontFromFileTTF("resources/NotoSans/NotoSans-Regular.ttf", 20.0f);
    io.FontDefault = robotoFont;
    ImGui::SFML::UpdateFontTexture();

    LOG_TRACE("Loaded font 'NotoSans-Regular.ttf'");
}

void MainWindow::SetFullscreen(bool newFullscreen) {
    fullscreen = newFullscreen;
    if (fullscreen) {
        prevSize = window.getSize();
        prevPos = window.getPosition();
        window.create(sf::VideoMode::getFullscreenModes()[0], title.c_str(), sf::Style::Fullscreen); // Switch to fullscreen
        window.setVerticalSyncEnabled(true);
    }
    else {
        window.create(sf::VideoMode(prevSize.x, prevSize.y), title.c_str()); // Return to old view
        window.setVerticalSyncEnabled(true);
        window.setPosition(prevPos);
    }
}

// 0: Fine, 1: Exit
int MainWindow::Update() {
    sf::Event event;
    bool br = false;
    while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(window, event);
        if (event.type == sf::Event::Closed) {
            br = true;
        }
        if (event.type == sf::Event::KeyPressed) {
            Keybinds::KeybindEvent keyEvent = Keybinds::ProcessEvent(event);
            switch (keyEvent) {
            case Keybinds::KeybindEvent_FullscreenToggle:
                SetFullscreen(!fullscreen);
                break;
            case Keybinds::KeybindEvent_Exit:
                exit = true;
                break;
            case Keybinds::KeybindEvent_RecompileNodes:
                LibraryManager::LoadNodeLibrary();
                break;
            case Keybinds::KeybindEvent_Copy:
            {
                if (graphView->multiSelectNodes.size() == 0) { break; }

                std::string data;
                Graph::Serialization::SaveNodesToData(*graphView, graphView->multiSelectNodes, data);
                data = "TEXTUREGRAPH!" + data;
                clip::set_text(data);
                LOG_INFO("Copied nodes to clipboard");
            }   break;
            case Keybinds::KeybindEvent_Paste:
            {
                std::string pasteData;
                clip::get_text(pasteData);
                auto data = Utility::String::split(pasteData, '!');
                if (data[0] != "TEXTUREGRAPH") { break; }

                int oldLastIndex = graphView->nodes.size();
                sf::Vector2f offset;
                if (graphView->nodes.size() > 0) {
                    for (auto& node : graphView->nodes) {
                        offset.x = std::max(offset.x, node->calcBounds().left + node->calcBounds().width);
                    }
                    offset.x + 100;
                }
                Graph::Serialization::AppendNodesFromData(*graphView, data[1]);
                graphView->multiSelectNodes.clear();

                for (int x = oldLastIndex; x < graphView->nodes.size(); x++) {
                    graphView->nodes[x]->nodePos = graphView->snapPos(graphView->nodes[x]->nodePos + offset);
                    graphView->multiSelectNodes.push_back(x);
                }

                LOG_INFO("Appended nodes from clipboard");
            }   break;
            default:
                break;
            }
        }

        for (auto& view : *views) {
            if (!view->visible) { continue; }

            auto transformed = Utility::Mapping::pixelToWindowLoc(sf::Mouse::getPosition(), window);
            if ((transformed.x >= view->prevPos.x && transformed.x <= view->prevPos.x + view->prevSize.x && transformed.y >= view->prevPos.y && transformed.y <= view->prevPos.y + view->prevSize.y) || handlingView == view) {
                if (view->ProcessEvent(event)) {
                    //LOG_TRACE("Handle event from Window '{0}'", view->name);
                    if (handlingView == nullptr) {
                        handlingView = view;
                        //LOG_TRACE("Window '{0}' gained handle", view->name);
                    }
                    else {
                        if (handlingView == view) {
                            handlingView = nullptr;
                            //LOG_TRACE("Window '{0}' lost handle", view->name);
                        }
                    }
                    
                };
            }
        }
    }
    if (br || exit) {
        return 1;
    }

    return 0;
}

void MainWindow::OpenPopup(std::string id) {
    nextOpenId = id;
    openPopup = true;
    buf[0] = '\0';
}

void MainWindow::Popups() {
    if (openPopup) {
        ImGui::OpenPopup(nextOpenId.c_str());
        openPopup = false;
    }

    if (ImGui::BeginPopup("##BrowserNew")) {
        if (ImGui::Selectable("Bundle...", false, 0, ImVec2(70,30))) {
            GraphEditorView& c = *graphView;
            std::function<void(void)> f = [&c]() {
                Bundle::Serialization::NewBundle();
                c.Clear();

            };
            if (Bundle::Serialization::dirty && Globals::currentBundle != "") {
                Bundle::Serialization::SafeNew(f);
            }
            else {
                f();
            }
            ImGui::CloseCurrentPopup(); 
        }
        if (ImGui::Selectable("Graph...", false, 0, ImVec2(70,30))) { OpenPopup("New Graph"); ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    Graph::Serialization::GraphNewPopup(*graphView, buf);
    Graph::Serialization::ClearPromptPopup(*graphView);
    Bundle::Serialization::BundlePromptPopup();
}

void MainWindow::BeginRender() {
    prevDeltaTime = deltaClock.restart();
    ImGui::SFML::Update(window, prevDeltaTime);
    ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    window.clear(sf::Color((sf::Uint8)(bgColor.x * 100.0f), (sf::Uint8)(bgColor.y * 100.0f), (sf::Uint8)(bgColor.z * 100.0f), (sf::Uint8)255));

    Dockspace();
    Popups();

    if (demoOpen) {
        ImGui::ShowDemoWindow(&demoOpen);
    }
}

void MainWindow::EndRender() {
    ImGui::SFML::Render(window);
    window.display();
}