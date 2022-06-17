#pragma once

#include <string>
#include <filesystem>

#include "../Utils/Log.h"
#include "ResourceManager.h"

#include <fpacker.h>
#include <nfd.h>

namespace Bundle
{
	namespace Serialization
	{
		extern std::string currentBundle;
		extern std::function<void(void)> clearPromptCallback;
		extern std::function<void(std::string)> openPopup;
		extern bool dirty;

		bool SaveBundleToFile(std::string file); // bundle will already be unpacked to temp/bundle/
		bool LoadBundleFromFile(std::string file); // will be unpacked to temp/bundle/

		bool AskLoadBundleFromFile();
		bool AskSaveBundleToFile();

		void SafeNew(std::function<void(void)> callback);
		void BundlePromptPopup();

		void NewBundle();
	}
}
