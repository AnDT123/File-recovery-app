#pragma once

#include "ntfs.h"
#include <vector>

using namespace std;
namespace DataStructures {
    typedef union {
        struct {
            vector<UCHAR>Content;
        } NoName;
        struct {
            vector<UCHAR>Name;
            vector<UCHAR>Content;
        } Named;
    } NextRecordData;
    typedef struct  {
        ATTRIBUTE_TYPE_CODE TypeCode;                                   //  offset = 0x000
        ULONG RecordLength;                                             //  offset = 0x004
        UCHAR FormCode;                                                 //  offset = 0x008
        UCHAR NameLength;                                               //  offset = 0x009
        USHORT NameOffset;                                              //  offset = 0x00A
        USHORT Flags;                                                   //  offset = 0x00C
        USHORT Instance;                                                //  offset = 0x00E
        union {
            struct {
                ULONG ValueLength;                                      //  offset = 0x010
                USHORT ValueOffset;                                     //  offset = 0x014
                UCHAR ResidentFlags;                                    //  offset = 0x016
                UCHAR Reserved;                                         //  offset = 0x017
                NextRecordData NData;
            } Resident;

            struct {
                VCN LowestVcn;                                          //  offset = 0x010
                VCN HighestVcn;                                         //  offset = 0x018
                USHORT MappingPairsOffset;                              //  offset = 0x020
                UCHAR CompressionUnit;                                  //  offset = 0x022
                UCHAR Reserved[5];                                      //  offset = 0x023
                LONGLONG AllocatedLength;                               //  offset = 0x028
                LONGLONG FileSize;                                      //  offset = 0x030
                LONGLONG ValidDataLength;
                LONGLONG TotalAllocated;                                //  offset = 0x040
                NextRecordData NData;
            } Nonresident;
        };

    } MFT_RECORD_ATTRIBUTE;
    typedef struct {
        FILE_RECORD_SEGMENT_HEADER header;
        vector<MFT_RECORD_ATTRIBUTE> attribute_list;
    } MFT_RECORD;
}
