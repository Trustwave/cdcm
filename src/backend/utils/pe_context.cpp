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
#include <locale>
#include <codecvt>
#include <boost/algorithm/string.hpp>
#include <iostream>
using trustwave::pe_context;
namespace {
    template<typename T> T* malloc_helper(const size_t count = 1)
    {
        return reinterpret_cast<T*>(malloc(count * sizeof(T)));
    }

    template<typename T, typename P> T* ptr_add(P* p, size_t o) { return reinterpret_cast<T*>(((char*)(p) + (o))); }
    template<typename T, typename P> T* ptr_dec(P* p, size_t o) { return reinterpret_cast<T*>(((char*)(p) - (o))); }
    template<typename T> bool allocate_elements(uint32_t e_count, T**& dest_arr, void*& dest_ptr)
    {
        if(e_count > 0) {
            dest_arr = malloc_helper<T*>(e_count);
            if(dest_arr == nullptr) {
                return false;
            }
            for(uint32_t i = 0; i < e_count; i++) {
                dest_arr[i] = ptr_add<T>(dest_ptr, i * sizeof(T));
            }
        }
        else {
            dest_ptr = nullptr;
        }
        return true;
    }
    template<typename T> void optional_header_parse(pe_file_t& pe, T*& p)
    {
        p = reinterpret_cast<T*>(pe.optional_hdr_ptr);
        pe.optional_hdr.length = sizeof(T);
        pe.num_directories = p->NumberOfRvaAndSizes;
        pe.entrypoint = p->AddressOfEntryPoint;
        pe.imagebase = p->ImageBase;
    }
    template<typename P> auto calculate_padding(P* p, const char* data)
    {
        return (4 - ((reinterpret_cast<char*>(p) - data) % 4)) % 4;
    }

    using WORD = uint16_t;

#pragma pack(push, 1)

    typedef struct {
        WORD wLength;
        WORD wValueLength; // in words
        WORD wType;

    } version_info_header;
#pragma pack(pop)

    void freeNodes(NODE_PERES* currentNode)
    {
        if(currentNode == nullptr) return;

        while(currentNode->nextNode != nullptr) {
            currentNode = currentNode->nextNode;
        }

        while(currentNode != nullptr) {
            if(currentNode->lastNode == nullptr) {
                free(currentNode);
                break;
            }
            else {
                currentNode = currentNode->lastNode;
                if(currentNode->nextNode != nullptr) free(currentNode->nextNode);
            }
        }
    }

    const NODE_PERES* lastNodeByTypeAndLevel(const NODE_PERES* currentNode, NODE_TYPE_PERES nodeTypeSearch,
                                             NODE_LEVEL_PERES nodeLevelSearch)
    {
        assert(currentNode != nullptr);

        if(currentNode->nodeType == nodeTypeSearch && currentNode->nodeLevel == nodeLevelSearch) {
            return currentNode;
        }

        while(currentNode != nullptr) {
            currentNode = currentNode->lastNode;
            if(currentNode != nullptr && currentNode->nodeType == nodeTypeSearch
               && currentNode->nodeLevel == nodeLevelSearch) {
                return currentNode;
            }
        }

        return nullptr;
    }

    const NODE_PERES* lastNodeByType(const NODE_PERES* currentNode, NODE_TYPE_PERES nodeTypeSearch)
    {
        assert(currentNode != nullptr);
        if(currentNode->nodeType == nodeTypeSearch) {
            return currentNode;
        }

        while(currentNode != nullptr) {
            currentNode = currentNode->lastNode;
            if(currentNode != nullptr && currentNode->nodeType == nodeTypeSearch) {
                return currentNode;
            }
        }

        return nullptr;
    }

