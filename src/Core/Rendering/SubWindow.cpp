#include "SubWindow.h"

SubWindow::SubWindow(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_) : main(main_) {
    visible = true;
    enabled = true;
    name = name_;
    flags = flags_;
}

SubWindow::~SubWindow() {
	//delete& rt;
}

bool SubWindow::ProcessEvent(sf::Event& event) {
    return false;
}

void SubWindow::ComponentRender() {

}

void SubWindow::InfoBarData() {

}

void SubWindow::InfoBar(float height) {
	ImGui::SetNextWindowPos(ImVec2(prevPos.x, prevPos.y + prevSize.y - height));
	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_AlwaysUseWindowPadding;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 2.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(bgColor.x * 1.0f, bgColor.y * 1.0f, bgColor.z * 1.0f, 1.0f));

	std::stringstream idStream;
	idStream << name << "INFOBAR";
	ImGui::BeginChild(idStream.str().c_str(), ImVec2(prevSize.x, height), false, window_flags);

	ImGui::PopStyleColor();

	InfoBarData();
	
	ImGui::EndChild();
	ImGui::PopStyleVar(2);
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
    ComponentRender();

    ImVec2 pos = GetPos();
    prevPos = sf::Vector2i((int)pos.x, (int)pos.y);
    prevSize = sf::Vector2u((unsigned int)contentAvail.x, (unsigned int)contentAvail.y);
}