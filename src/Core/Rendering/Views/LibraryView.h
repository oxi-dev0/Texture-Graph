#pragma once

#include "../SubWindow.h"
#include "../../Utils/ImGuiExtra.h"
#include "../../Library/LibraryManager.h"

#include <map>
#include <sstream>
#include <thread>

class LibraryEditorView : public SubWindow
{
public:
	std::map<std::vector<std::string>, std::vector<int>> categoryIndexList; // full category section list -> NM Index
	std::map<std::string, std::vector<std::string>> categories; // category section -> list of indiv category sections
	std::vector<std::string> topLevelCategories;
	std::map<std::vector<std::string>, Utility::ImGuiExtra::CollapsableSelectorData> categorySelector; // full category section list -> selector data
	std::vector<std::string> selectedCategory; // full category section list

	std::map<std::string, sf::Texture> thumbnails; // node class -> thumbnail image

	void LoadNodes();

	LibraryEditorView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_);

private:
	void RenderCategories(std::vector<std::string> categories, std::string parentCategory="");
	void RenderNodes();
	void RenderNodeListing(GraphNode& node, int index);

protected:
	virtual void ComponentRender();

	void RenderThumbnails();
};