    NODE_PERES* createNode(NODE_PERES* currentNode, NODE_TYPE_PERES typeOfNextNode)
    {
        assert(currentNode != nullptr);

        auto newNode = (NODE_PERES*)malloc(sizeof(NODE_PERES));
        if(newNode == nullptr) {
            return nullptr;
        }

        memset(newNode, 0, sizeof(NODE_PERES));
        newNode->lastNode = currentNode;
        newNode->nextNode = nullptr;
        newNode->nodeType = typeOfNextNode;
        currentNode->nextNode = newNode;
        return newNode;
    }
} // namespace

int pe_context::parse()
{
    static constexpr uint16_t dos_header_size = 64;
    if(!fm_.map_chunk(0, dos_header_size)) {
        return -1;
    }
    pe_.dos_hdr = reinterpret_cast<IMAGE_DOS_HEADER*>(fm_.data());
    if(pe_.dos_hdr->e_magic != MAGIC_MZ) {
        return LIBPE_E_NOT_A_PE_FILE;
    }
    const auto signature_ptr = ptr_add<uint32_t>(pe_.dos_hdr, pe_.dos_hdr->e_lfanew);
    if(!fm_.map_chunk_by_pointer(signature_ptr, LIBPE_SIZEOF_MEMBER(pe_file_t, signature))) {
        return LIBPE_E_INVALID_LFANEW;
    }
    // NT signature (PE\0\0), or 16-bit Windows NE signature (NE\0\0).
    pe_.signature = *signature_ptr;
    switch(pe_.signature) {
        default:
            return LIBPE_E_INVALID_SIGNATURE;
        case SIGNATURE_NE:
        case SIGNATURE_PE:
            break;
    }
    pe_.coff_hdr = ptr_add<IMAGE_COFF_HEADER>((void*)signature_ptr, LIBPE_SIZEOF_MEMBER(pe_file_t, signature));

    if(!fm_.map_chunk_by_pointer(pe_.coff_hdr, sizeof(IMAGE_COFF_HEADER))) {
        return LIBPE_E_MISSING_COFF_HEADER;
    }

    pe_.num_sections = pe_.coff_hdr->NumberOfSections;

    // Optional header points right after the COFF header.
    pe_.optional_hdr_ptr = ptr_add<void>(pe_.coff_hdr, sizeof(IMAGE_COFF_HEADER));

    // Figure out whether it's a PE32 or PE32+.
    auto* opt_type_ptr = reinterpret_cast<uint16_t*>(pe_.optional_hdr_ptr);
    if(!fm_.map_chunk_by_pointer(opt_type_ptr, LIBPE_SIZEOF_MEMBER(IMAGE_OPTIONAL_HEADER, type))) {
        return LIBPE_E_MISSING_OPTIONAL_HEADER;
    }

    pe_.optional_hdr.type = *opt_type_ptr;
    switch(pe_.optional_hdr.type) {
        default:
        case MAGIC_ROM:
            return LIBPE_E_UNSUPPORTED_IMAGE;
        case MAGIC_PE32:
            if(!fm_.map_chunk_by_pointer(pe_.optional_hdr_ptr, sizeof(IMAGE_OPTIONAL_HEADER_32)))
                return LIBPE_E_MISSING_OPTIONAL_HEADER;
            optional_header_parse<IMAGE_OPTIONAL_HEADER_32>(pe_, pe_.optional_hdr._32);
            break;
        case MAGIC_PE64:
            if(!fm_.map_chunk_by_pointer(pe_.optional_hdr_ptr, sizeof(IMAGE_OPTIONAL_HEADER_64))) {
                return LIBPE_E_MISSING_OPTIONAL_HEADER;
            }
            optional_header_parse<IMAGE_OPTIONAL_HEADER_64>(pe_, pe_.optional_hdr._64);
            break;
    }

    if(pe_.num_directories > MAX_DIRECTORIES) {
        return LIBPE_E_TOO_MANY_DIRECTORIES;
    }

    if(pe_.num_sections > MAX_SECTIONS) {
        return LIBPE_E_TOO_MANY_SECTIONS;
    }

    pe_.directories_ptr = ptr_add<void>(pe_.optional_hdr_ptr, pe_.optional_hdr.length);

    if(!fm_.map_chunk_by_pointer(pe_.directories_ptr, sizeof(IMAGE_DATA_DIRECTORY) * pe_.num_directories)) {
        return LIBPE_E_ALLOCATION_FAILURE;
    }
    uint32_t sections_offset
        = LIBPE_SIZEOF_MEMBER(pe_file_t, signature) + sizeof(IMAGE_FILE_HEADER) + pe_.coff_hdr->SizeOfOptionalHeader;
    pe_.sections_ptr = ptr_add<void>(signature_ptr, sections_offset);
    if(!fm_.map_chunk_by_pointer(pe_.sections_ptr, sizeof(IMAGE_SECTION_HEADER) * pe_.num_sections)) {
        return LIBPE_E_ALLOCATION_FAILURE;
    }

    if(!allocate_elements<IMAGE_DATA_DIRECTORY>(pe_.num_directories, pe_.directories, pe_.directories_ptr)) {
        return LIBPE_E_ALLOCATION_FAILURE;
    }
    if(!allocate_elements<IMAGE_SECTION_HEADER>(pe_.num_sections, pe_.sections, pe_.sections_ptr)) {
        return LIBPE_E_ALLOCATION_FAILURE;
    }

    return 0;
}

