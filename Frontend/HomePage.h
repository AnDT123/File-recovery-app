#pragma once
// Include necessary ImGui header
#include "imgui.h"
#include <vector>
#include "../Backend/ntfs.h"
#include "../Backend/Tool.h"
#include <string>
namespace HomePage {
    struct InformationFile {
        std::string file_name;
        std::string file_path;
        int file_size;
        std::string file_state;
        std::string file_create_time;
        std::string file_lastw_time;
        std::string file_mode;
        std::string file_offset;
        std::string byte_per_page;
    };
	void ShowAppMainMenuBar(std::vector<string> drivesVec, bool* loadDrive, std::string* currentDrive);
	void ShowMenuFile();
	void ShowMenuTools();
	void ShowFileData(bool showFileData, const std::vector<int>& filedata, float currentHeight, DWORD offset);
	void ShowInfoFile(bool showInfoFile, const std::vector<char>&infoData);
    void ShowFileContextMenu(MFT_RECORD record);
	// Include any other necessary headers for your project here
}