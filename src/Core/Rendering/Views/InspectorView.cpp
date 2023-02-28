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
			ImGui::Indent();
			std::stringstream classStream;
			classStream << "Class: " << graph->nodes[graph->selectedNode]->nodeClass;
			ImGui::Text(classStream.str().c_str());
			std::stringstream idStream;
			idStream << "ID: " << graph->nodes[graph->selectedNode]->nodeId;
			ImGui::Text(idStream.str().c_str());
			std::stringstream evStream;
			evStream << "Evaluated: " << graph->nodes[graph->selectedNode]->evaluated << "; Evaluating: " << graph->nodes[graph->selectedNode]->isEvaluating;
			ImGui::Text(evStream.str().c_str());
			ImGui::Unindent();
		}
		if (ImGui::CollapsingHeader("Node Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent();
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
				if (LibraryManager::compiledNodeVarShowExpressions.find(graph->nodes[graph->selectedNode]->nodeClass) != LibraryManager::compiledNodeVarShowExpressions.end()) {
					auto& luaVarShowExpressionsMap = LibraryManager::compiledNodeVarShowExpressions.find(graph->nodes[graph->selectedNode]->nodeClass)->second;
						if (luaVarShowExpressionsMap.find(var.second) != luaVarShowExpressionsMap.end()) {
							std::function<bool(bool, std::string, ConditionParser::ExpressionOperation, bool, std::string)> evalFunc = [&](bool invertVar1, std::string var1Str, ConditionParser::ExpressionOperation op, bool invertVar2, std::string var2Str) {
								Types::DataType type = Types::DataType::DataType_Bool;
								bool var1Set = false;
								bool var2Set = false;
								Types::WildData var1Data;
								Types::WildData var2Data;

								auto& varDataMap = graph->nodes[graph->selectedNode]->luaVarData;
								if (varDataMap.find(var1Str) != varDataMap.end()) {
									var1Data = varDataMap[var1Str];
									var1Set = true;
									type = var1Data.dataType;
								}
								if (varDataMap.find(var2Str) != varDataMap.end()) {
									if (varDataMap[var2Str].dataType != type && var1Set) {
										LOG_CRITICAL("Variable '{0}' error with 'show' expression: Mismatching data types in comparison '{1}{2}{3}'", var.second, var1Str, ConditionParser::expressionOperationToString.find(op)->second, var2Str);
									}
									var2Data = varDataMap[var2Str];
									var2Set = true;
									type = var2Data.dataType;
								}

								if (!var1Set && !var2Set) {
									LOG_CRITICAL("Variable '{0}' error with 'show' expression: No variable found in comparison '{1}{2}{3}'", var.second, var1Str, ConditionParser::expressionOperationToString.find(op)->second, var2Str);
								}

								if (!var1Set) {
									var1Data.dataType = type;
									var1Data.FromString(var1Str);
								}
								if (!var2Set) {
									var2Data.dataType = type;
									var2Data.FromString(var2Str);
								}

								if (type == Types::DataType::DataType_Bool) {
									if (invertVar1) { var1Data.boolVar = !var1Data.boolVar; }
									if (invertVar2) { var2Data.boolVar = !var2Data.boolVar; }
								}

								switch (op) {
								case ConditionParser::ExpressionOperation::EQUAL:
									return var1Data.AsString() == var2Data.AsString();
									break;
								case ConditionParser::ExpressionOperation::NOTEQUAL:
									return var1Data.AsString() != var2Data.AsString();
								}

								return false;
							};

							bool show = luaVarShowExpressionsMap[var.second]->Eval(evalFunc);
							if (!show) {
								continue;
							}
						}
				}

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
				case Types::DataType_Vec2:
				{
					float data[2] = { luaVarData.vec2Var.x, luaVarData.vec2Var.y };
					ImGui::InputFloat2(name.c_str(), data);
					luaVarData.vec2Var.x = data[0];
					luaVarData.vec2Var.y = data[1];
				}	break;
				case Types::DataType_Enum:
				{
					auto enumSet = graph->nodes[graph->selectedNode]->luaVarEnumSets[var.second];
					static int selectedEnum = std::find(enumSet.begin(), enumSet.end(), luaVarData.enumVar) - enumSet.begin();
					const char* previewVal = enumSet[selectedEnum].c_str();
					if (ImGui::BeginCombo(name.c_str(), previewVal, ImGuiComboFlags_None))
					{
						for (int n = 0; n < enumSet.size(); n++)
						{
							const bool selected = (selectedEnum == n);
							if (ImGui::Selectable(enumSet[n].c_str(), selected)) {
								selectedEnum = n;
								luaVarData.enumVar = enumSet[selectedEnum];
							}

							if (selected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
				}	break;
				default:
					break;
				}
			}
			ImGui::Unindent();
		}
	}
}