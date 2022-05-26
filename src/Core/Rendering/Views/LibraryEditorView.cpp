#include "LibraryEditorView.h"

LibraryEditorView::LibraryEditorView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_) 
	: SubWindow(main_, name_, flags_) 
{
}

void LibraryEditorView::LoadNodes() {
	categories.clear();

	int i = 0;
	for (auto& node : LibraryManager::compiledNodes) {
		if (categories.find(node.category) == categories.end()) {
			categories.insert({ node.category, std::vector<int>({i}) });
		}
		else {
			auto categoryNodes = categories[node.category];
			categoryNodes.push_back(i);
			categories[node.category] = categoryNodes;
		}
		 
		i++;
	}
}

void LibraryEditorView::ComponentRender() {
	for (std::map<std::string, std::vector<int>>::iterator it = categories.begin(); it != categories.end(); ++it) {
		if (ImGui::CollapsingHeader(it->first.c_str())) {
			for (auto nodeIndex : it->second) {
				ImGui::Text(LibraryManager::compiledNodes[nodeIndex].displayName.c_str());
			}
		}
	}
}