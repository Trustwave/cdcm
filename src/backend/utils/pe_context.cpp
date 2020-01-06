//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														pe_context.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/21/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================

#include "pe_context.hpp"
#include <unordered_set>
#include <string>
#include <string_view>
#include <locale>

#include <boost/algorithm/string.hpp>
#include <iostream>
using namespace trustwave;
namespace {
    template<typename T>
    T *malloc_helper(const size_t count = 1) {
        return reinterpret_cast<T *>( malloc(count * sizeof(T)));
    }

    template<typename T, typename P>
    T *ptr_add(P *p, size_t o) {
        return reinterpret_cast<T *>(((char *) (p) + (o)));
    }
    template<typename T, typename P>
    T *ptr_dec(P *p, size_t o) {
        return reinterpret_cast<T *>(((char *) (p) - (o)));
    }
    template<typename T>
    bool  allocate_elements(uint32_t e_count,T**& dest_arr,void*& dest_ptr)
    {
        if (e_count> 0) {
            dest_arr = malloc_helper<T *>(e_count);
            if (dest_arr == nullptr)
            {
                return false;
            }
            for (uint32_t i = 0; i <e_count; i++) {
                dest_arr[i] = ptr_add<T>(dest_ptr,i * sizeof(T));
            }
        } else {
            dest_ptr = nullptr;
        }
        return true;
    }
    template<typename T>
    void  optional_header_parse(pe_file_t& pe,T*& p)
    {
        p = reinterpret_cast<T *>(pe.optional_hdr_ptr);
        pe.optional_hdr.length = sizeof(T);
        pe.num_directories = p->NumberOfRvaAndSizes;
        pe.entrypoint = p->AddressOfEntryPoint;
        pe.imagebase = p->ImageBase;
    }
    template< typename P>
    auto calculate_padding(P *p,const char* data) {
        return (4 - ((reinterpret_cast<char*>(p)-data)%4))%4;

    }

    using WORD=uint16_t;

#pragma pack(push, 1)

