#pragma once

#include "../SubWindow.h"
#include "../../Library/LibraryManager.h"
#include <map>

class LibraryEditorView : public SubWindow
{
public:
	std::map<std::string, std::vector<int>> categories; // category name -> Library Manager Index

	void LoadNodes();

	LibraryEditorView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_);

protected:
	virtual void ComponentRender();
};

