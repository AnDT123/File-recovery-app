#include  "DataStructures.h"

namespace DataStructures {
    // return filename content if the attribute is of type $FILENAME
	void FILE_NAME_DATA::ParseFileData(UCHAR * PContent) {
        memcpy(&ParentDirectoryReference, PContent, 8);
        memcpy(&FileCreationTime, PContent + 0x08, 8);
        memcpy(&FileAlteredTime, PContent + 0x10, 8);
        memcpy(&MFTChangedTime, PContent + 0x18, 8);
        memcpy(&FileReadTime, PContent + 0x20, 8);
        memcpy(&AllocatedSize, PContent + 0x28, 8);
        memcpy(&RealSize, PContent + 0x30, 8);
        memcpy(&Flag, PContent + 0x38, 4);
        memcpy(&Used, PContent + 0x3C, 4);
        memcpy(&FileNameLength, PContent + 0x40, 1);
        memcpy(&FileNamespace, PContent + 0x41, 1);
        FileName = (UCHAR*)malloc(FileNameLength * 2 * sizeof(UCHAR));
        for (int i = 0; i < FileNameLength * 2; i++) {
            memcpy(&FileName[i], PContent + 0x42 + i, sizeof(UCHAR));
        }

    }
    // return dataruns if the attribute is of type $DATA
    vector<FRAGMENT> MFT_RECORD_ATTRIBUTE::GetFragments() {
        vector<FRAGMENT> v;
        UCHAR *offset = this->Nonresident.NData.Content;
        UCHAR IndexByte = *offset;
        while (IndexByte != 0x00) {
            FRAGMENT temp;
            int low4Bits = IndexByte & 0x0F;

            int high4Bits = (IndexByte >> 4) & 0x0F;

            memcpy(&temp.NumberOfClusters, offset + 1, low4Bits);
            memcpy(&temp.FragmentOffset, offset + 1+ low4Bits, high4Bits);

            v.push_back(temp);
            offset = offset + 1 + low4Bits + high4Bits;
            IndexByte = *offset;
        }
        return v;
    };
}