    typedef struct {
        WORD  wLength;
        WORD  wValueLength;//in words
        WORD  wType;

    }version_info_header;
#pragma pack(pop)

//    output_node_t *showNode(const NODE_PERES *node, output_node_t *output) {
//        switch (node->nodeType) {
//            default:
//                return nullptr;
//            case RDT_RESOURCE_DIRECTORY: {
//                const IMAGE_RESOURCE_DIRECTORY *resourceDirectory = node->resource.resourceDirectory;
//                output->kind = RDT_RESOURCE_DIRECTORY;
//                output->node_type.resourcesDirectory.NodeType = node->nodeLevel;
//                output->node_type.resourcesDirectory.Characteristics = resourceDirectory->Characteristics;
//                output->node_type.resourcesDirectory.TimeDateStamp = resourceDirectory->TimeDateStamp;
//                output->node_type.resourcesDirectory.MajorVersion = resourceDirectory->MajorVersion;
//                output->node_type.resourcesDirectory.MinorVersion = resourceDirectory->MinorVersion;
//                output->node_type.resourcesDirectory.NumberOfNamedEntries = resourceDirectory->NumberOfNamedEntries;
//                output->node_type.resourcesDirectory.NumberOfIdEntries = resourceDirectory->NumberOfIdEntries;
//                break;
//            }
//            case RDT_DIRECTORY_ENTRY: {
//                const IMAGE_RESOURCE_DIRECTORY_ENTRY *directoryEntry = node->resource.directoryEntry;
//                output->kind = RDT_DIRECTORY_ENTRY;
//                output->node_type.directoryEntry.NodeType = node->nodeLevel;
//                output->node_type.directoryEntry.NameOffset = directoryEntry->DirectoryName.name.NameOffset;
//                output->node_type.directoryEntry.NameIsString = directoryEntry->DirectoryName.name.NameIsString;
//                output->node_type.directoryEntry.OffsetIsDirectory = directoryEntry->DirectoryData.data.OffsetToDirectory;
//                output->node_type.directoryEntry.DataIsDirectory = directoryEntry->DirectoryData.data.DataIsDirectory;
//
//                break;
//            }
//            case RDT_DATA_STRING: {
//                const IMAGE_RESOURCE_DATA_STRING *dataString = node->resource.dataString;
//                output->kind = RDT_DATA_STRING;
//                output->node_type.dataString.NodeType = node->nodeLevel;
//                output->node_type.dataString.Strlen = dataString->length;
//                output->node_type.dataString.String = dataString->string[0];
//                break;
//            }
//            case RDT_DATA_ENTRY: {
//                const IMAGE_RESOURCE_DATA_ENTRY *dataEntry = node->resource.dataEntry;
//                output->kind = RDT_DATA_ENTRY;
//                output->node_type.dataEntry.NodeType = node->nodeLevel;
//                output->node_type.dataEntry.OffsetToData = dataEntry->offsetToData;
//                output->node_type.dataEntry.Size = dataEntry->size;
//                output->node_type.dataEntry.CodePage = dataEntry->codePage;
//                output->node_type.dataEntry.CodePage = dataEntry->codePage;
//                output->node_type.dataEntry.Reserved = dataEntry->reserved;
//                break;
//            }
//        }
//
//        return output;
//    }

//    count_output_node_t countNode(NODE_PERES *node) {
//        count_output_node_t count;
//        memset(&count, 0, sizeof(count_output_node_t));
//
//        switch (node->nodeType) {
//            default:
//                return count;
//            case RDT_RESOURCE_DIRECTORY:
//                count.kind = RDT_RESOURCE_DIRECTORY;
//                break;
//            case RDT_DIRECTORY_ENTRY:
//                count.kind = RDT_DIRECTORY_ENTRY;
//                break;
//            case RDT_DATA_STRING:
//                count.kind = RDT_DATA_STRING;
//                break;
//            case RDT_DATA_ENTRY:
//                count.kind = RDT_DATA_ENTRY;
//                break;
//        }
//
//        return count;
//    }

//    pe_resources_count_t get_count(NODE_PERES *node) {
//        pe_resources_count_t count;
//        int resourcesDirectory = 0;
//        int directoryEntry = 0;
//        int dataString = 0;
//        int dataEntry = 0;
//
//        count_output_node_t output;
//        while (node->lastNode != nullptr) {
//            node = node->lastNode;
//        }
//
//        while (node != nullptr) {
//            output = countNode(node);
//            if (output.kind == RDT_RESOURCE_DIRECTORY)
//                resourcesDirectory++;
//            if (output.kind == RDT_DIRECTORY_ENTRY)
//                directoryEntry++;
//            if (output.kind == RDT_DATA_STRING)
//                dataString++;
//            if (output.kind == RDT_DATA_ENTRY)
//                dataEntry++;
//            node = node->nextNode;
//        }
//        count.resourcesDirectory = resourcesDirectory;
//        count.directoryEntry = directoryEntry;
//        count.dataString = dataString;
//        count.dataEntry = dataEntry;
//        return count;
//    }

    void freeNodes(NODE_PERES *currentNode) {
        if (currentNode == nullptr)
            return;

        while (currentNode->nextNode != nullptr) {
            currentNode = currentNode->nextNode;
        }

        while (currentNode != nullptr) {
            if (currentNode->lastNode == nullptr) {
                free(currentNode);
                break;
            } else {
                currentNode = currentNode->lastNode;
                if (currentNode->nextNode != nullptr)
                    free(currentNode->nextNode);
            }
        }
    }

    const NODE_PERES *lastNodeByTypeAndLevel(const NODE_PERES *currentNode, NODE_TYPE_PERES nodeTypeSearch,
                                             NODE_LEVEL_PERES nodeLevelSearch) {
        assert(currentNode != nullptr);

        if (currentNode->nodeType == nodeTypeSearch && currentNode->nodeLevel == nodeLevelSearch)
            return currentNode;

        while (currentNode != nullptr) {
            currentNode = currentNode->lastNode;
            if (currentNode != nullptr && currentNode->nodeType == nodeTypeSearch &&
                currentNode->nodeLevel == nodeLevelSearch)
                return currentNode;
        }

        return nullptr;
    }

