#pragma once

#include "../SubWindow.h"
#include "../../Utils/ImGuiExtra.h"
#include "GraphEditorView.h"
#include "../../Utils/Strings.h"
#include "../../Bundle/GraphSerializer.h"
#include "../../Bundle/ResourceManager.h"

#include <map>
#include <sstream>
#include <thread>
#include <filesystem>
#include <functional>

class BrowserView : public SubWindow
{
private:
	Utility::ImGuiExtra::CollapsableSelectorData oldTitleData;
	Utility::ImGuiExtra::CollapsableSelectorData oldResourcesData;
public:
	GraphEditorView* focusedGraphView;
	std::function<void(std::string id)> openPopup;

	std::string* currentBundle;

protected:
	virtual void ToolBarButtons();
	virtual void ComponentRender();

public:
	inline void RegisterPopupCallback(std::function<void (std::string id)> callback)
	{
		openPopup = callback;
	};

	BrowserView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_);
};
