#pragma once
// Include necessary ImGui header
#include "imgui.h"
#include <vector>
namespace HomePage {
	// Declaration of the ShowExampleAppMainMenuBar function
	void ShowAppMainMenuBar();
	void ShowMenuFile();
	void ShowMenuTools();
	void ShowFileData(bool showFileData, const std::vector<std::vector<unsigned char>>& filedata, float currentHeight);
	void ShowInfoFile(bool showInfoFile, const std::vector<char>&infoData);
	// Include any other necessary headers for your project here
}