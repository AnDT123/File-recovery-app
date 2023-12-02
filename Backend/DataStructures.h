#pragma once

#include "ntfs.h"
#include <memory>
#include <vector>

using namespace std;
namespace DataStructures {
    typedef union {
        UCHAR* Name;
        UCHAR* Content;
    } NextRecordData;
    typedef struct FRAGMENT {
        ULONG NumberOfClusters = 0;
        ULONGLONG FragmentOffset= 0;
    };

    struct  MFT_RECORD_ATTRIBUTE {
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
                UCHAR Reserved[6];                                      //  offset = 0x022
                LONGLONG AllocatedLength;                               //  offset = 0x028
                LONGLONG FileSize;                                      //  offset = 0x030
                LONGLONG ValidDataLength;
                LONGLONG TotalAllocated;                                //  offset = 0x040
                NextRecordData NData;
            } Nonresident;
        };
    public:
        bool IsResident() {
            return FormCode == RESIDENT_FORM;
        }
        vector<FRAGMENT> GetFragments();
    };
    typedef struct FILE_NAME_DATA {
        FILE_REFERENCE ParentDirectoryReference;
        ULONGLONG FileCreationTime;
        ULONGLONG FileAlteredTime;
        ULONGLONG MFTChangedTime;
        ULONGLONG FileReadTime;
        ULONGLONG AllocatedSize;
        ULONGLONG RealSize;
        ULONG Flag;
        ULONG Used;
        UCHAR FileNameLength;
        UCHAR FileNamespace;
        UCHAR* FileName;
    public:
        void ParseFileData(UCHAR* PContent);
    };

    struct MFT_RECORD {
            DWORD offset;
            FILE_RECORD_SEGMENT_HEADER header;
            MFT_RECORD_ATTRIBUTE* attribute_list;
            int numOfAtt =0 ;
            ULONG GetRecordId(){
                return header.RecordId;
            }
            ULONG GetBaseRecordId() {
                return header.BaseFileRecordSegment.SegmentNumberLowPart;
            }
            ULONG GetBaseRecordSeqNumber() {
                return header.BaseFileRecordSegment.SequenceNumber;
            }
            vector<FILE_NAME_DATA> GetFilenameVec() {
                vector<FILE_NAME_DATA> vec;
                for (int i = 0; i < numOfAtt; i++) {
                    if (attribute_list[i].TypeCode == 0x00000030) {
                        FILE_NAME_DATA temp;
                        if (attribute_list[i].IsResident())
                            temp.ParseFileData(attribute_list[i].Resident.NData.Content);
                        vec.push_back(temp);
                    }
                }
                return vec;
            }
            vector<FRAGMENT> GetDataRun() {
                for (int i = 0; i < numOfAtt; i++) {
                    if (attribute_list[i].TypeCode == 0x00000080) {
                        return attribute_list[i].GetFragments();
                    }
                }
                return (vector<FRAGMENT>) NULL;
            }
            bool IsDir() {
                return header.Flags == 0x02 || header.Flags == 0x03;
            }
        };
}
