#include "InspectorView.h"

InspectorView::InspectorView(sf::RenderWindow& main_, std::string name_, ImGuiWindowFlags flags_)
	: SubWindow(main_, name_, flags_)
{
	graph = nullptr;
}

void InspectorView::SetGraph(GraphEditorView* newGraph) {
	graph = newGraph;
	newSize[0] = log2(graph->texSize.x); 
	newSize[1] = log2(graph->texSize.y);
}

void InspectorView::ComponentRender()
{
	if (ImGui::CollapsingHeader("Graph Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderInt("##XTexSlider", &newSize[0], 1, 12); ImGui::SameLine(); ImGui::Text(("Size X: " + std::to_string((int)pow(2, newSize[0]))).c_str());
		ImGui::SliderInt("##YTexSlider", &newSize[1], 1, 12); ImGui::SameLine(); ImGui::Text(("Size Y: " + std::to_string((int)pow(2, newSize[1]))).c_str());
		if (ImGui::Button("Update Graph", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			int newX = pow(2, newSize[0]);
			int newY = pow(2, newSize[1]);
			if (newX != graph->texSize.x || newY != graph->texSize.y) {

				graph->UpdateTexSize(sf::Vector2i(newX, newY));
			}
		}
	}

	if (graph->selectedNode >= 0 && graph->selectedNode < graph->nodes.size()) {
		if (ImGui::CollapsingHeader("Node Info", ImGuiTreeNodeFlags_DefaultOpen)) {
			std::stringstream classStream;
			classStream << "Class: " << graph->nodes[graph->selectedNode]->nodeClass;
			ImGui::Text(classStream.str().c_str());
			std::stringstream idStream;
			idStream << "ID: " << graph->nodes[graph->selectedNode]->nodeId;
			ImGui::Text(idStream.str().c_str());
		}
		if (ImGui::CollapsingHeader("Node Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (dynamic_cast<ImageNode*>(graph->nodes[graph->selectedNode]) != nullptr) {
				if (dynamic_cast<ImageNode*>(graph->nodes[graph->selectedNode])->resourceName != "") {
					ImGui::InputText("##imagepath", const_cast<char*>(std::filesystem::path(dynamic_cast<ImageNode*>(graph->nodes[graph->selectedNode])->resourceName).filename().replace_extension("").string().c_str()), IM_ARRAYSIZE(dynamic_cast<ImageNode*>(graph->nodes[graph->selectedNode])->resourceName.c_str()), ImGuiInputTextFlags_ReadOnly);
				}
				else {
					ImGui::InputText("##imagepath", const_cast<char*>(""), 0, ImGuiInputTextFlags_ReadOnly);
				}
				ImGui::SameLine();
				if (ImGui::Button("...")) {
					ImGui::OpenPopup("##ResourceSelector");
				}
				std::string selector = Bundle::Resources::ResourceSelector(dynamic_cast<ImageNode*>(graph->nodes[graph->selectedNode])->resourceName);
				if (selector != "") {
					dynamic_cast<ImageNode*>(graph->nodes[graph->selectedNode])->resourceName = selector;
				}
				ImGui::SameLine();
				ImGui::Text("Image Resource");
			}

			for (auto& var : graph->nodes[graph->selectedNode]->paramLuaVars)
			{
				auto& luaVarData = graph->nodes[graph->selectedNode]->luaVarData[var.second];
				auto& name = var.first;
				switch (luaVarData.dataType)
				{
				case Types::DataType_Bool:
					ImGui::Checkbox(name.c_str(), &luaVarData.boolVar);
					break;
				case Types::DataType_Int:
					ImGui::InputInt(name.c_str(), &luaVarData.intVar);
					break;
				case Types::DataType_Float:
					ImGui::InputFloat(name.c_str(), &luaVarData.floatVar);
					break;
				case Types::DataType_Color:
				{
					ImColor colorTemp = ImColor(luaVarData.colorVar.r / 255.f, luaVarData.colorVar.g / 255.f, luaVarData.colorVar.b / 255.f, luaVarData.colorVar.a / 255.f);
					ImGui::ColorEdit4(name.c_str(), (float*)&colorTemp, ImGuiColorEditFlags_AlphaPreview);
					luaVarData.colorVar = sf::Color(colorTemp.Value.x * 255, colorTemp.Value.y * 255, colorTemp.Value.z * 255, colorTemp.Value.w * 255);
				} break;
				default:
					break;
				}
			}
		}
	}
}