    const NODE_PERES *lastNodeByType(const NODE_PERES *currentNode, NODE_TYPE_PERES nodeTypeSearch) {
        assert(currentNode != nullptr);
        if (currentNode->nodeType == nodeTypeSearch)
            return currentNode;

        while (currentNode != nullptr) {
            currentNode = currentNode->lastNode;
            if (currentNode != nullptr && currentNode->nodeType == nodeTypeSearch)
                return currentNode;
        }

        return nullptr;
    }

    NODE_PERES *createNode(NODE_PERES *currentNode, NODE_TYPE_PERES typeOfNextNode) {
        assert(currentNode != nullptr);

        auto newNode = (NODE_PERES *) malloc(sizeof(NODE_PERES));
        if (newNode == nullptr) {
            return nullptr;
        }

        memset(newNode, 0, sizeof(NODE_PERES));
        newNode->lastNode = currentNode;
        newNode->nextNode = nullptr;
        newNode->nodeType = typeOfNextNode;
        currentNode->nextNode = newNode;
        return newNode;
    }
}



int pe_context::parse() {
    fm_.map_chunk(0, 64);
    pe_.dos_hdr = reinterpret_cast<IMAGE_DOS_HEADER *>( fm_.data());
    if (pe_.dos_hdr->e_magic != MAGIC_MZ)
    {
        return LIBPE_E_NOT_A_PE_FILE;
    }
    const auto signature_ptr = ptr_add<uint32_t>(pe_.dos_hdr,pe_.dos_hdr->e_lfanew);
    if (!fm_.map_chunk_by_pointer(signature_ptr, LIBPE_SIZEOF_MEMBER(pe_file_t, signature))) {
        return LIBPE_E_INVALID_LFANEW;
    }
    // NT signature (PE\0\0), or 16-bit Windows NE signature (NE\0\0).
    pe_.signature = *signature_ptr;
    switch (pe_.signature) {
        default:
            return LIBPE_E_INVALID_SIGNATURE;
        case SIGNATURE_NE:
        case SIGNATURE_PE:
            break;
    }
    pe_.coff_hdr = ptr_add<IMAGE_COFF_HEADER>((void *) signature_ptr,
                                              LIBPE_SIZEOF_MEMBER(pe_file_t, signature));

    if (!fm_.map_chunk_by_pointer(pe_.coff_hdr, sizeof(IMAGE_COFF_HEADER)))
        return LIBPE_E_MISSING_COFF_HEADER;

    pe_.num_sections = pe_.coff_hdr->NumberOfSections;

    // Optional header points right after the COFF header.
    pe_.optional_hdr_ptr = ptr_add<void>(pe_.coff_hdr,
                                         sizeof(IMAGE_COFF_HEADER));

    // Figure out whether it's a PE32 or PE32+.
    auto *opt_type_ptr = reinterpret_cast<uint16_t *>( pe_.optional_hdr_ptr);
    if (!fm_.map_chunk_by_pointer(opt_type_ptr, LIBPE_SIZEOF_MEMBER(IMAGE_OPTIONAL_HEADER, type)))
        return LIBPE_E_MISSING_OPTIONAL_HEADER;

    pe_.optional_hdr.type = *opt_type_ptr;
    switch (pe_.optional_hdr.type) {
        default:
        case MAGIC_ROM:
            return LIBPE_E_UNSUPPORTED_IMAGE;
        case MAGIC_PE32:
            if (!fm_.map_chunk_by_pointer(pe_.optional_hdr_ptr,
                                          sizeof(IMAGE_OPTIONAL_HEADER_32)))
                return LIBPE_E_MISSING_OPTIONAL_HEADER;
            optional_header_parse<IMAGE_OPTIONAL_HEADER_32>(pe_,pe_.optional_hdr._32);
            break;
        case MAGIC_PE64:
            if (!fm_.map_chunk_by_pointer(pe_.optional_hdr_ptr,
                                          sizeof(IMAGE_OPTIONAL_HEADER_64)))
                return LIBPE_E_MISSING_OPTIONAL_HEADER;
            optional_header_parse<IMAGE_OPTIONAL_HEADER_64>(pe_,pe_.optional_hdr._64);
            break;
    }

    if (pe_.num_directories > MAX_DIRECTORIES) {
        return LIBPE_E_TOO_MANY_DIRECTORIES;
    }

    if (pe_.num_sections > MAX_SECTIONS) {
        return LIBPE_E_TOO_MANY_SECTIONS;
    }

    pe_.directories_ptr = ptr_add<void>(pe_.optional_hdr_ptr,
                                        pe_.optional_hdr.length);

    if (!fm_.map_chunk_by_pointer(pe_.directories_ptr,
                                  sizeof(IMAGE_DATA_DIRECTORY) * pe_.num_directories))
        return LIBPE_E_ALLOCATION_FAILURE;
    uint32_t sections_offset = LIBPE_SIZEOF_MEMBER(pe_file_t, signature)
                               + sizeof(IMAGE_FILE_HEADER)
                               + pe_.coff_hdr->SizeOfOptionalHeader;
    pe_.sections_ptr = ptr_add<void>(signature_ptr, sections_offset);
    if (!fm_.map_chunk_by_pointer(pe_.sections_ptr, sizeof(IMAGE_SECTION_HEADER) * pe_.num_sections))
        return LIBPE_E_ALLOCATION_FAILURE;

    if (!allocate_elements<IMAGE_DATA_DIRECTORY>(pe_.num_directories,pe_.directories,pe_.directories_ptr))
    {
        return LIBPE_E_ALLOCATION_FAILURE;
    }
    if (!allocate_elements<IMAGE_SECTION_HEADER>(pe_.num_sections,pe_.sections,pe_.sections_ptr))
    {
        return LIBPE_E_ALLOCATION_FAILURE;
    }

    return 0;
}

