#include "BundleSerializer.h"

namespace Bundle 
{
	namespace Serialization
	{
		std::string currentBundle;
		std::function<void(void)> clearPromptCallback;
		std::function<void(std::string)> openPopup;
		bool dirty = false;

		bool SaveBundleToFile(std::string file) {
			FPacker::Package pkg = FPacker::Package::LoadDir("temp/bundle");
			bool res = pkg.Pack(file);
			currentBundle = file;
			if (res) {
				Bundle::Resources::LoadFromBundle();
				LOG_INFO("Successfully saved bundle '{0}'", file);
			}
			else {
				LOG_ERROR("Error saving bundle '{0}'. (Bundle may be empty)", file);
			}
			dirty = false;
			return res;
		}

		bool LoadBundleFromFile(std::string file) {
			NewBundle();
			FPacker::Package pkg = FPacker::Package::LoadPackageFile(file);
			bool res = pkg.Unpack("temp/bundle");
			currentBundle = file;
			if (res) {
				Bundle::Resources::LoadFromBundle();
				LOG_INFO("Successfully loaded bundle '{0}'", file);
			}
			else {
				LOG_ERROR("Error loading bundle '{0}'", file);
			}
			dirty = false;
			return res;
		}

		bool AskLoadBundleFromFile() {
			nfdchar_t* outPath;
			nfdfilteritem_t filterItem[1] = { { "Texture Graph Bundle", "tgbundle" } };
			nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, "");
			if (result == NFD_OKAY) {
				return LoadBundleFromFile(std::string(outPath));
			}
			else {
				if (result == NFD_CANCEL) {
					LOG_WARN("User canceled loading graph");
					return false;
				}
				else {
					LOG_ERROR("Error loading graph");
					return false;
				}
			}
		}

		bool AskSaveBundleToFile() {
			nfdchar_t* outPath;
			nfdfilteritem_t filterItem[1] = { { "Texture Graph Bundle", "tgbundle" } };
			nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, "", "");
			if (result == NFD_OKAY) {
				return SaveBundleToFile(std::string(outPath));
			}
			else {
				if (result == NFD_CANCEL) {
					LOG_WARN("User canceled saving graph");
					return false;
				}
				else {
					LOG_ERROR("Error saving graph");
					return false;
				}
			}
		}

		void NewBundle() {
			if (std::filesystem::exists(std::filesystem::path("temp/bundle"))) {
				std::filesystem::remove_all("temp/bundle");
			}
			std::filesystem::create_directories(std::filesystem::path("temp/bundle"));
			std::filesystem::create_directories(std::filesystem::path("temp/bundle/resources"));
			Bundle::Resources::LoadFromBundle();

			currentBundle = "";
			dirty = false;
		}

		void SafeNew(std::function<void(void)> callback) {
			clearPromptCallback = callback;

			openPopup("##BundlePrompt");
		}

		void BundlePromptPopup() {
			if (ImGui::BeginPopup("##BundlePrompt")) {
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5, 0.5));
				ImGui::Selectable("You have unsaved bundle changes.", false, 0, ImVec2(270, 25), false, nullptr, false, true);
				ImGui::PopStyleVar();
				if (ImGui::Button("Cancel", ImVec2(80, 30))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Don't Save", ImVec2(80, 30))) {
					ImGui::CloseCurrentPopup();
					clearPromptCallback();
				}
				ImGui::SameLine();
				if (ImGui::Button("Save", ImVec2(80, 30))) {
					SaveBundleToFile(currentBundle);
					ImGui::CloseCurrentPopup();
					clearPromptCallback();
				}
				ImGui::EndPopup();
			}
		}
	}
}