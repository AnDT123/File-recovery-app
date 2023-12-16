#pragma once

#include "ntfs.h"
#include <memory>
#include <vector>
#include <iostream>
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
                //for (int i = 0; i < numOfAtt; i++) {
                //    if (attribute_list[i].TypeCode == 0x00000090) {
                //        return true;
                //    }
                //}
                //return false;
                return header.Flags == 0x0002 || header.Flags == 0x0003;
            }
        };
    class NODE {
    public:
        wchar_t* Name;
        ULONG RecordId;
        NODE* NextSibling;
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
        vector<MFT_RECORD> recordVec;

        MFT_RECORD findRecordById(ULONG id) {
            for (auto r : recordVec) {
                if (r.GetRecordId() == id)
                {
                    return r;
                }
            }
        }
        NODE* GetIdWithNode(DIR_NODE* node, ULONG id) {
            if (id == 0x00000005)
                return root;
            if (node->RecordId == id)
                return node;
            NODE* child = node->FirstChild;
            while (child != nullptr) {
                if (findRecordById(child->RecordId).IsDir()) {
                    auto result = GetIdWithNode((DIR_NODE*)child, id);
                    if (result != nullptr)
                        return result;
                }
                child = child->NextSibling;
            }
            return nullptr;
        }
        bool is_in_tree(ULONG id) {
            if (std::find(NodeIds.begin(), NodeIds.end(), id) != NodeIds.end())
                return true;
            else
                return false;
        }
        void insert(DIR_NODE* parent, NODE* child) {
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
        FileSystemTree(vector<MFT_RECORD> vec) {
            DIR_NODE* r = (DIR_NODE*)malloc(sizeof(DIR_NODE));
            r->Name = (wchar_t*)L"root";
            r->RecordId = 0x00000005;
            NodeIds.push_back(r->RecordId);
            r->FirstChild = nullptr;
            r->NextSibling = nullptr;
            root = r;
            recordVec = vec;
            for (auto record : vec) {
                for (auto fnd : record.GetFilenameVec()) {
                    if (record.GetRecordId() == 41)
                            int a = 0;
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
            int a = 0;
        }
        NODE* GetNodeFromId(ULONG id) {
            return GetIdWithNode(root, id);
        }
        void AddDirNodeToTree(FILE_NAME_DATA fnd, ULONG recordId) {
            //std::cout << "AddDirNodeToTree" << std::endl;
            if (recordId == 41)
                int a = 0;
            if (is_in_tree(recordId))
                return;
            NodeIds.push_back(recordId);
            DIR_NODE* parentNode = (DIR_NODE*)malloc(sizeof(DIR_NODE));
            DIR_NODE* childNode = nullptr;
            wchar_t* filename = (wchar_t*)malloc(fnd.FileNameLength * sizeof(wchar_t) + 2);
            memcpy(filename, fnd.FileName, fnd.FileNameLength * sizeof(wchar_t));
            filename[fnd.FileNameLength] = 0x00;
            parentNode->Name = filename;
            parentNode->RecordId = recordId;
            parentNode->FirstChild = nullptr;
            parentNode->NextSibling = nullptr;

            while (true) {
                recordId = fnd.ParentDirectoryReference.SegmentNumberLowPart;
                if (is_in_tree(recordId)) {
                    break;
                }
                MFT_RECORD currRecord = findRecordById(recordId);
                //-------------File Sequence Check here---------------
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
                NodeIds.push_back(recordId);
                
                childNode = parentNode;
                parentNode = node;
                parentNode->FirstChild = childNode;
            }
            insert((DIR_NODE*) GetNodeFromId(recordId), parentNode);
        }
        void AddFileNodeToTree(FILE_NAME_DATA fnd, ULONG recordId) {
            //std::cout << "AddFileNodeToTree" << std::endl;
            if (is_in_tree(recordId))
                return;
            auto parentId = fnd.ParentDirectoryReference.SegmentNumberLowPart;
            //-------------File Sequence Check here---------------
            // MFT_RECORD* currRecord = findRecordById(parentId);
            //---------------currRecord and fnd-------------------

            FILE_NODE *childNode = (FILE_NODE*) malloc(sizeof(FILE_NODE));
            wchar_t* filename = (wchar_t*)malloc(fnd.FileNameLength * sizeof(wchar_t) + 2);
            memcpy(filename, fnd.FileName, fnd.FileNameLength * sizeof(wchar_t));
            filename[fnd.FileNameLength] = 0x00;
            childNode->Name = filename;
            childNode->RecordId = recordId;
            childNode->NextSibling = nullptr;

            if (is_in_tree(parentId))
                insert((DIR_NODE*) GetNodeFromId(parentId), childNode);
            auto c = 1;
        }
    };
}
