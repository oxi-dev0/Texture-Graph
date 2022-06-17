#pragma once

#include "../SubWindow.h"
#include "../../Utils/ImGuiExtra.h"
#include "../../Bundle/Node/ImageNode.h"
#include "../../Bundle/ResourceManager.h"
#include <nfd.h>
#include "GraphEditorView.h"
#include <vector>
#include <map>

class InspectorView : public SubWindow
{
private:
	int newSize[2];

public:
	InspectorView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_);
	void SetGraph(GraphEditorView* newGraph);

	GraphEditorView* graph;

protected:
	virtual void ComponentRender();
};