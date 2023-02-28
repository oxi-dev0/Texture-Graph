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

void SetImguiStyle()
{
    // Photoshop style by Derydoca from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 4.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 2.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 2.0f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 13.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabMinSize = 7.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.TabBorderSize = 1.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.2627451121807098f, 0.2627451121807098f, 0.2627451121807098f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2745098173618317f, 0.2745098173618317f, 0.2745098173618317f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2980392277240753f, 0.2980392277240753f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.3910000026226044f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.2627451121807098f, 0.2627451121807098f, 0.2627451121807098f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.6700000166893005f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.3490196168422699f, 0.3490196168422699f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.5843137502670288f, 0.5843137502670288f, 0.5843137502670288f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.96f, 0.62f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5860000252723694f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5860000252723694f);
}

void MainWindow::Init(std::string windowTitle) {
    title = windowTitle;
    fullscreen = false;
    demoOpen = false;
    handlingView = nullptr;

    // INIT WINDOW
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    desktopMode.width = (unsigned int)((float)desktopMode.width * 0.75f);
    desktopMode.height = (unsigned int)((float)desktopMode.height * 0.75f);
    window.create(desktopMode, windowTitle.c_str()); // Main editor window
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window, false); // Init IMGUI with main window

    LOG_INFO("Initialised Window");

    // IMGUI FLAGS
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // IMGUI STYLE AND THEME
    SetImguiStyle();
    LOG_TRACE("Applied theme");

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
                if (graphView->multiSelectNodes.size() > 0) {
                    std::string data;
                    Graph::Serialization::SaveNodesToData(*graphView, graphView->multiSelectNodes, data);
                    data = "TEXTUREGRAPH!" + data;
                    clip::set_text(data);
                }
                if (graphView->selectedNode > -1) {
                    std::string data;
                    Graph::Serialization::SaveNodesToData(*graphView, std::vector<int>({ graphView->selectedNode }), data);
                    data = "TEXTUREGRAPH!" + data;
                    clip::set_text(data);
                }
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

    ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 2);
    if (ImGui::BeginPopup("##BrowserNew")) {
        if (ImGui::Selectable("Bundle...", false, 0, ImVec2(70, 30))) {
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
        if (ImGui::Selectable("Graph...", false, 0, ImVec2(70, 30))) { OpenPopup("New Graph"); ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    Graph::Serialization::GraphNewPopup(*graphView, buf);
    Graph::Serialization::ClearPromptPopup(*graphView);
    Bundle::Serialization::BundlePromptPopup();

    ImGui::PopStyleVar(1);
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