uint64_t pe_context::pe_rva2ofs(uint64_t rva) {
    // Converts a RVA (Relative Virtual Address) to a raw file offset
    if (rva == 0 || pe_.sections == nullptr) {
        return 0;
    }

    // Find out which section the given RVA belongs
    for (uint32_t i = 0; i < pe_.num_sections; i++) {
        if (pe_.sections[i] == nullptr) {
            return 0;
        }

        // Use SizeOfRawData if VirtualSize == 0
        size_t section_size = pe_.sections[i]->Misc.VirtualSize;
        if (section_size == 0) {
            section_size = pe_.sections[i]->SizeOfRawData;
        }

        if (pe_.sections[i]->VirtualAddress <= rva) {
            if ((pe_.sections[i]->VirtualAddress + section_size) > rva) {
                rva -= pe_.sections[i]->VirtualAddress;
                rva += pe_.sections[i]->PointerToRawData;
                return rva;
            }
        }
    }

    // Handle PE with a single section
    if (pe_.num_sections == 1) {
        rva -= pe_.sections[0]->VirtualAddress;
        rva += pe_.sections[0]->PointerToRawData;
        return rva;
    }

    return rva; // PE with no sections, return RVA
}

IMAGE_DATA_DIRECTORY *pe_context::pe_directory_by_entry(ImageDirectoryEntry entry) {
    if (pe_.directories == nullptr || entry > pe_.num_directories - 1)
        return nullptr;

    return pe_.directories[entry];
}

