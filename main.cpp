// Dear ImGui: standalone example application for DirectX 9

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <memory>
#include "Backend/Tool.h"
#include "Frontend/HomePage.h"
#include <map>
#include <fcntl.h>
#include <io.h>
#include <imgui_memory_editor.h>
#include <codecvt> 
#include "nativefiledialog/include/nfd.h"
// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    double num1 = 0;
    float sliderValue = 0.5f;
    double num2 = 0;
    double result = 0;
    char operation = '\0';
    char inputBuffer[64] = "";
    const int bufferSize = 64;
    bool isTyping = false;
    //bool showInfoFile = false;
    //std::vector<char> infoData;


    //------------------------------------------GET AVAILABLE LOGICAL DRIVE--------------------------------------------------
    char drivesBuff[255];
    DWORD numberOfDrives = GetLogicalDriveStringsA(sizeof(drivesBuff) - 1, drivesBuff);
    std::vector<string> drivesVec;
    if (numberOfDrives > 0) {
        char* currentDrive = drivesBuff;
        while (*currentDrive != '\0') {
            std::cout << "Drive: " << currentDrive << std::endl;
            drivesVec.push_back(std::string(currentDrive));
            currentDrive += strlen(currentDrive) + 1; // Move to the next drive
        }
    }
    else {
        std::cerr << "Failed to retrieve logical drives." << std::endl;
    }
    //-----------------------------------------END REGION-------------------------------------------------------------------
    //------------------------------------INITIALIZE FILE SYSTEM TREE VARIABLES----------------------------------------------
    bool loadDrive = false;
    bool isRunning = false;
    std::string currentDrive;
    std::string logicalDrivePath = "\\\\.\\0:";
    HANDLE diskHandle = new HANDLE;
    unsigned char buffer[4096];
    DWORD bytesRead;     
    OVERLAPPED ol{};

    bool flag;
    MFT_RECORD mft;
    vector<wstring> filename;
    vector<MFT_RECORD> mftRecords;
    FileSystemTree* tree = new FileSystemTree();
    // Main loop
    bool done = false;
    //-------------------------------VARIABLES FOR FILE NAVIGATION-----------------------------------------------------------
    DIR_NODE* parentNode = new DIR_NODE();
    std::vector<NODE*> traversePath;
    int selectedId = -1;
    int previousSelectedId = -1;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;


    //------------------------------------END REGION------------------------------------------------------------------------
    //------------------------------VARIABLES FOR RECOVER DATA STEP---------------------------------------------------------
    MFT_RECORD recoverRecord;
    //------------------------------------END REGION------------------------------------------------------------------------
    //-----------------------------------VARIABLE FOR HEX VIEWER---------------------------------------------------------
    std::vector<int> filedata;
    bool showFileData = false;
    //------------------------------------END REGION------------------------------------------------------------------------

    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        //---------------------------------
        HomePage::ShowAppMainMenuBar(drivesVec, &loadDrive,&currentDrive);
        if (loadDrive == true) {
            if (logicalDrivePath[4] != currentDrive[0]) {
                logicalDrivePath[4] = currentDrive[0];
                diskHandle= CreateFileA(
                    logicalDrivePath.c_str(),
                    GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS,
                    NULL
                );
                ol.Offset = 0xC0000 * 4096;
                ReadFile(diskHandle, buffer, sizeof(buffer), &bytesRead, &ol);
                MFT_RECORD mft = Tool::MFTRecordParser(buffer, 0, &flag);
                mftRecords = Tool::ParseRecordsWithMFTDatarun(diskHandle, mft);
                std::cout << mftRecords.size() << endl;
                tree = new FileSystemTree(mftRecords);
                parentNode = tree->GetRoot();
                std::vector<NODE*> newTraversePath;
                newTraversePath.push_back(parentNode);
                traversePath = newTraversePath;
            }
            isRunning = true;
            loadDrive = false;
        }
        ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 250), ImGuiCond_Always); 
        ImGui::Begin("MyComPuter", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        if (ImGui::BeginTabBar("MyTabBar", ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            
            if (isRunning) {
                if (ImGui::BeginTabItem(currentDrive.c_str())) {
                    ImGui::BeginTable("FileTable", 6, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody);

                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                    ImGui::TableSetupColumn("Date Created", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Date Modified", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Date Accessed", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);

                    ImGui::TableHeadersRow();
                    // Add table header
                    if (traversePath.size() > 1) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Selectable("..");
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            traversePath.pop_back();
                            parentNode = (DIR_NODE*) traversePath.back();
                        }

                    }

                    NODE* iterNode = parentNode->FirstChild;
                    if (traversePath.size() == 1) {
                        NODE fileInfo = *iterNode;
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        std::string fileName = converter.to_bytes(fileInfo.Name);
                        if (ImGui::Selectable(fileName.c_str(), selectedId == fileInfo.RecordId)) {
                            selectedId = fileInfo.RecordId;
                            if (selectedId != previousSelectedId) {
                                previousSelectedId = selectedId;
                            }
                        }
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            if (fileInfo.IsDirectory) {
                                traversePath.push_back(iterNode);
                                parentNode = (DIR_NODE*)iterNode;
                                selectedId = -1;
                            }
                        }
                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("0x%X", fileInfo.RecordId);
                        iterNode = iterNode->NextSibling;
                    }

                    // Display data in rows
                    while (iterNode != nullptr) {
                        NODE fileInfo = *iterNode;
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        std::string fileName = converter.to_bytes(fileInfo.Name);
                        if (ImGui::Selectable(fileName.c_str(), selectedId == fileInfo.RecordId)) {
                            selectedId = fileInfo.RecordId;
                            if (selectedId != previousSelectedId) {
                                ol.Offset = tree->FindRecordById(selectedId).offset;
                                ReadFile(diskHandle, buffer, sizeof(buffer), &bytesRead, &ol);
                                vector<int> fd;
                                for (int i = 0; i < sizeof(buffer); i++) {
                                    fd.push_back((int)buffer[i]);
                                }
                                filedata = fd;
                                previousSelectedId = selectedId;
                            }
                        }
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            if (fileInfo.IsDirectory) {
                                traversePath.push_back(iterNode);
                                parentNode = (DIR_NODE*)iterNode;
                                selectedId = -1;
                            }
                        }
                        if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                            if (!fileInfo.IsDirectory) {
                                recoverRecord = tree->FindRecordById(fileInfo.RecordId);
                                ImGui::OpenPopup("context-menu-popup");
                            }
                        }
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%llukb", fileInfo.Fnd.AllocatedSize/1000);
                        ImGui::TableSetColumnIndex(2);
                        std::string fileCreationDate = converter.to_bytes(Tool::GetDateTimeFromULONGLONG(fileInfo.Fnd.FileCreationTime));
                        ImGui::Text("%s", fileCreationDate.c_str());
                        ImGui::TableSetColumnIndex(3);
                        std::string fileAlteredTime = converter.to_bytes(Tool::GetDateTimeFromULONGLONG(fileInfo.Fnd.FileAlteredTime));
                        ImGui::Text("%s", fileAlteredTime.c_str());
                        ImGui::TableSetColumnIndex(4);
                        std::string fileReadTime = converter.to_bytes(Tool::GetDateTimeFromULONGLONG(fileInfo.Fnd.FileReadTime));
                        ImGui::Text("%s", fileReadTime.c_str());
                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%d", fileInfo.RecordId);
                        iterNode = iterNode->NextSibling;
                       // ImGui::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);
                    }
                    if (ImGui::BeginPopup("context-menu-popup"))
                    {
                        if (ImGui::Selectable("Go to datarun")) {
                            vector<FRAGMENT> datarun = recoverRecord.GetDataRun();
                            if (datarun.size() > 0) {
                                selectedId = recoverRecord.GetRecordId();
                                ol.Offset = datarun.at(0).FragmentOffset * 4096;
                                ReadFile(diskHandle, buffer, sizeof(buffer), &bytesRead, &ol);
                                vector<int> fd;
                                for (int i = 0; i < sizeof(buffer); i++) {
                                    fd.push_back((int)buffer[i]);
                                }
                                filedata = fd;
                                previousSelectedId = selectedId;
                            }
                        }
                        if (ImGui::Selectable("Recover to ..")) {
                            nfdchar_t* outPath = NULL;
                            nfdresult_t r = NFD_SaveDialog(NULL,NULL, &outPath);
                            if (outPath != nullptr && *outPath != '\0')
                                Tool::RecoverDataToFilePath(diskHandle, recoverRecord, outPath);
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::EndTable();
                    ImGui::EndTabItem();
                }
            }

            // Kết thúc tab bar
            ImGui::EndTabBar();
            if (selectedId != -1)
                showFileData = true;
            else
                showFileData = false;
            HomePage::ShowFileData(showFileData, filedata, 270, ol.Offset);
            ImGui::End();
        }
        //---------------------------------










        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
