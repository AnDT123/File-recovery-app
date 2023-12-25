#pragma once

#include "ntfs.h"
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
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
            MFT_RECORD_ATTRIBUTE* attribute_list = nullptr;
            int numOfAtt =0 ;
            ULONG GetRecordId(){
                return header.RecordId;
            }
            ULONG GetBaseRecordId() {
                return header.BaseFileRecordSegment.SegmentNumberLowPart;
            }
            //USHORT GetBaseRecordSeqNumber() {
            //    return header.BaseFileRecordSegment.SequenceNumber;
            //}
            USHORT GetSeqNumber() {
                return header.SequenceNumber;
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
                return header.Flags == 0x0002 || header.Flags == 0x0003;
            }
            bool Is_in_use() {
                return header.Flags % 2 == 1;
            }
        };
    class NODE {
    public:
        wchar_t* Name;
        ULONG RecordId;
        NODE* NextSibling;
        FILE_NAME_DATA Fnd;
        bool IsDirectory = false;
    };
    class FILE_NODE : public NODE {

    };
    class DIR_NODE : public NODE {
    public:
        NODE* FirstChild;
    };
    class FileSystemTree {
    private:
        vector<ULONG> NodeIds;
        DIR_NODE* root;
        DIR_NODE* orphanFiles;
        vector<MFT_RECORD> recordVec;

        NODE* GetIdWithNode(DIR_NODE* node, ULONG id) {
            if (id == 0x00000005)
                return root;            
            if (id == 0xDEADBEEF)
                return orphanFiles;
            if (node->RecordId == id)
                return node;

            NODE* child = node->FirstChild;
            if(node == root)
                child = child->NextSibling;
            while (child != nullptr) {
                if (FindRecordById(child->RecordId).IsDir()) {
                    auto result = GetIdWithNode((DIR_NODE*)child, id);
                    if (result != nullptr)
                        return result;
                }
                child = child->NextSibling;
            }
            return nullptr;
        }
        bool Is_in_tree(ULONG id) {
            if (std::find(NodeIds.begin(), NodeIds.end(), id) != NodeIds.end())
                return true;
            else
                return false;
        }
        void Insert(DIR_NODE* parent, NODE* child) {
            if (parent->FirstChild == nullptr) {
                parent->FirstChild = child;
                return;
            }
            NODE* Next = parent->FirstChild;
            while (Next->NextSibling != nullptr) {
                Next = Next->NextSibling;
            }
            Next->NextSibling = child;
            //NodeIds.push_back(child->RecordId);
        }
    public:
        MFT_RECORD FindRecordById(ULONG targetId) {
            auto iterator = std::lower_bound(recordVec.begin(), recordVec.end(), targetId, [](const MFT_RECORD& record, ULONG target) {
                return record.header.RecordId < target;
                });
            if (iterator != recordVec.end() && iterator->GetRecordId() == targetId)
                return *iterator;
        }
        NODE* GetNodeFromId(ULONG id) {
            return GetIdWithNode(root, id);
        }
        void AddDirNodeToTree(FILE_NAME_DATA fnd, ULONG recordId) {
            if (Is_in_tree(recordId))
                return;
            DIR_NODE* childNode = nullptr;
            bool isOrphan = false;
            NodeIds.push_back(recordId);
            DIR_NODE* parentNode = (DIR_NODE*)malloc(sizeof(DIR_NODE));
            wchar_t* filename = (wchar_t*)malloc(fnd.FileNameLength * sizeof(wchar_t) + 2);
            memcpy(filename, fnd.FileName, fnd.FileNameLength * sizeof(wchar_t));
            filename[fnd.FileNameLength] = 0x00;
            parentNode->Name = filename;
            parentNode->Fnd = fnd;
            parentNode->RecordId = recordId;
            parentNode->FirstChild = nullptr;
            parentNode->NextSibling = nullptr;
            parentNode->IsDirectory = true;

            while (true) {
                recordId = fnd.ParentDirectoryReference.SegmentNumberLowPart;
                if (Is_in_tree(recordId)) {
                    break;
                }
                MFT_RECORD currRecord = FindRecordById(recordId);
                //-------------File Sequence Check here---------------
                if (currRecord.Is_in_use())
                    isOrphan = fnd.ParentDirectoryReference.SequenceNumber != currRecord.GetSeqNumber();
                else
                    isOrphan = fnd.ParentDirectoryReference.SequenceNumber != currRecord.GetSeqNumber() - 1;
                if (isOrphan) {
                    //Insert(orphanFiles, parentNode);
                    return;
                }
                //---------------currRecord and fnd-------------------
                vector<FILE_NAME_DATA> filenameVec = currRecord.GetFilenameVec();
                if (filenameVec.size() <= 0) {
                    return;
                }
                fnd = currRecord.GetFilenameVec().at(0);

                DIR_NODE* node = (DIR_NODE*) malloc(sizeof(DIR_NODE));
                wchar_t* filename = (wchar_t*)malloc(fnd.FileNameLength * sizeof(wchar_t) + 2);
                memcpy(filename, fnd.FileName, fnd.FileNameLength * sizeof(wchar_t));
                filename[fnd.FileNameLength] = 0x00;
                node->Name = filename;
                node->RecordId = recordId;
                node->NextSibling = nullptr;
                node->IsDirectory = true;
                node->Fnd = fnd;
                NodeIds.push_back(recordId);
                
                childNode = parentNode;
                parentNode = node;
                parentNode->FirstChild = childNode;
            }
            Insert((DIR_NODE*) GetNodeFromId(recordId), parentNode);
        }
        void AddFileNodeToTree(FILE_NAME_DATA fnd, ULONG recordId) {
            bool isOrphan = false;
            if (Is_in_tree(recordId))
                return;
            auto parentId = fnd.ParentDirectoryReference.SegmentNumberLowPart;

            FILE_NODE *childNode = (FILE_NODE*) malloc(sizeof(FILE_NODE));
            wchar_t* filename = (wchar_t*)malloc(fnd.FileNameLength * sizeof(wchar_t) + 2);
            memcpy(filename, fnd.FileName, fnd.FileNameLength * sizeof(wchar_t));
            filename[fnd.FileNameLength] = 0x00;
            childNode->Name = filename;
            childNode->RecordId = recordId;
            childNode->NextSibling = nullptr;
            childNode->Fnd = fnd;
            childNode->IsDirectory = false;
            //-------------File Sequence Check here---------------
            MFT_RECORD currRecord = FindRecordById(parentId);
            if (currRecord.Is_in_use())
                isOrphan = fnd.ParentDirectoryReference.SequenceNumber != currRecord.GetSeqNumber();
            else
                isOrphan = fnd.ParentDirectoryReference.SequenceNumber != currRecord.GetSeqNumber() - 1;
            if (isOrphan) {
                Insert(orphanFiles, childNode);
                return;
            }
            //---------------currRecord and fnd-------------------

            if (Is_in_tree(parentId))
                Insert((DIR_NODE*) GetNodeFromId(parentId), childNode);
        }
        DIR_NODE* GetRoot() {
            return root;
        }
        FileSystemTree(){}
        FileSystemTree(vector<MFT_RECORD> vec) {
            orphanFiles = (DIR_NODE*)malloc(sizeof(DIR_NODE));
            orphanFiles->Name = (wchar_t*)L"OrphanFiles";
            orphanFiles->NextSibling = nullptr;
            orphanFiles->RecordId = 0xDEADBEEF;
            orphanFiles->FirstChild = nullptr;
            orphanFiles->IsDirectory = true;

            root = (DIR_NODE*)malloc(sizeof(DIR_NODE));
            root->Name = (wchar_t*)L"root";
            root->RecordId = 0x00000005;
            NodeIds.push_back(root->RecordId);
            root->FirstChild = orphanFiles;
            root->NextSibling = nullptr;
            root->IsDirectory = true;

            recordVec = vec;
            std::sort(recordVec.begin(), recordVec.end(), [](const MFT_RECORD& a, const MFT_RECORD& b) {
                return a.header.RecordId < b.header.RecordId;
                });
            for (auto record : vec) {
                for (auto fnd : record.GetFilenameVec()) {
                    if (record.IsDir() && record.GetRecordId() != 0x00000005)
                        AddDirNodeToTree(fnd, record.GetRecordId());
                }
            }
            for (auto record : vec) {
                for (auto fnd : record.GetFilenameVec()) {
                    if (!record.IsDir())
                        AddFileNodeToTree(fnd, record.GetRecordId());
                }
            }
            //int a = 0;
        }
    };
}