NODE_PERES *pe_context::discoveryNodesPeres() {
    const IMAGE_DATA_DIRECTORY *resourceDirectory = pe_directory_by_entry(IMAGE_DIRECTORY_ENTRY_RESOURCE);
    if (resourceDirectory == nullptr || resourceDirectory->Size == 0)
        return nullptr;

    uint64_t resourceDirOffset = pe_rva2ofs(resourceDirectory->VirtualAddress);

    uintptr_t offset = resourceDirOffset;
    void *ptr = ptr_add<void>(fm_.data(), offset);

    if (!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY))) {
        return nullptr;
    }

    auto node = malloc_helper<NODE_PERES>();
    if (node == nullptr) {
        return nullptr;
    }
    memset(node, 0, sizeof(NODE_PERES));
    node->lastNode = nullptr; // root
    node->nodeType = RDT_RESOURCE_DIRECTORY;
    node->nodeLevel = RDT_LEVEL1;
    node->resource.resourceDirectory = (IMAGE_RESOURCE_DIRECTORY *) ptr;

    for (int i = 1, offsetDirectory1 = 0; i <= (lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY,
                                                                       RDT_LEVEL1)->resource.resourceDirectory->NumberOfNamedEntries +
                                                lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY,
                                                                       RDT_LEVEL1)->resource.resourceDirectory->NumberOfIdEntries); i++) {
        offsetDirectory1 += (i == 1) ? 16 : 8;
        offset = resourceDirOffset + offsetDirectory1;
        ptr = ptr_add<void>(fm_.data(), offset);
        if (!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY))) {
            goto _error;
        }

        node = createNode(node, RDT_DIRECTORY_ENTRY);
        NODE_PERES *rootNode = node;
        node->rootNode = rootNode;
        node->nodeLevel = RDT_LEVEL1;
        node->resource.directoryEntry = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY_ENTRY *>( ptr);

        const NODE_PERES *lastDirectoryEntryNodeAtLevel1 = lastNodeByTypeAndLevel(node, RDT_DIRECTORY_ENTRY,
                                                                                  RDT_LEVEL1);

        if (lastDirectoryEntryNodeAtLevel1->resource.directoryEntry->DirectoryData.data.DataIsDirectory) {
            offset = resourceDirOffset +
                     lastDirectoryEntryNodeAtLevel1->resource.directoryEntry->DirectoryData.data.OffsetToDirectory;
            ptr = ptr_add<void>(fm_.data(), offset);
            if (!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY))) {
                goto _error;
            }

            node = createNode(node, RDT_RESOURCE_DIRECTORY);
            node->rootNode = const_cast<NODE_PERES *>(lastDirectoryEntryNodeAtLevel1);
            node->nodeLevel = RDT_LEVEL2;
            node->resource.resourceDirectory = (IMAGE_RESOURCE_DIRECTORY *) ptr;

            for (int j = 1, offsetDirectory2 = 0; j <= (lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY,
                                                                               RDT_LEVEL2)->resource.resourceDirectory->NumberOfNamedEntries +
                                                        lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY,
                                                                               RDT_LEVEL2)->resource.resourceDirectory->NumberOfIdEntries); j++) {
                offsetDirectory2 += (j == 1) ? 16 : 8;
                offset = resourceDirOffset + lastNodeByTypeAndLevel(node, RDT_DIRECTORY_ENTRY,
                                                                    RDT_LEVEL1)->resource.directoryEntry->DirectoryData.data.OffsetToDirectory +
                         offsetDirectory2;
                ptr = ptr_add<void>(fm_.data(), offset);
                if (!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY))) {
                    goto _error;
                }

                node = createNode(node, RDT_DIRECTORY_ENTRY);
                node->rootNode = rootNode;
                node->nodeLevel = RDT_LEVEL2;
                node->resource.directoryEntry = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY_ENTRY *>( ptr);

                offset = resourceDirOffset +
                         node->resource.directoryEntry->DirectoryData.data.OffsetToDirectory; // posiciona em 0x72
                ptr = ptr_add<void>(fm_.data(), offset);
                if (!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY))) {
                    goto _error;
                }

                node = createNode(node, RDT_RESOURCE_DIRECTORY);
                node->rootNode = rootNode;
                node->nodeLevel = RDT_LEVEL3;
                node->resource.resourceDirectory = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY *>( ptr);

                offset += sizeof(IMAGE_RESOURCE_DIRECTORY);

                for (int y = 1; y <= (lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY,
                                                             RDT_LEVEL3)->resource.resourceDirectory->NumberOfNamedEntries +
                                      lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY,
                                                             RDT_LEVEL3)->resource.resourceDirectory->NumberOfIdEntries); y++) {
                    ptr = ptr_add<void>(fm_.data(), offset);
                    if (!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY))) {
                        goto _error;
                    }
                    node = createNode(node, RDT_DIRECTORY_ENTRY);
                    node->rootNode = rootNode;
                    node->nodeLevel = RDT_LEVEL3;
                    node->resource.directoryEntry = (IMAGE_RESOURCE_DIRECTORY_ENTRY *) ptr;

                    offset = resourceDirOffset + node->resource.directoryEntry->DirectoryName.name.NameOffset;
                    ptr = ptr_add<void>(fm_.data(), offset);
                    if (!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DATA_STRING))) {
                        goto _error;
                    }
                    node = createNode(node, RDT_DATA_STRING);
                    node->rootNode = rootNode;
                    node->nodeLevel = RDT_LEVEL3;
                    node->resource.dataString = reinterpret_cast<IMAGE_RESOURCE_DATA_STRING *>( ptr);

                    offset = resourceDirOffset +
                             node->lastNode->resource.directoryEntry->DirectoryData.data.OffsetToDirectory;
                    ptr = ptr_add<void>(fm_.data(), offset);
                    if (!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DATA_ENTRY))) {
                        goto _error;
                    }
                    node = createNode(node, RDT_DATA_ENTRY);
                    node->rootNode = rootNode;
                    node->nodeLevel = RDT_LEVEL3;
                    node->resource.dataEntry = reinterpret_cast<IMAGE_RESOURCE_DATA_ENTRY *>( ptr);

                    offset += sizeof(IMAGE_RESOURCE_DATA_ENTRY);
                }
            }
        }
    }

    return node;

    _error:
    if (node != nullptr)
        freeNodes(node);
    return nullptr;
}

