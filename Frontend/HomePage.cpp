// Include the "front.h" header
#include <vector>
#include "HomePage.h"
#include "imgui.h"
#include "../Backend/ntfs.h"
#include "../Backend/Tool.h"
#include <imgui_internal.h>
#include <codecvt>
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
    void ShowFileData(bool showFileData, const std::vector<int>& filedata, float currentHeight, DWORD offset) {
        if (!showFileData) {
            return;
        }
        static bool display_headers = true;
        enum ContentsType { CT_Text, CT_FillButton, CT_Selectable, CT_SelectableSpanRow };
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        static int contents_type = CT_Selectable;
        static int row_min_height = 0.0f;
        int filesize_data = filedata.size() / 16;
        static int selection_x = 0;
        static int selection_y = 0;
        float windowHeight = ImGui::GetIO().DisplaySize.y;

        // Calculate the height from the cursor to the bottom of the window
        float heightFromCursorToBottom = windowHeight - currentHeight;
        float hexViewerWidth = 830;
        ImGui::SetNextWindowSize(ImVec2(hexViewerWidth, heightFromCursorToBottom), ImGuiCond_Always);
        ImGui::Begin("File Data Viewer", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        if (ImGui::BeginTable("File table", 18, ImGuiTableFlags_NoBordersInBody)) {
            if (display_headers) {
                ImGui::TableSetupColumn("  Offset", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("00", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("01", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("02", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("03", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("04", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("05", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("06", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("07", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("08", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("09", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0A", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0B", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0C", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0D", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0E", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0F", ImGuiTableColumnFlags_WidthFixed, 20.0f);
                ImGui::TableSetupColumn("   \t\t\t\t\tUTF-8", ImGuiTableColumnFlags_WidthFixed, 350.0f);
                ImGui::TableHeadersRow();
            }
            static bool column_selected[17] = {};
            for (int row = 0; row < filesize_data; row++)
            {
                ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
                for (int column = 0; column < 17; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    int index = row * 16 + (column - 1) ;
                    const bool item_is_selected = (selection_y == row && selection_x == column);
                    if (column == 0) {
                        char buf[32];
                        ImGui::Text("%010X", row * 16 + offset);
                    }
                    else if (column < 17) {
                        char buf[32];
                        if(filedata[index] <= 0xF)
                            sprintf_s(buf, sizeof(buf), "0%X", filedata[index]);
                        else
                            sprintf_s(buf, sizeof(buf), "%X", filedata[index]);
                        if (contents_type == CT_Selectable) {
                            ImGuiSelectableFlags selectable_flags = (contents_type == CT_SelectableSpanRow) ? ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap : ImGuiSelectableFlags_None;
                            if (item_is_selected)
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f))); // Màu nền của ô
                            // ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                             //ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                            if (ImGui::Selectable(buf, item_is_selected, selectable_flags)) {}
                            if (ImGui::IsItemClicked()) {
                                selection_x = column;
                                selection_y = row;
                            }
                        }
                    }
                }
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
                ImGui::TableSetColumnIndex(17);
                float verticalBorderX = ImGui::GetCursorPosX() -4 ;
                ImGui::GetWindowDrawList()->AddLine(
                    ImVec2(verticalBorderX, ImGui::GetWindowPos().y+10),
                    ImVec2(verticalBorderX, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y),
                    IM_COL32(255, 255, 255, 255) // Change color as needed
                );
                {
                    float rows_height = 5 * 2;
                    if (ImGui::BeginTable("table_nested2", 16, ImGuiTableFlags_NoBordersInBody))
                    {
                        //ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 20.0f);
                        for (int i = 1; i < 17; i++) {
                            
                            int index = row * 16 + (i - 1) ;
                            const bool item_is_selected = (selection_y == row && selection_x == i);
                            ImGui::TableNextColumn();
                            char buf[32];
                            sprintf_s(buf, sizeof(buf), "%c", filedata[index]);

                            if (ImGui::Selectable(buf, item_is_selected)) {}
                        }
                        ImGui::EndTable();
                    }
                }

            }
            ImGui::EndTable();
        }
        ImGui::End();
    }
    void ShowInfoFile(bool showInfoFile, const InformationFile& infoData)
    {
        if (showInfoFile) {
            ImGui::Begin("Information of File", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

            ImGui::Text("File Name: %s", infoData.file_name.c_str());
            ImGui::Text("File Path: %s", infoData.file_path.c_str());
            ImGui::Text("File Size: %d", infoData.file_size);
            ImGui::Text("File State: %s", infoData.file_state.c_str());
            ImGui::Text("Create Time: %s", infoData.file_create_time.c_str());
            ImGui::Text("Last Write Time: %s", infoData.file_lastw_time.c_str());
            ImGui::Text("File Mode: %s", infoData.file_mode.c_str());
            ImGui::Text("File Offset: %s", infoData.file_offset.c_str());
            ImGui::Text("Bytes per Page: %s", infoData.byte_per_page.c_str());

            ImGui::End();
        }
    }
    void ShowMenuFile()
    {
        ImGui::MenuItem("(MENU FILE)", NULL, false, false);
        if (ImGui::MenuItem("New")) {}
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
            
        }
        if (ImGui::MenuItem("Quit", "Alt+F4")) {}
        // You can use ImGui::MenuItem to create menu items with shortcuts
        // You can use ImGui::Separator to add separators
        // You can add functionality to the menu items as needed
    }
    void ShowAppMainMenuBar(std::vector<string> drivesVec, bool* loadDrive, std::string* currentDrive)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("(MENU FILE)", NULL, false, false);
                if (ImGui::MenuItem("New")) {}
                if (ImGui::BeginMenu("Open", "Ctrl+O")) {
                    for (std::string drive: drivesVec) {
                        if (ImGui::MenuItem(drive.c_str())) {
                            *loadDrive = true;
                            *currentDrive = drive;
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Quit", "Alt+F4")) {}
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
    void ShowFileContextMenu (MFT_RECORD record) {
        return;
    }

}