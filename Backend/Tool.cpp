#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <vector>
#include "imgui.h"
#include <memory>
#include "DataStructures.h"
using namespace DataStructures;
namespace Tool {
    std::vector<std::vector<unsigned char>> InitializeFileData() {
        std::vector<std::vector<unsigned char>> filedata;
        // Sample data (you can replace this with your actual data)
        const char* sampleData[] = {
            "0123456789ABCDEF",
            "FEDCBA9876543210",
            "A1B2C3D4E5F67890",
            "1F2E3D4C5B6A7980"
            // Add more rows as needed
        };

        // Convert the sample data to unsigned char and store in the 2D vector
        for (const char* row : sampleData) {
            std::vector<unsigned char> rowData(row, row + strlen(row));
            filedata.push_back(rowData);
        }

        return filedata;
    }
    MFT_RECORD_ATTRIBUTE* MFTRecordAttributeParser(unsigned char buffer[], int offset, bool* success, int* attributeLength) {
        if (offset >= 1024) {
            *success = false;
            return nullptr;
        }
        unsigned char* startOffset = buffer + offset;
        MFT_RECORD_ATTRIBUTE* mra = (MFT_RECORD_ATTRIBUTE*) malloc(sizeof(MFT_RECORD_ATTRIBUTE));
        memcpy(&(mra->TypeCode), startOffset, 4);
        //check false
        if (mra->TypeCode == 0xFFFFFFFF || !(mra->TypeCode == 0x00000010 || mra->TypeCode == 0x00000030 || mra->TypeCode == 0x00000080 || mra->TypeCode == 0x00000090) ) {
            *success = false;
            return nullptr;
        }
        memcpy(&(mra->RecordLength), startOffset + 0x4, 4);
        if (mra->RecordLength > 1024) {
            *success = false;
            return nullptr;
        }
        *attributeLength = mra->RecordLength;
        memcpy(&(mra->FormCode), startOffset + 0x8, 1);
        memcpy(&(mra->NameLength), startOffset + 0x9, 1);
        memcpy(&(mra->NameOffset), startOffset + 0x0A, 2);
        memcpy(&(mra->Flags), startOffset + 0x0C, 2);
        memcpy(&(mra->Instance), startOffset + 0x0E, 2);
        if (mra->FormCode == RESIDENT_FORM) {
            memcpy(&(mra->Resident.ValueLength), startOffset + 0x10, 4);
            memcpy(&(mra->Resident.ValueOffset), startOffset + 0x14, 2);
            memcpy(&(mra->Resident.ResidentFlags), startOffset + 0x16, 1);
            memcpy(&(mra->Resident.Reserved), startOffset + 0x17, 1);
            *success = (mra->Resident.ValueLength) > 0 && (mra->Resident.ValueLength < (1024 - offset)) && (mra->NameLength < mra->Resident.ValueLength < (1024 - offset));
            if (!(*success)) 
                return nullptr;
            
            if (mra->NameLength > 0) {
                mra->Resident.NData.Name = (UCHAR*) malloc (mra->NameLength * 2 * sizeof(UCHAR));
                for (int i = 0; i < mra->NameLength * 2; i++) {
                    memcpy(&mra->Resident.NData.Name[i], startOffset + mra->NameOffset + i, sizeof(UCHAR));
                }
                
            }
            mra->Resident.NData.Content = (UCHAR*) malloc (mra->Resident.ValueLength * sizeof(UCHAR));
            for (int i = 0; i < mra->Resident.ValueLength; i++) {
                memcpy(&mra->Resident.NData.Content[i], startOffset + mra->Resident.ValueOffset + i, sizeof(UCHAR));
            }
        }
        else if (mra->FormCode == NONRESIDENT_FORM) {
            memcpy(&(mra->Nonresident.LowestVcn), startOffset + 0x10, 8);
            memcpy(&(mra->Nonresident.HighestVcn), startOffset + 0x18, 8);
            memcpy(&(mra->Nonresident.MappingPairsOffset), startOffset + 0x20, 2);
            memcpy(&(mra->Nonresident.Reserved), startOffset + 0x22, 6);
            memcpy(&(mra->Nonresident.AllocatedLength), startOffset + 0x28, 8);
            memcpy(&(mra->Nonresident.FileSize), startOffset + 0x28, 8);
            memcpy(&(mra->Nonresident.ValidDataLength), startOffset + 0x30, 8);
            memcpy(&(mra->Nonresident.TotalAllocated), startOffset + 0x38, 8);
            if (mra->NameLength > 0) {
                mra->Nonresident.NData.Name  = (UCHAR*) malloc( mra->NameLength * 2 * sizeof(UCHAR));
                for (int i = 0; i < mra->NameLength * 2; i++) {
                    memcpy(&mra->Nonresident.NData.Name[i], startOffset + mra->NameOffset + i, sizeof(UCHAR));
                }
              
            }
            int dataRunsLength = mra->RecordLength - mra->Nonresident.MappingPairsOffset;
            *success = dataRunsLength < (1024 - offset) && (mra->NameLength < (1024 - offset));
            mra->Nonresident.NData.Content = (UCHAR*) malloc( (dataRunsLength) * sizeof(UCHAR));
            //for loop through datarun
            for (int i = 0; i < dataRunsLength; i++) {
                memcpy(&mra->Nonresident.NData.Content[i], startOffset + mra->Nonresident.MappingPairsOffset + i, sizeof(UCHAR));
            }
            
        }
        return mra;

    }
    MFT_RECORD MFTRecordParser(unsigned char buffer[], DWORD offset, bool* success) {
        MFT_RECORD record;
        record.offset = offset;
        record.numOfAtt = 0;
        if (buffer[0] != 0x46 || buffer[1] != 0x49 || buffer[2] != 0x4C || buffer[3] != 0x45 || (buffer[4] == '_' && buffer[5] == 'D')) {
            *success = false;
            return record;
        }
        try {
            memcpy(&(record.header.MultiSectorHeader.Signature), buffer, 4);
            memcpy(&(record.header.MultiSectorHeader.UpdateSequenceArrayOffset), buffer + 0x04, 2);
            memcpy(&(record.header.MultiSectorHeader.UpdateSequenceArraySize), buffer + 0x06, 2);
            memcpy(&(record.header.Lsn), buffer + 0x08, 8);
            memcpy(&(record.header.SequenceNumber), buffer + 0x10, 2);
            memcpy(&(record.header.ReferenceCount), buffer + 0x12, 2);
            memcpy(&(record.header.FirstAttributeOffset), buffer + 0x14, 2);
            memcpy(&(record.header.Flags), buffer + 0x16, 2);
            memcpy(&(record.header.FirstFreeByte), buffer + 0x18, 4);
            memcpy(&(record.header.BytesAvailable), buffer + 0x1C, 4);
            memcpy(&(record.header.BaseFileRecordSegment), buffer + 0x20, 8);
            memcpy(&(record.header.NextAttributeInstance), buffer + 0x28, 2);
            memcpy(&(record.header.Allign), buffer + 0x2A, 2);
            memcpy(&(record.header.RecordId), buffer + 0x2C, 4);

            if (record.header.MultiSectorHeader.UpdateSequenceArraySize * 2 >= 1024 - record.header.MultiSectorHeader.UpdateSequenceArrayOffset
                || record.header.MultiSectorHeader.UpdateSequenceArraySize * 2 >= 1024
                || record.header.MultiSectorHeader.UpdateSequenceArrayOffset >= 1024) {
                *success = false;
                return record;
            }
            memcpy(&(record.header.UpdateArrayForCreateOnly), buffer + record.header.MultiSectorHeader.UpdateSequenceArrayOffset, record.header.MultiSectorHeader.UpdateSequenceArraySize * 2);

            bool attributeFlag = true;
            int nextAttributeOffset = record.header.FirstAttributeOffset;
            int len = 0;
            int numOfAttributes = 0;
            while (attributeFlag) {
                auto attr = MFTRecordAttributeParser(buffer, nextAttributeOffset, &attributeFlag, &len);
                if (attributeFlag) {
                    record.numOfAtt++;
                    if (record.numOfAtt == 1)
                        record.attribute_list = (MFT_RECORD_ATTRIBUTE*)malloc(sizeof(MFT_RECORD_ATTRIBUTE));
                    else
                        record.attribute_list = (MFT_RECORD_ATTRIBUTE*)realloc(record.attribute_list, record.numOfAtt * sizeof(MFT_RECORD_ATTRIBUTE));
                    memcpy(&record.attribute_list[record.numOfAtt - 1], attr, sizeof(MFT_RECORD_ATTRIBUTE));
                }
                else
                    break;
                free(attr);
                nextAttributeOffset += len;
            }
        }
        catch (...) {
            *success = false;
        }
        return record;
    }
    std::vector<int> NHIPHAN(std::string filename) {
        std::ifstream file;
        // Mở tệp tin để đọc
        file.open(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Không thể mở tệp tin " << filename << " để đọc." << std::endl;
            exit;
        }

        // Đọc toàn bộ nội dung của tệp tin vào vector
        std::vector<char> binary_data(std::istreambuf_iterator<char>(file), {});
        // Tạo một vector để lưu trữ mã nhị phân dưới dạng chuỗi
        std::vector<std::string> binary_string_vector;
        // In ra mã nhị phân
      //  std::cout << "Mã nhị phân đọc được:" << std::endl;
        for (const char& byte : binary_data) {
            //    std::cout << std::bitset<8>(byte).to_string() << " ";
            std::string binary_string = std::bitset<8>(byte).to_string();
            binary_string_vector.push_back(binary_string);
        }
        // Chuyển vector chuỗi thành vector kiểu char
        std::vector<char> char_vector;
        for (const auto& binary_string : binary_string_vector) {
            const char* char_array = binary_string.c_str();
            char_vector.insert(char_vector.end(), char_array, char_array + binary_string.size());
        }
        // Tạo một vector int để lưu giá trị kiểu int tương ứng
        std::vector<int> int_vector;
        // Chuyển đổi từng ký tự char sang kiểu int và lưu vào vector int
        for (const char& c : char_vector) {
            if (c == '0' || c == '1') {
                int value = c - '0'; // Chuyển đổi từ ký tự sang giá trị int (1 hoặc 0)
                int_vector.push_back(value);
            }
            else {
                // Xử lý trường hợp ký tự không phải '0' hoặc '1'
                std::cerr << "Lỗi: Ký tự không hợp lệ - " << c << std::endl;
                // Có thể bỏ qua hoặc xử lý theo nhu cầu của bạn
            }
        }

        // Đóng tệp tin (đóng tự động khi ra khỏi phạm vi)
        file.close();
        return int_vector;
    }
    std::vector<int> groupBits(const std::vector<int>& inputVector) {
        std::vector<int> groupedVector;

        // Đảm bảo độ dài của vector là bội số của 4
        size_t vectorLength = inputVector.size();
        if (vectorLength % 4 != 0) {
            // Nếu không phải là bội số của 4, cắt bớt phần dư
            vectorLength -= vectorLength % 4;
        }

        // Nhóm 4 bit và thêm vào groupedVector
        for (size_t i = 0; i < vectorLength; i += 4) {
            // Chuyển 4 bit thành một số và thêm vào groupedVector
            int groupValue = 0;
            for (size_t j = 0; j < 4; ++j) {
                groupValue = (groupValue << 1) | inputVector[i + j];
            }
            groupedVector.push_back(groupValue);
        }

        return groupedVector;
    }

