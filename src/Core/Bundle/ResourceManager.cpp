#include "ResourceManager.h"

namespace Bundle
{
	namespace Resources
	{
		std::vector<std::string> resourceList;
		std::unordered_map<int, sf::Texture*> resourcePreviews;

		void Init() {
			resourceList = std::vector<std::string>();
		}

		void Clear() {
			ClearPreviews();
			resourceList.clear();
			resourcePreviews.clear();
		}

		void LoadFromBundle()
		{
			Clear();
			for (std::filesystem::directory_iterator i("temp/bundle/resources"), end; i != end; ++i) {
				if (!is_directory(i->path())) {
					std::string newPath = "temp/bundle/resources/" + i->path().filename().string();
					resourceList.push_back(newPath);
				}
			}
			GeneratePreviews();
		}

		std::string ImportResourceFromAsk() {
			nfdchar_t* outPath;
			nfdfilteritem_t filterItem[2] = { { "PNG", "png" }, {"JPG", "jpg"} };
			nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, "");
			if (result == NFD_OKAY) {
				std::string newPath = "temp/bundle/resources/" + std::filesystem::path(outPath).filename().string();
				std::filesystem::copy(outPath, newPath);
				resourceList.push_back(std::string(newPath));
				GeneratePreviews();
				return std::string(newPath);
			}
			else {
				if (result == NFD_CANCEL) {
					LOG_WARN("User cancelled picking image");
					return "";
				}
				else {
					LOG_ERROR("Error picking image");
					return "";
				}
			}
		}

		void ClearPreviews() {
			for (auto pair : resourcePreviews) {
				if (pair.second == nullptr) { continue; }
				delete pair.second;
			}
		}

		void GeneratePreviews() {
			ClearPreviews();
			resourcePreviews.clear();
			int r = 0;
			for (auto& resource : resourceList) {
				sf::Texture* newT = new sf::Texture();
				if (!newT->loadFromFile(resource)) {
					LOG_ERROR("Error loading preview for resource '{0}'", resource);
				}

				resourcePreviews.insert({r, newT});
				r++;
			}
		}

		std::string ResourceSelector(std::string currentResource) {
			std::string selected = "";
			if (ImGui::BeginPopup("##ResourceSelector")) {
				ImGuiWindowFlags window_flags = 0
					| ImGuiWindowFlags_NoDocking
					| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
					| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
					| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

				float ySize = std::min(resourcePreviews.size() * 50.f, 200.f) + 3.f;

				ImGui::BeginChild("##RSWindow", ImVec2(500, ySize), true, window_flags);
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ChildBg));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(ImGuiCol_Header));
				for (int r = 0; r < resourceList.size(); r++) {
					if (resourceList[r] == currentResource) {
						ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_Header));
					}
					if (ImGui::ImageButtonWithText(*resourcePreviews[r], std::filesystem::path(resourceList[r]).filename().replace_extension("").string().c_str(), 5, ImVec2(40, 40), ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
						ImGui::CloseCurrentPopup();
						selected = resourceList[r];
					}
					if (resourceList[r] == currentResource) {
						ImGui::PopStyleColor();
					}
				}
				ImGui::PopStyleColor(2);
				ImGui::EndChild();

				if (ImGui::Selectable("Link new resource...", false, ImGuiSelectableFlags_SelectOnClick, ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
					ImGui::CloseCurrentPopup();
					selected = ImportResourceFromAsk();
				}
				ImGui::EndPopup();
			}
			return selected;
		}
	}
}