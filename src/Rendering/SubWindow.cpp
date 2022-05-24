#include "SubWindow.h"

SubWindow::SubWindow(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_) : main(main_) {
    visible = true;
    name = name_;
    flags = flags_;
}

SubWindow::~SubWindow() {
	//delete& rt;
}

void SubWindow::ProcessEvent(sf::Event& event) {

}

void SubWindow::ImGuiRender() {

}

ImVec2 SubWindow::GetPos() {
    // MUST BE IN RENDER CONTEXT (Gets top left corner)
    ImVec2 vMin = ImGui::GetWindowContentRegionMin();

    vMin.x += ImGui::GetWindowPos().x;
    vMin.y += ImGui::GetWindowPos().y;
    return vMin;
}

void SubWindow::Render() {
    ImVec2 contentAvail = ImGui::GetContentRegionAvail();
    ImGuiRender();

    ImVec2 pos = GetPos();
    prevPos = sf::Vector2i(pos.x, pos.y);
    prevSize = sf::Vector2u((unsigned int)contentAvail.x, (unsigned int)contentAvail.y);
}