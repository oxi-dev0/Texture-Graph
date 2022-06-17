#include "BrowserView.h"

BrowserView::BrowserView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_)
	: SubWindow(main_, name_, flags_)
{
}

void BrowserView::ToolBarButtons() {
	if (ImGui::ImageButton(*ImageCache::images["icon-new"], sf::Vector2f(25, 25), 5)) {
		openPopup("New Graph");
	}
	ImGui::SameLine();
}

void BrowserView::ComponentRender() {
	ToolBar(45.f);

	oldTitleData.open = true;
	oldTitleData.selected = false;
	Utility::ImGuiExtra::CollapsableSelectorData data = Utility::ImGuiExtra::CollapsingSelectable(focusedGraphView->currentGraphFile == "" ? "Unsaved Bundle*" : focusedGraphView->currentGraphFile.c_str(), "TitleSelectable", oldTitleData);
	oldTitleData.hovered = data.hovered;

	ImGui::Indent();
	oldResourcesData.selected = false;
	Utility::ImGuiExtra::CollapsableSelectorData resourcesData = Utility::ImGuiExtra::CollapsingSelectable("Resources", "ResourcesSelectable", oldResourcesData);
	oldResourcesData.hovered = resourcesData.hovered;
	oldResourcesData.open = resourcesData.open;
	if (oldResourcesData.open) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ChildBg));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_Header));

		ImGui::Indent();

		for (int r = 0; r < Graph::Resources::resourceList.size(); r++) {
			ImGui::ImageButtonWithText(Graph::Resources::resourcePreviews[r], Graph::Resources::resourceList[r].c_str(), 5, ImVec2(40, 40), ImVec2(ImGui::GetContentRegionAvail().x, 50));
		}

		if (ImGui::Button("Link new resource...", ImVec2(ImGui::GetContentRegionAvail().x, 35.f))) {
			Graph::Resources::ImportResourceFromAsk();
		}

		ImGui::Unindent();
		ImGui::PopStyleColor(2);
	}

	ImGui::Unindent();
}