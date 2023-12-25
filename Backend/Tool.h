#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <bitset>
#include "DataStructures.h"
using namespace DataStructures;
namespace Tool {
	std::vector<std::vector<unsigned char>> HEXA(unsigned char buffer[], int bufferSize);
	std::vector<std::vector<unsigned char>> InitializeFileData();
	std::unique_ptr <MFT_RECORD_ATTRIBUTE> MFTRecordAttributeParser(unsigned char buffer[], int offset, bool* success, int* attributeLength);
	MFT_RECORD MFTRecordParser(unsigned char buffer[], DWORD offset, bool * success);

	std::vector<MFT_RECORD> ParseRecordsWithMFTDatarun(HANDLE fileHandle, MFT_RECORD mft);
	wchar_t* GetDateTimeFromULONGLONG(ULONGLONG datetime);
	int RecoverDataToFilePath(HANDLE diskHandle, MFT_RECORD record, char* filePath);
}