uint64_t pe_context::pe_rva2ofs(uint64_t rva)
{
    // Converts a RVA (Relative Virtual Address) to a raw file offset
    if(rva == 0 || pe_.sections == nullptr) {
        return 0;
    }

    // Find out which section the given RVA belongs
    for(uint32_t i = 0; i < pe_.num_sections; i++) {
        if(pe_.sections[i] == nullptr) {
            return 0;
        }

        // Use SizeOfRawData if VirtualSize == 0
        size_t section_size = pe_.sections[i]->Misc.VirtualSize;
        if(section_size == 0) {
            section_size = pe_.sections[i]->SizeOfRawData;
        }

        if(pe_.sections[i]->VirtualAddress <= rva) {
            if((pe_.sections[i]->VirtualAddress + section_size) > rva) {
                rva -= pe_.sections[i]->VirtualAddress;
                rva += pe_.sections[i]->PointerToRawData;
                return rva;
            }
        }
    }

    // Handle PE with a single section
    if(pe_.num_sections == 1) {
        rva -= pe_.sections[0]->VirtualAddress;
        rva += pe_.sections[0]->PointerToRawData;
        return rva;
    }

    return rva; // PE with no sections, return RVA
}

IMAGE_DATA_DIRECTORY* pe_context::pe_directory_by_entry(ImageDirectoryEntry entry)
{
    if(pe_.directories == nullptr || entry > pe_.num_directories - 1) {
        return nullptr;
    }
    return pe_.directories[entry];
}

