#include "FilePicker.h"
#include <imgui.h>
#include <filesystem>

namespace ImGuiFilePicker {

    std::string OpenFilePicker(const char* windowTitle, const char* startDirectory) {
        std::string selectedPath;
        static std::filesystem::path currentPath = std::filesystem::absolute(startDirectory);

        // Define a structure to keep track of the file browser state.
        struct FileBrowserState {
            std::filesystem::path currentDirectory;
        };

        static FileBrowserState state;

        // Helper function to display a directory and its contents.
        auto DisplayDirectory = [&](const std::filesystem::path& directory) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                ImGui::Selectable(entry.path().filename().string().c_str());
                if (ImGui::IsItemClicked()) {
                    if (std::filesystem::is_directory(entry.path())) {
                        state.currentDirectory = entry.path();
                    }
                    else {
                        selectedPath = entry.path().string();
                    }
                }
            }
            };

        ImGui::OpenPopup(windowTitle);

        if (ImGui::BeginPopupModal(windowTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Current Directory: %s", state.currentDirectory.string().c_str());

            if (ImGui::Button("Up")) {
                state.currentDirectory = state.currentDirectory.parent_path();
            }

            // Display the contents of the current directory.
            DisplayDirectory(state.currentDirectory);

            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        return selectedPath;
    }

} // namespace ImGuiFilePicker
