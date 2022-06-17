#include "LibraryView.h"

LibraryEditorView::LibraryEditorView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_) 
	: SubWindow(main_, name_, flags_) 
{
}

void LibraryEditorView::LoadNodes() {
	categories.clear();
	topLevelCategories.clear();
	categoryIndexList.clear();
	categorySelector.clear();

	int i = 0;
	for (auto* node : LibraryManager::compiledNodes) {
		std::vector<std::string> categorySections = Utility::String::split(node->category, '/');

		// Insert node index into node index list
		if (categoryIndexList.find(categorySections) == categoryIndexList.end()) {
			categoryIndexList.insert({ categorySections, std::vector<int>({i}) });
		}
		else {
			auto categoryNodes = categoryIndexList[categorySections];
			categoryNodes.push_back(i);
			categoryIndexList[categorySections] = categoryNodes;
		}

		// Category combining
		for (int s = 0; s < categorySections.size(); s++) {
			auto section = categorySections[s];
			if (s == 0) {
				if (categories.find(section) == categories.end()) {
					// New Top Level
					categories.insert({ section, std::vector<std::string>() });
					topLevelCategories.push_back(section);
					auto n = std::vector<std::string>({ categorySections.begin(), categorySections.begin() + s + 1});
					categorySelector.insert({ n , Utility::ImGuiExtra::CollapsableSelectorData()});
				}
				continue;
			}

			auto parent = categorySections[s - 1];
			if (categories.find(parent) == categories.end()) {
				categories.insert({ parent, std::vector<std::string>({section})});
				auto n = std::vector<std::string>({ categorySections.begin(), categorySections.begin() + s + 1});
				categorySelector.insert({ n, Utility::ImGuiExtra::CollapsableSelectorData() });
			}
			else {
				auto subCategories = categories[parent];
				if (std::find(subCategories.begin(), subCategories.end(), section) == subCategories.end()) {
					subCategories.push_back(section);
					categories[parent] = subCategories;
					auto n = std::vector<std::string>({ categorySections.begin(), categorySections.begin() + s + 1 });
					categorySelector.insert({ n, Utility::ImGuiExtra::CollapsableSelectorData() });
				}
			}
		}
		 
		i++;
	}

	selectedCategory = std::vector<std::string>({ topLevelCategories[0] });

	RenderThumbnails();
}

void LibraryEditorView::RenderThumbnails()
{
	thumbnails.clear();
	Utility::Timer thumbTmr;

	for (auto& node : LibraryManager::compiledNodes)
	{
		Utility::Timer nodeTmr;
		auto& className = node->nodeClass;
		node->SetTextureSize(sf::Vector2i(100, 100));
		node->Execute(nullptr);
		auto& tex = node->displayTexture;
		thumbnails.insert({ className, tex });
		LOG_TRACE("Generated thumbnail for Node Class '{0}' in {1}ms", className, nodeTmr.Elapsed() * 1000.f);
	}

	LOG_INFO("Generated library thumbnails in {0}ms", thumbTmr.Elapsed() * 1000.f);
}

