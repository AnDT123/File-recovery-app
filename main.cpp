#include <iostream>
#include <filesystem>
#include <windows.h>
#include <comutil.h>
#include <wbemcli.h>
#include <fileapi.h>
#include <winioctl.h>
#include <fileapi.h>
namespace fs = std::filesystem;
HANDLE GetVolumeHandleForFile(const wchar_t* filePath)
{
    wchar_t volume_path[MAX_PATH];
    if (!GetVolumePathName(filePath, volume_path, ARRAYSIZE(volume_path)))
        return nullptr;

    wchar_t volume_name[MAX_PATH];
    if (!GetVolumeNameForVolumeMountPoint(volume_path,
        volume_name, ARRAYSIZE(volume_name)))
        return nullptr;

    auto length = wcslen(volume_name);
    if (length && volume_name[length - 1] == L'\\')
        volume_name[length - 1] = L'\0';

    return CreateFile(volume_name, 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
}

bool IsFileOnSsd(const wchar_t* file_path)
{
    bool is_ssd{ false };
    HANDLE volume = GetVolumeHandleForFile(file_path);
    if (volume == INVALID_HANDLE_VALUE)
    {
        return false; /*invalid path! throw?*/
    }

    STORAGE_PROPERTY_QUERY query{};
    query.PropertyId = StorageDeviceSeekPenaltyProperty;
    query.QueryType = PropertyStandardQuery;
    DWORD count;
    DEVICE_SEEK_PENALTY_DESCRIPTOR result{};
    if (DeviceIoControl(volume, IOCTL_STORAGE_QUERY_PROPERTY,
        &query, sizeof(query), &result, sizeof(result), &count, nullptr))
    {
        is_ssd = !result.IncursSeekPenalty;
    }
    else { /*fails for network path, etc*/ }
    CloseHandle(volume);
    return is_ssd;
}

int main()
{
    fs::path diskpath;
    std::cout << "Input the name of Disk: ";
    std::cin >> diskpath;
    std::string Disk_path;
    Disk_path = diskpath.string();
    try {
        fs::space_info space = fs::space(diskpath);
        std::cout << "Total storage in " << diskpath << " is: " << space.capacity << " bytes" << std::endl;
        std::cout << "Remaining space on " << diskpath << " is: " << space.available << " bytes" << std::endl;

    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    //std::cout << Disk_path;
    //LPCWSTR name_drive = "L" + Disk_path;
    //Disk_path = "\"" + Disk_path + "\"";
    //Disk_path = "L" + Disk_path;
    //std::cout << Disk_path << std::endl;

    std::wstring tmp = std::wstring(Disk_path.begin(), Disk_path.end());
    LPCWSTR name_drive = tmp.c_str();
    //std::wcout << name_drive << std::endl;

    UINT test;
    test = GetDriveType(name_drive);
    //std::wcout << test << std::endl;
    switch (test)
    {

    case 0: printf("Drive %S is type %d - Cannot be determined.\n", name_drive, test);

        break;

    case 1: printf("Drive %S is type %d - Invalid root path/Not available.\n", name_drive, test);

        break;

    case 2: printf("Drive %S is type %d - Removable.\n", name_drive, test);

        break;

    case 3: printf("Drive %S is type %d - Fixed.\n", name_drive, test);

        break;

    case 4: printf("Drive %S is type %d - Network.\n", name_drive, test);

        break;

    case 5: printf("Drive %S is type %d - CD-ROM.\n", name_drive, test);

        break;

    case 6: printf("Drive %S is type %d - RAMDISK.\n", name_drive, test);

        break;

    default: "Unknown value!\n";
    }

    if (IsFileOnSsd(name_drive))
        std::wcout << "Partition is on A SSD" << std::endl;
    PARTITION_INFORMATION_EX pinfo;
    HANDLE fileHandle = GetVolumeHandleForFile(name_drive);

    DeviceIoControl(fileHandle, IOCTL_DISK_GET_PARTITION_INFO_EX, NULL, 0, &pinfo, sizeof(pinfo), NULL, nullptr);
    switch (pinfo.PartitionStyle) {
    case 0: std::cout << "Partition type is MBR" << std::endl;
        break;
    case 1: std::cout << "Partition type is GPT" << std::endl;
        break;
    default: std::cout << "Cannot determine" << std::endl;
    }
    //LPCSTR test = name_drive.c_str();
    //GetVolumeInformation(name_drive.c_str(),)
    char szVolumeName[256] = { 0 };
    char szFSName[256] = { 0 };
    unsigned long dwVolumeSN = 0, dwMaxComponentLen = 0, dwFSFlags = 0;
    if (!GetVolumeInformationA(Disk_path.c_str(), szVolumeName, 256, &dwVolumeSN,
        &dwMaxComponentLen, &dwFSFlags, szFSName, 256)) return 0;
    std::cout << "Partition format is: " << szFSName << std::endl;
    return 0;
}