NODE_PERES* pe_context::discoveryNodesPeres()
{
    const IMAGE_DATA_DIRECTORY* resourceDirectory = pe_directory_by_entry(IMAGE_DIRECTORY_ENTRY_RESOURCE);
    if(resourceDirectory == nullptr || resourceDirectory->Size == 0) {
        return nullptr;
    }

    uint64_t resourceDirOffset = pe_rva2ofs(resourceDirectory->VirtualAddress);

    uintptr_t offset = resourceDirOffset;
    void* ptr = ptr_add<void>(fm_.data(), offset);

    if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY))) {
        return nullptr;
    }

    auto node = malloc_helper<NODE_PERES>();
    if(node == nullptr) {
        return nullptr;
    }
    memset(node, 0, sizeof(NODE_PERES));
    node->lastNode = nullptr; // root
    node->nodeType = RDT_RESOURCE_DIRECTORY;
    node->nodeLevel = RDT_LEVEL1;
    node->resource.resourceDirectory = (IMAGE_RESOURCE_DIRECTORY*)ptr;

    for(int i = 1, offsetDirectory1 = 0; i <= (lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY, RDT_LEVEL1)
                                                   ->resource.resourceDirectory->NumberOfNamedEntries
                                               + lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY, RDT_LEVEL1)
                                                     ->resource.resourceDirectory->NumberOfIdEntries);
        i++) {
        static constexpr auto first_directory_offset = 16;
        static constexpr auto not_first_directory_offset = 8;
        offsetDirectory1 += (i == 1) ?   first_directory_offset:not_first_directory_offset;
        offset = resourceDirOffset + offsetDirectory1;
        ptr = ptr_add<void>(fm_.data(), offset);
        if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY))) {
            goto _error;
        }

        node = createNode(node, RDT_DIRECTORY_ENTRY);
        NODE_PERES* rootNode = node;
        node->rootNode = rootNode;
        node->nodeLevel = RDT_LEVEL1;
        node->resource.directoryEntry = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY_ENTRY*>(ptr);
        if (node->resource.directoryEntry->DirectoryName.name.NameIsString == 1){
            offset = resourceDirOffset + node->resource.directoryEntry->DirectoryName.name.NameOffset;
            ptr = ptr_add<void>(fm_.data(), offset);
            if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DATA_STRING))) {
                goto _error;
            }
            node = createNode(node, RDT_DATA_STRING);
            node->rootNode = rootNode;
            node->nodeLevel = RDT_LEVEL1;
            node->resource.dataString = reinterpret_cast<IMAGE_RESOURCE_DATA_STRING*>(ptr);
        }

        const NODE_PERES* lastDirectoryEntryNodeAtLevel1
            = lastNodeByTypeAndLevel(node, RDT_DIRECTORY_ENTRY, RDT_LEVEL1);

        if(lastDirectoryEntryNodeAtLevel1->resource.directoryEntry->DirectoryData.data.DataIsDirectory) {
            offset = resourceDirOffset
                     + lastDirectoryEntryNodeAtLevel1->resource.directoryEntry->DirectoryData.data.OffsetToDirectory;
            ptr = ptr_add<void>(fm_.data(), offset);
            if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY))) {
                goto _error;
            }

            node = createNode(node, RDT_RESOURCE_DIRECTORY);
            node->rootNode = const_cast<NODE_PERES*>(lastDirectoryEntryNodeAtLevel1);
            node->nodeLevel = RDT_LEVEL2;
            node->resource.resourceDirectory = (IMAGE_RESOURCE_DIRECTORY*)ptr;

            for(int j = 1, offsetDirectory2 = 0;
                j <= (lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY, RDT_LEVEL2)
                          ->resource.resourceDirectory->NumberOfNamedEntries
                      + lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY, RDT_LEVEL2)
                            ->resource.resourceDirectory->NumberOfIdEntries);
                j++) {
                offsetDirectory2 += (j == 1) ?   first_directory_offset:not_first_directory_offset;
                offset = resourceDirOffset
                         + lastNodeByTypeAndLevel(node, RDT_DIRECTORY_ENTRY, RDT_LEVEL1)
                               ->resource.directoryEntry->DirectoryData.data.OffsetToDirectory
                         + offsetDirectory2;
                ptr = ptr_add<void>(fm_.data(), offset);
                if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY))) {
                    goto _error;
                }

                node = createNode(node, RDT_DIRECTORY_ENTRY);
                node->rootNode = rootNode;
                node->nodeLevel = RDT_LEVEL2;
                node->resource.directoryEntry = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY_ENTRY*>(ptr);
                if (node->resource.directoryEntry->DirectoryName.name.NameIsString == 1){
                    offset = resourceDirOffset + node->resource.directoryEntry->DirectoryName.name.NameOffset;
                    ptr = ptr_add<void>(fm_.data(), offset);
                    if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DATA_STRING))) {
                        goto _error;
                    }
                    node = createNode(node, RDT_DATA_STRING);
                    node->rootNode = rootNode;
                    node->nodeLevel = RDT_LEVEL2;
                    node->resource.dataString = reinterpret_cast<IMAGE_RESOURCE_DATA_STRING*>(ptr);
                }
                const NODE_PERES * lastDirectoryEntryNodeAtLevel2 = lastNodeByTypeAndLevel(node, RDT_DIRECTORY_ENTRY, RDT_LEVEL2);
                offset = resourceDirOffset + lastDirectoryEntryNodeAtLevel2->resource.directoryEntry->DirectoryData.data.OffsetToDirectory;
                ptr = ptr_add<void>(fm_.data(), offset);
                if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY))) {
                    goto _error;
                }

                node = createNode(node, RDT_RESOURCE_DIRECTORY);
                node->rootNode = rootNode;
                node->nodeLevel = RDT_LEVEL3;
                node->resource.resourceDirectory = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY*>(ptr);

                offset += sizeof(IMAGE_RESOURCE_DIRECTORY);

                for(int y = 1; y <= (lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY, RDT_LEVEL3)
                                         ->resource.resourceDirectory->NumberOfNamedEntries
                                     + lastNodeByTypeAndLevel(node, RDT_RESOURCE_DIRECTORY, RDT_LEVEL3)
                                           ->resource.resourceDirectory->NumberOfIdEntries);
                    y++) {
                    ptr = ptr_add<void>(fm_.data(), offset);
                    if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY))) {
                        goto _error;
                    }
                    node = createNode(node, RDT_DIRECTORY_ENTRY);
                    node->rootNode = rootNode;
                    node->nodeLevel = RDT_LEVEL3;
                    node->resource.directoryEntry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)ptr;
                    if (node->resource.directoryEntry->DirectoryName.name.NameIsString == 1) {
                        offset = resourceDirOffset + node->resource.directoryEntry->DirectoryName.name.NameOffset;
                        ptr = ptr_add<void>(fm_.data(), offset);
                        if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DATA_STRING))) { goto _error; }
                        node = createNode(node, RDT_DATA_STRING);
                        node->rootNode = rootNode;
                        node->nodeLevel = RDT_LEVEL3;
                        node->resource.dataString = reinterpret_cast<IMAGE_RESOURCE_DATA_STRING*>(ptr);
                    }
                    if(y==1)
                    {
                        const NODE_PERES * lastDirectoryEntryNodeAtLevel3 = lastNodeByTypeAndLevel(node, RDT_DIRECTORY_ENTRY, RDT_LEVEL3);
                        offset = resourceDirOffset + lastDirectoryEntryNodeAtLevel3->resource.directoryEntry->DirectoryData.data.OffsetToDirectory;
                    }

                    ptr = ptr_add<void>(fm_.data(), offset);
                    if(!fm_.map_chunk_by_pointer(ptr, sizeof(IMAGE_RESOURCE_DATA_ENTRY))) {
                        goto _error;
                    }
                    node = createNode(node, RDT_DATA_ENTRY);
                    node->rootNode = rootNode;
                    node->nodeLevel = RDT_LEVEL3;
                    node->resource.dataEntry = reinterpret_cast<IMAGE_RESOURCE_DATA_ENTRY*>(ptr);

                    offset += sizeof(IMAGE_RESOURCE_DATA_ENTRY);
                }
            }
        }
    }

    return node;