void LibraryEditorView::RenderCategories(std::vector<std::string> categorySections, std::string parentCategory)
{
	ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetColorU32(ImGuiCol_WindowBg));
	for (auto section : categorySections) {
		if (categories.find(section) == categories.end()) {
			// Lowest Level Category
			auto categoryFull = parentCategory + (parentCategory.size() > 0 ? "/" : "") + section;
			auto split = Utility::String::split(categoryFull, '/');
			categorySelector[split].selected = selectedCategory == split;
			Utility::ImGuiExtra::CollapsableSelectorData data = Utility::ImGuiExtra::ThickSelectable(section.c_str(), categoryFull.c_str(), categorySelector[split]);
			if (data.selected) {
				if (selectedCategory != split) {
					std::stringstream nameStream;
					for (auto section : split) {
						nameStream << "/" << section;
					}
					LOG_TRACE("[Library] Selected Category '{0}'", nameStream.str());
				}
				selectedCategory = split;
			}
			data.selected = false;
			categorySelector[split] = data;
		}
		else {
			if (categories[section].size() == 0) {
				// Lowest Level Category
				auto categoryFull = parentCategory + (parentCategory.size() > 0 ? "/" : "") + section;
				auto split = Utility::String::split(categoryFull, '/');
				categorySelector[split].selected = selectedCategory == split;
				Utility::ImGuiExtra::CollapsableSelectorData data = Utility::ImGuiExtra::ThickSelectable(section.c_str(), categoryFull.c_str(), categorySelector[split]);
				if (data.selected) {
					if (selectedCategory != split) {
						std::stringstream nameStream;
						for (auto section : split) {
							nameStream << "/" << section;
						}
						LOG_TRACE("[Library] Selected Category '{0}'", nameStream.str());
					}
					selectedCategory = split;
				}
				data.selected = false;
				categorySelector[split] = data;
			}
			else {
				auto categoryFull = parentCategory + (parentCategory.size() > 0 ? "/" : "") + section;
				auto split = Utility::String::split(categoryFull, '/');
				categorySelector[split].selected = selectedCategory == split;
				Utility::ImGuiExtra::CollapsableSelectorData data = Utility::ImGuiExtra::CollapsingSelectable(section.c_str(), categoryFull.c_str(), categorySelector[split]);
				if (data.selected) {
					if (selectedCategory != split) {
						std::stringstream nameStream;
						for (auto section : split) {
							nameStream << "/" << section;
						}
						LOG_TRACE("[Library] Selected Category '{0}'", nameStream.str());
					}
					selectedCategory = split; 
				}
				data.selected = false;
				categorySelector[split] = data;
				if(categorySelector[split].open) {
					ImGui::Indent();
					RenderCategories(categories[section], parentCategory + (parentCategory.size() > 0 ? "/" : "") + section);
					ImGui::Unindent();
				}
			}
		}
	}
	ImGui::PopStyleColor();
}

void LibraryEditorView::RenderNodeListing(GraphNode* node, int index) {
	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ChildBg));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_Header));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5));

	sf::Texture& tex = thumbnails[node->nodeClass];
		
	ImGui::ImageButtonWithText(tex, node->displayName.c_str(), 5, ImVec2(40,40), ImVec2(ImGui::GetContentRegionAvail().x, 50));
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		// Set payload to carry the index of node
		ImGui::SetDragDropPayload("NODE_CLASS_INSTANCE", &index, sizeof(int));

		ImGui::Text(node->displayName.c_str());

		ImGui::EndDragDropSource();
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);
}

void LibraryEditorView::RenderNodes() {
	for (auto nodeIndex : categoryIndexList[selectedCategory])
	{
		auto* node = LibraryManager::compiledNodes[nodeIndex];
		RenderNodeListing(node, nodeIndex);
	}
}

void LibraryEditorView::ComponentRender() {
	float tableHeight = ImGui::GetWindowContentRegionMax().y;
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(1, 0));
	ImGui::BeginTable("Library_Table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInner, ImVec2(0, tableHeight));
	ImGui::TableSetupColumn("##Categories", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoHide);
	ImGui::TableSetupColumn("##Nodes", ImGuiTableColumnFlags_NoHeaderLabel | ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoHide);

	ImGui::TableNextRow();

	ImGui::TableSetColumnIndex(0);

	float leftColWidth = ImGui::GetContentRegionAvail().x + 1;
	ImVec2 windowPos = ImGui::GetWindowPos();

	// CATEGORY SECTION
	ImGuiWindowFlags window_flags = 0
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 2.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetColorU32(ImGuiCol_WindowBg));
	ImGui::SetNextWindowSize(ImGui::GetContentRegionAvail());
	ImGui::BeginChild("##LibraryCategories", ImGui::GetContentRegionAvail(), false, window_flags);
	RenderCategories(topLevelCategories);
	ImGui::EndChild();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor();

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddLine(windowPos + ImVec2(leftColWidth, 0), windowPos + ImVec2(leftColWidth, tableHeight), ImGui::GetColorU32(ImGuiCol_Border));

	ImGui::TableSetColumnIndex(1);
	// NODES SECTION
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 2.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::SetNextWindowSize(ImGui::GetContentRegionAvail());
	ImGui::BeginChild("##LibraryNodes", ImGui::GetContentRegionAvail(), false, window_flags);
	RenderNodes();
	ImGui::EndChild();
	ImGui::PopStyleVar(2);

	ImGui::EndTable();
	ImGui::PopStyleVar();
}