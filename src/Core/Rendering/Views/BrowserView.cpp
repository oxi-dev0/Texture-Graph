#include "BrowserView.h"

BrowserView::BrowserView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_)
	: SubWindow(main_, name_, flags_)
{
}

void BrowserView::ToolBarButtons() {
	if (ImGui::ImageButton(*ImageCache::images["icon-new-drop"], sf::Vector2f(25, 25), 5)) {
		openPopup("##BrowserNew");
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("New");

	ImGui::SameLine();
	ImGui::Image(*ImageCache::images["detail-separator"], sf::Vector2f(15, 35));
	ImGui::SameLine();

	if (ImGui::ImageButton(*ImageCache::images["icon-save-1"], sf::Vector2f(25, 25), 5)) {
		if (Bundle::Serialization::currentBundle == "") {
			Bundle::Serialization::AskSaveBundleToFile();
		}
		else {
			Bundle::Serialization::SaveBundleToFile(Bundle::Serialization::currentBundle);
		}
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Save Bundle");

	ImGui::SameLine();

	if (ImGui::ImageButton(*ImageCache::images["icon-open"], sf::Vector2f(25, 25), 5)) {
		GraphEditorView& c = *focusedGraphView;
		std::function<void(void)> f = [&c]() {
			if (Bundle::Serialization::AskLoadBundleFromFile()) {
				c.Clear();
				Graph::Serialization::currentGraph = "";
			}
		};
		if (Bundle::Serialization::dirty && Bundle::Serialization::currentBundle != "") {
			Bundle::Serialization::SafeNew(f);
		}
		else {
			f();
		}
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Open Bundle");
}

void BrowserView::ComponentRender() {
	ToolBar(45.f);

	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::BeginChild("##BrowserBody", ImGui::GetContentRegionAvail(), false, window_flags);
	oldTitleData.open = true;
	oldTitleData.selected = false;
	std::string currentBundle = Bundle::Serialization::currentBundle;
	Utility::ImGuiExtra::CollapsableSelectorData data = Utility::ImGuiExtra::CollapsingSelectable(currentBundle == "" ? "Unsaved Bundle*" : (std::filesystem::path(currentBundle).filename().replace_extension("").string() + (Bundle::Serialization::dirty ? "*" : "")).c_str(), "TitleSelectable", oldTitleData);
	oldTitleData.hovered = data.hovered;

	ImGui::Indent();

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_HeaderHovered));
	for (auto graphFile : Graph::Serialization::GetGraphsInBundle()) {
		bool selected = Graph::Serialization::currentGraph == std::filesystem::path(graphFile).filename().replace_extension("").string();

		if (focusedGraphView->dirty && Bundle::Serialization::dirty == false) { Bundle::Serialization::dirty = true; }

		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(selected ? ImGuiCol_Button : ImGuiCol_Header));
		if (ImGui::ImageButtonWithText(*ImageCache::images["icon-graph"], (std::filesystem::path(graphFile).filename().replace_extension("").string() + (selected && focusedGraphView->dirty ? "*" : "")).c_str(), 5, ImVec2(40, 40), ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
			Graph::Serialization::LoadGraphFromFile(*focusedGraphView, "temp/bundle/" + std::filesystem::path(graphFile).filename().replace_extension("").string() + ".graph");
		}
		if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1])
		{
			ImGui::OpenPopup(("##GraphListingRightClick" + graphFile).c_str());
		}
		bool openRename = false;
		if (ImGui::BeginPopup(("##GraphListingRightClick" + graphFile).c_str())) {
			ImGui::Selectable("##WidthSetter", false, 0, ImVec2(100, 1), false, nullptr, false, true);
			if (ImGui::Selectable("Rename...", false, 0, ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
				openRename = true;
			}
			if (ImGui::Selectable("Delete", false, 0, ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
				std::filesystem::remove(graphFile);
				if (Graph::Serialization::currentGraph == std::filesystem::path(graphFile).filename().replace_extension("").string())
				{
					focusedGraphView->Clear();
					Graph::Serialization::currentGraph = name;
				}
				Bundle::Serialization::dirty = true;
			}
			ImGui::EndPopup();
		}
		if (openRename) {
			ImGui::OpenPopup(("##GraphListingRename" + graphFile).c_str());
		}
		if (ImGui::BeginPopup(("##GraphListingRename" + graphFile).c_str())) {
			std::function<void(void)> f = [this, graphFile]() {
				std::filesystem::copy(graphFile, std::filesystem::path(graphFile).remove_filename().string() + std::string(inputBuf) + ".graph");
				std::filesystem::remove(graphFile);
				Graph::Serialization::LoadGraphFromFile(*focusedGraphView, std::filesystem::path(graphFile).remove_filename().string() + std::string(inputBuf) + ".graph");
			};
			if (ImGui::InputText("Name", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
				f();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Ok")) {
				f();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();
	}
	ImGui::PopStyleColor();

	oldResourcesData.selected = false;
	Utility::ImGuiExtra::CollapsableSelectorData resourcesData = Utility::ImGuiExtra::CollapsingSelectable("Resources", "ResourcesSelectable", oldResourcesData);
	oldResourcesData.hovered = resourcesData.hovered;
	oldResourcesData.open = resourcesData.open;
	if (oldResourcesData.open) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ChildBg));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_Header));

		ImGui::Indent();

		for (int r = 0; r < Bundle::Resources::resourceList.size(); r++) {
			ImGui::ImageButtonWithText(*Bundle::Resources::resourcePreviews[r], std::filesystem::path(Bundle::Resources::resourceList[r]).filename().string().c_str(), 5, ImVec2(40, 40), ImVec2(ImGui::GetContentRegionAvail().x, 50));
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				// Set payload to carry the index of node
				ImGui::SetDragDropPayload("IMAGE_RESOURCE_INSTANCE", &Bundle::Resources::resourceList[r], sizeof(std::string));

				ImGui::Text(std::filesystem::path(Bundle::Resources::resourceList[r]).filename().string().c_str());

				ImGui::EndDragDropSource();
			}
			if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1])
			{
				ImGui::OpenPopup(("##ResourceRightClick" + std::to_string(r)).c_str());
			}
			if (ImGui::BeginPopup(("##ResourceRightClick" + std::to_string(r)).c_str())) {
				ImGui::Selectable("##WidthSetter", false, 0, ImVec2(100, 1), false, nullptr, false, true);
				if (ImGui::Selectable("Delete", false, 0, ImVec2(ImGui::GetContentRegionAvail().x, 30))) {
					std::filesystem::remove(std::filesystem::path(Bundle::Resources::resourceList[r]));
					Bundle::Resources::LoadFromBundle();
				}
				ImGui::EndPopup();
			}
		}

		if (ImGui::Button("Link new resource...", ImVec2(ImGui::GetContentRegionAvail().x, 35.f))) {
			Bundle::Resources::ImportResourceFromAsk();
		}


		ImGui::Unindent();
		ImGui::PopStyleColor(2);
	}

	ImGui::Unindent();
	ImGui::EndChild();
}