    std::vector<MFT_RECORD> ParseRecordsWithMFTDatarun(HANDLE fileHandle, MFT_RECORD mft) {
        vector<FRAGMENT> datarun = mft.GetDataRun();
        vector<MFT_RECORD> mftRecords;
        for (FRAGMENT f : datarun) {
            const int fragmentSize = f.NumberOfClusters;
            unsigned char fragment[1024];
            DWORD bytesRead;
            OVERLAPPED o{};
            // Set the offset from the start of the file
            for (int i = 0; i < f.NumberOfClusters * 4; i++) {
                bool success = true;
                o.Offset = f.FragmentOffset * 4096 + i * 1024;
                std::cout << std::hex << o.Offset << std::endl;
                ReadFile(fileHandle, fragment, sizeof(fragment), &bytesRead, &o);
                MFT_RECORD record = Tool::MFTRecordParser(fragment, o.Offset, &success);
                if (success)
                    mftRecords.push_back(record);
            }
        }
        return mftRecords;
    }

    wchar_t* GetDateTimeFromULONGLONG(ULONGLONG datetime) {
        FILETIME fileTime;
        fileTime.dwLowDateTime = static_cast<DWORD>(datetime & 0xFFFFFFFF);
        fileTime.dwHighDateTime = static_cast<DWORD>(datetime >> 32);
        SYSTEMTIME systemTime;
        if (FileTimeToSystemTime(&fileTime, &systemTime)) {
            wchar_t* result = new wchar_t[256] {0};
            // Format date
            wchar_t dateFormat[128];
            if (GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime, L"yyyy-MM-dd", dateFormat, sizeof(dateFormat) / sizeof(dateFormat[0]))) {
                // Format time
                wchar_t timeFormat[128];
                if (GetTimeFormat(LOCALE_USER_DEFAULT, 0, &systemTime, L"HH:mm:ss", timeFormat, sizeof(timeFormat) / sizeof(timeFormat[0]))) {
                    // Display the formatted date and time using ImGui
                    wcscat(result, dateFormat);
                    wcscat(result, L" ");
                    wcscat(result, timeFormat);
                    return result;
                }
            }

        }
    }
    bool fileExists(char* filePath) {
        struct stat buffer;
        return (stat(filePath, &buffer) == 0);
    }
    int RecoverDataToFilePath(HANDLE diskHandle, MFT_RECORD record, char* filePath) {
        if (filePath == nullptr || *filePath == '\0') return 0;
        if (fileExists(filePath)) {
            std::cerr << "File already exists: " << filePath << std::endl;
            return 0;
        }
        std::fstream outputFile(filePath, std::ios::out | std::ios::app | std::ios::binary);
        
        vector<FRAGMENT> datarun = record.GetDataRun();
        for (FRAGMENT f : datarun) {
            const int fragmentSize = f.NumberOfClusters;
            char fragment[1024];
            DWORD bytesRead;
            OVERLAPPED o{};
            // read data as 1024-byte-fragments
            for (int i = 0; i < f.NumberOfClusters * 4; i++) {
                bool success = true;
                o.Offset = f.FragmentOffset * 4096 + i * 1024;
                ReadFile(diskHandle, fragment, sizeof(fragment), &bytesRead, &o);
                // Write the fragment buffer to the file    
                for (int x = 0; x < bytesRead; x++) {
                    outputFile << fragment[x];
                }
            }
        }
        // Check for write errors
        if (outputFile.fail()) {
            std::cerr << "Failed to write data to file: " << filePath << std::endl;
            outputFile.close();
            return false;
        }

        // Close the file
        outputFile.close();

        std::cout << "Data successfully written to file: " << filePath << std::endl;
        return true;

    }
}