#pragma once

#include <string>

namespace ImGuiFilePicker {

	// Function to open a file picker window and return the selected file or directory path.
	std::string OpenFilePicker(const char* windowTitle = "File Picker", const char* startDirectory = "");

} // namespace ImGuiFilePicker
