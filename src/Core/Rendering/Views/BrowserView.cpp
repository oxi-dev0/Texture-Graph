#include "BrowserView.h"

BrowserView::BrowserView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_)
	: SubWindow(main_, name_, flags_)
{
	LoadGraphs();
}

void BrowserView::ToolBarButtons() {
	if (ImGui::ImageButton(*ImageCache::images["icon-new"], sf::Vector2f(25, 25), 5)) {
		openPopup("New Graph");
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(*ImageCache::images["icon-refresh"], sf::Vector2f(25, 25), 5)) {
		LoadGraphs();
	}
}

void BrowserView::RenderGraphListing(std::string graph, std::string graphFile) {
	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ChildBg));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_Header));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5));
	if (ImGui::Button(graph.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
		
			if (focusedGraph != nullptr) {
				Serialization::Graph::LoadGraphFromFile(*focusedGraph, graphFile);
			}
	}
	ImGui::PopStyleVar(2); 
	ImGui::PopStyleColor(2);
}

void BrowserView::ComponentRender() {
	ToolBar(45.f);

	for (auto& graphMap : foundGraphs) {
		RenderGraphListing(graphMap.first, graphMap.second);
	}
}

void BrowserView::LoadGraphs() {
	foundGraphs.clear();

	if (!std::filesystem::is_directory("graphs") || !std::filesystem::exists("graphs")) {
		std::filesystem::create_directory("graphs");
	}

	LOG_INFO("Scanning graphs folder for graph definitions...");
	std::vector<std::string> foundGraphFiles;
	for (const auto& p : std::filesystem::recursive_directory_iterator("graphs/"))
	{
		if (p.path().extension() == ".tgraph")
		{
			auto path = p.path().string();
			foundGraphFiles.push_back(Utility::String::replace(path, '\\', '/'));
		}
	}
	LOG_TRACE("Found {0} definitions in graphs folder", foundGraphFiles.size());

	for (auto& file : foundGraphFiles) {
		std::vector<std::string> sections = Utility::String::split(file, '/');
		auto lastSection = sections[sections.size() - 1];
		auto name = Utility::String::split(lastSection, '.')[0];
		foundGraphs.insert({ name, file });
	}
}