// Include the "front.h" header
#include <vector>
#include "HomePage.h"
#include "imgui.h"
// Include any other necessary headers for your project here
// Definition of the ShowMenuTools function
namespace HomePage {

    void ShowMenuTools()
    {
        ImGui::MenuItem("(Tools", NULL, false, false);
        if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
        if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
        ImGui::Separator();
        if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        if (ImGui::MenuItem("Paste", "CTRL+V")) {}
    }
    void ShowFileData(bool showFileData, const std::vector<std::vector<unsigned char>>& filedata, float currentHeight) {
        if (!showFileData) {
            return;
        }

        float windowHeight = ImGui::GetIO().DisplaySize.y;

        // Calculate the height from the cursor to the bottom of the window
        float heightFromCursorToBottom = windowHeight - currentHeight ;

        float hexViewerWidth = 500;
        ImGui::SetNextWindowSize(ImVec2(hexViewerWidth, heightFromCursorToBottom), ImGuiCond_Always);
        ImGui::Begin("File Data Viewer", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        ImGui::Columns(17, "fileDataColumns", false);  // 16 bytes + 1 offset column
        int offset = 0;
        int Row = filedata.size();
        for (size_t i = 0; i <Row ; i++) {
            // first row
            if (i == 0) {
                for (int k = -1; k < 16; ++k) {
                    if (k == -1) {
                        ImGui::Text("Offset");
                        ImGui::SetColumnWidth(0, 80.0f);
                        ImGui::NextColumn();
                    }
                    else {
                        ImGui::Text("%02X", k);
                        ImGui::SetColumnWidth(k+1, 25.0f);
                        ImGui::NextColumn();
                    }
                }
                ImGui::Separator();
            }
            for (size_t j = 0; j < 33; j+= 2) {
                if (j == 0) {
                    ImGui::Text("%010X", offset);
                    ImGui::SetColumnWidth(0, 80.0f);
                    ImGui::NextColumn();
                    offset++;
                }
                else {
                    ImGui::Text("%c%c", static_cast<int>(filedata[i][j - 2]), static_cast<int>(filedata[i][j - 1]));
                    ImGui::SetColumnWidth(j/2, 25.0f);
                    ImGui::NextColumn();
                }
            } 
        }
        ImGui::Separator();
        ImGui::End();
    }
    void ShowInfoFile(bool showInfoFile, const std::vector<char>& infoData)
    {
        int count = 0, Tmp = 0;
        const std::vector<char>  tmp;
        ImGui::Begin("Infomation of file", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        for (int i = 0; i < infoData.size(); i += count) {
            if (infoData[i] == '|') {
                while (infoData[i] != '|') {
                    return;
                    }
                }
            }
        ImGui::End();
    }
    void ShowMenuFile()
    {
        ImGui::MenuItem("(MENU FILE)", NULL, false, false);
        if (ImGui::MenuItem("New")) {}
        if (ImGui::MenuItem("Open", "Ctrl+O")) {}
        if (ImGui::BeginMenu("Open Recent"))
        {
            ImGui::MenuItem("fish_hat.c");
            ImGui::MenuItem("fish_hat.inl");
            ImGui::MenuItem("fish_hat.h");
            if (ImGui::BeginMenu("More.."))
            {
                ImGui::MenuItem("Hello");
                ImGui::MenuItem("Sailor");
                if (ImGui::BeginMenu("Recourse.."))
                {
                    ShowMenuFile();
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {}
        if (ImGui::MenuItem("Save As..")) {}

        ImGui::Separator();
        if (ImGui::BeginMenu("Options"))
        {
            static bool enabled = true;
            ImGui::MenuItem("Enabled", "", &enabled);
            ImGui::BeginChild("child", ImVec2(0, 60), true);
            for (int i = 0; i < 10; i++)
                ImGui::Text("Scrolling Text %d", i);
            ImGui::EndChild();
            static float f = 0.5f;
            static int n = 0;
            ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
            ImGui::InputFloat("Input", &f, 0.1f);
            ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Colors"))
        {
            float sz = ImGui::GetTextLineHeight();
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
                ImVec2 p = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
                ImGui::Dummy(ImVec2(sz, sz));
                ImGui::SameLine();
                ImGui::MenuItem(name);
            }
            ImGui::EndMenu();
        }
        // Here we demonstrate appending again to the "Options" menu (which we already created above)
        // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
        // In a real code-base using it would make senses to use this feature from very different code locations.
        if (ImGui::BeginMenu("Options")) // <-- Append!
        {
            static bool b = true;
            ImGui::Checkbox("SomeOption", &b);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Disabled", false)) // Disabled
        {
            IM_ASSERT(0);
        }
        if (ImGui::MenuItem("Checked", NULL, true)) {}
        ImGui::Separator();
        if (ImGui::MenuItem("Quit", "Alt+F4")) {}
        // You can use ImGui::MenuItem to create menu items with shortcuts
        // You can use ImGui::Separator to add separators
        // You can add functionality to the menu items as needed
    }
    void ShowAppMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ShowMenuFile(); // Call the function to populate the File menu
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Search"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Navigation"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tool"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Special"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Options"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                // Add Edit menu items here
                // Example: ImGui::MenuItem("Undo", "CTRL+Z");
                // Example: ImGui::MenuItem("Redo", "CTRL+Y", false, false);  // Disabled item
                // Example: ImGui::MenuItem("Cut", "CTRL+X");
                // Example: ImGui::MenuItem("Copy", "CTRL+C");
                // Example: ImGui::MenuItem("Paste", "CTRL+V");
                // ...
                ShowMenuTools();
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
}