_error:
    if(node != nullptr) freeNodes(node);
    return nullptr;
}

void pe_context::extract_info(std::map<std::u16string, std::u16string>& ret,
                              const std::unordered_set<std::u16string>& s)
{
    auto node_ptr = std::unique_ptr<NODE_PERES, decltype(freeNodes)*>(discoveryNodesPeres(), freeNodes);
    auto node = node_ptr.get();
    if(!node)
    {
       return;
    }

    const NODE_PERES* dataEntryNode = nullptr;
    uint32_t nameOffset;
    bool found = false;

    while(node->lastNode != nullptr) {
        node = node->lastNode;
    }

    while(node != nullptr) {
        if(node->nodeType != RDT_DATA_ENTRY) {
            node = node->nextNode;
            continue;
        }
        dataEntryNode = lastNodeByType(node, RDT_DATA_ENTRY);
        if(dataEntryNode == nullptr) return;
             nameOffset = node->rootNode->resource.directoryEntry->DirectoryName.name.NameOffset;
            if(nameOffset == RT_VERSION) {
                found = true;
                break;
            }

        node = node->nextNode;
    }

    if(!found) {
        return;
    }
    static constexpr auto header_size = 32;
    const uint64_t offsetData = pe_rva2ofs(dataEntryNode->resource.dataEntry->offsetToData);
    const size_t dataEntrySize = dataEntryNode->resource.dataEntry->size;

    const char* buffer = ptr_add<char>(fm_.data(), offsetData);

    if(!fm_.map_chunk_by_pointer(buffer, dataEntrySize)) {
        return;
    }
    buffer = ptr_add<char>(buffer, header_size);
    VS_FIXEDFILEINFO* info = reinterpret_cast<VS_FIXEDFILEINFO*>(const_cast<char*>(buffer));
    static constexpr size_t MAX_MSG = 256;
    char version_from_fixed[MAX_MSG];

    int rc = snprintf(version_from_fixed, MAX_MSG, "%u.%u.%u.%u",
                      (unsigned int)(info->dwProductVersionMS & 0xffff0000) >> 16,
                      (unsigned int)info->dwProductVersionMS & 0x0000ffff,
                      (unsigned int)(info->dwProductVersionLS & 0xffff0000) >> 16,
                      (unsigned int)info->dwProductVersionLS & 0x0000ffff);

    auto vih = ptr_add<version_info_header>(buffer, sizeof(VS_FIXEDFILEINFO) + 8);
    static constexpr auto sfi = u"StringFileInfo";
    auto* pad = ptr_add<char16_t>(vih, sizeof(version_info_header));
    while(std::u16string(pad) != sfi) {
        vih = ptr_add<version_info_header>(vih, vih->wLength==0?2:vih->wLength);
        pad = ptr_add<char16_t>(vih, sizeof(version_info_header));
    }

    // first
    static constexpr auto string_file_version_len = 28;
    auto padding = ptr_add<WORD>(vih, sizeof(version_info_header) + string_file_version_len);
    auto st_vih = ptr_add<version_info_header>(padding, calculate_padding(padding, fm_.data())); //

    // second
    static constexpr auto string_table_key_len = 16;
    auto st_padding = ptr_add<WORD>(st_vih, sizeof(version_info_header) + string_table_key_len);
    auto str_vih = ptr_add<version_info_header>(st_padding, calculate_padding(st_padding, fm_.data())); //

    auto buffer_end = ptr_add<char>(buffer, dataEntrySize);

    while(str_vih->wLength > 0 && ptr_add<char>(str_vih, str_vih->wLength) <= buffer_end) {
        auto* kstart = ptr_add<char16_t>(str_vih, sizeof(version_info_header));
        auto* vend = ptr_add<char16_t>(str_vih, str_vih->wLength);
        std::u16string k(kstart);

        if((s.find(k) != s.end())) {
            auto* vstart = ptr_dec<char16_t>(vend, str_vih->wValueLength * sizeof(WORD));
            vstart = ptr_dec<char16_t>(vstart, calculate_padding(vstart, fm_.data()));
            std::u16string_view v(vstart);
            ret[k] = v;
        }
        str_vih = ptr_add<version_info_header>(vend, calculate_padding(vend, fm_.data()));
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
    if(rc >= 0 && rc < MAX_MSG) {
        static constexpr auto fv_str = u"FileVersion";
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        ret[fv_str] = convert.from_bytes(version_from_fixed);
    }
// restore compiler switches
#pragma GCC diagnostic pop
}
