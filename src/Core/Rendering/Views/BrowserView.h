#pragma once

#include "../SubWindow.h"
#include "../../Utils/ImGuiExtra.h"
#include "GraphEditorView.h"
#include "../../Utils/Strings.h"
#include "../../Graph/GraphSerializer.h"

#include <map>
#include <sstream>
#include <thread>
#include <filesystem>
#include <functional>

class BrowserView : public SubWindow
{
public:
	std::string currentGraph;
	GraphEditorView* focusedGraph;
	std::function<void(std::string id)> openPopup;

	std::map<std::string, std::string> foundGraphs; // name -> file

private:
	void RenderGraphListing(std::string graph, std::string graphFile);

protected:
	virtual void ToolBarButtons();
	virtual void ComponentRender();

public:
	void LoadGraphs();

	inline void RegisterPopupCallback(std::function<void (std::string id)> callback)
	{
		openPopup = callback;
	};

	BrowserView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_);
};