/*pe_final_output_t pe_context::pe_get_resources() {
    pe_final_output_t sum_output;
    sum_output.resourcesDirectory = nullptr;
    sum_output.directoryEntry = nullptr;
    sum_output.dataString = nullptr;
    sum_output.dataEntry = nullptr;
    NODE_PERES *node = discoveryNodesPeres();
    if (node == nullptr) {
        //fprintf(stderr, "this file has no resources\n");
        sum_output.err = LIBPE_E_ALLOCATION_FAILURE;
        freeNodes(node);
        return sum_output;
    }

    output_node_t *output = malloc_helper<output_node_t>();
    if (output == nullptr) {
        sum_output.err = LIBPE_E_ALLOCATION_FAILURE;
        freeNodes(node);
        return sum_output;
    }
    memset(output, 0, sizeof(output_node_t));

    while (node->lastNode != nullptr) {
        node = node->lastNode;
    }

    pe_resources_count_t count = get_count(node);

    int index_resourcesDirectory = 0;
    int index_directoryEntry = 0;
    int index_dataString = 0;
    int index_dataEntry = 0;

    type_RDT_RESOURCE_DIRECTORY *resourcesDirectory = malloc_helper<type_RDT_RESOURCE_DIRECTORY>(
            count.resourcesDirectory);
    type_RDT_DIRECTORY_ENTRY *directoryEntry = malloc_helper<type_RDT_DIRECTORY_ENTRY>(count.directoryEntry);
    type_RDT_DATA_STRING *dataString = malloc_helper<type_RDT_DATA_STRING>(count.dataString);
    type_RDT_DATA_ENTRY *dataEntry = malloc_helper<type_RDT_DATA_ENTRY>(count.dataEntry);

    NODE_PERES *Todelete = node;
    while (node != nullptr) {
        output = showNode(node, output);
        if (output == nullptr)
            continue;

        if (output->kind == RDT_RESOURCE_DIRECTORY) {
            resourcesDirectory[index_resourcesDirectory] = output->node_type.resourcesDirectory;
            index_resourcesDirectory++;
        }

        if (output->kind == RDT_DIRECTORY_ENTRY) {
            directoryEntry[index_directoryEntry] = output->node_type.directoryEntry;
            index_directoryEntry++;
        }

        if (output->kind == RDT_DATA_STRING) {
            dataString[index_dataString] = output->node_type.dataString;
            index_dataString++;
        }

        if (output->kind == RDT_DATA_ENTRY) {
            dataEntry[index_dataEntry] = output->node_type.dataEntry;
            index_dataEntry++;
        }
        node = node->nextNode;
    }
    sum_output.resourcesDirectory = resourcesDirectory;
    sum_output.directoryEntry = directoryEntry;
    sum_output.dataString = dataString;
    sum_output.dataEntry = dataEntry;

    free(output);
    freeNodes(Todelete);
    sum_output.err = LIBPE_E_OK;
    return sum_output;
}*/
void pe_context::extract_info(std::map<std::u16string,std::u16string>& ret) {
    NODE_PERES *node = discoveryNodesPeres();
    assert(node != nullptr);

    const NODE_PERES *dataEntryNode= nullptr;
    uint32_t nameOffset;
    bool found = false;

    while (node->lastNode != nullptr) {
        node = node->lastNode;
    }

    while (node != nullptr) {
        if (node->nodeType != RDT_DATA_ENTRY) {
            node = node->nextNode;
            continue;
        }
        dataEntryNode = lastNodeByType(node, RDT_DATA_ENTRY);
        if (dataEntryNode == nullptr)
            return;
        nameOffset = node->rootNode->resource.directoryEntry->DirectoryName.name.NameOffset;
        if (nameOffset == RT_VERSION) {
            found = true;
            break;
        }
        node = node->nextNode;
    }

    if (!found)
        return;

    const uint64_t offsetData = pe_rva2ofs(dataEntryNode->resource.dataEntry->offsetToData);
    const size_t dataEntrySize = dataEntryNode->resource.dataEntry->size;
    const char *buffer = ptr_add<char>(fm_.data(), 32 + offsetData);

    if (!fm_.map_chunk_by_pointer(buffer, dataEntrySize)) {
        return;
    }

   // VS_FIXEDFILEINFO *info = reinterpret_cast<VS_FIXEDFILEINFO *>(const_cast<char *> (buffer));
    auto vih = ptr_add<version_info_header>(buffer, sizeof(VS_FIXEDFILEINFO)+8);
    //first
    static constexpr auto string_file_version_len = 28;
    auto padding = ptr_add<WORD>(vih, sizeof(version_info_header)+string_file_version_len);
    auto st_vih = ptr_add<version_info_header>(padding, calculate_padding(padding,fm_.data()));//

    //second
    static constexpr auto string_table_key_len = 16;
    auto st_padding = ptr_add<WORD>(st_vih, sizeof(version_info_header)+string_table_key_len);
    auto str_vih = ptr_add<version_info_header>(st_padding,
                                                                calculate_padding(st_padding, fm_.data()));//

    auto buffer_end = ptr_add<char>(buffer,dataEntrySize );

    std::unordered_set<std::u16string> s
    =
            {            u"CompanyName",            u"FileDescription",            u"FileVersion",            u"ProductName",            u"ProductVersion"}    ;



     while(str_vih->wLength> 0 && ptr_add<char>(str_vih, str_vih->wLength)<=buffer_end) {

        auto *kstart = ptr_add<char16_t>(str_vih, sizeof(version_info_header));
         auto *vend = ptr_add<char16_t>(str_vih, str_vih->wLength);
         std::u16string k(kstart );
         if (!(s.find(k) != s.end())) {}
         else {
             auto *vstart = ptr_dec<char16_t>(vend, str_vih->wValueLength * sizeof(WORD));
             std::u16string_view v(vstart);
             ret[k] = v;
         }
         str_vih = ptr_add<version_info_header>(vend, calculate_padding(vend,fm_.data()));
     }




}