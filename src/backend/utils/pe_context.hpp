//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														pe_context.hpp
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
#ifndef UTILS_PE_CONTEXT_HPP
#define UTILS_PE_CONTEXT_HPP

#include <string>

#include "file_mapper.hpp"
#include "libpe/pe.h"
#include "libpe/resources.h"
#include "libpe/dir_resources.h"

namespace trustwave {
    class pe_context {

        output_node_t *showNode(const NODE_PERES *node, output_node_t *output) {
            switch (node->nodeType) {
                default:
                    return nullptr;
                case RDT_RESOURCE_DIRECTORY: {
                    const IMAGE_RESOURCE_DIRECTORY *resourceDirectory = node->resource.resourceDirectory;
                    output->kind = RDT_RESOURCE_DIRECTORY;
                    output->node_type.resourcesDirectory.NodeType = node->nodeLevel;
                    output->node_type.resourcesDirectory.Characteristics = resourceDirectory->Characteristics;
                    output->node_type.resourcesDirectory.TimeDateStamp = resourceDirectory->TimeDateStamp;
                    output->node_type.resourcesDirectory.MajorVersion = resourceDirectory->MajorVersion;
                    output->node_type.resourcesDirectory.MinorVersion = resourceDirectory->MinorVersion;
                    output->node_type.resourcesDirectory.NumberOfNamedEntries = resourceDirectory->NumberOfNamedEntries;
                    output->node_type.resourcesDirectory.NumberOfIdEntries = resourceDirectory->NumberOfIdEntries;
                    break;
                }
                case RDT_DIRECTORY_ENTRY: {
                    const IMAGE_RESOURCE_DIRECTORY_ENTRY *directoryEntry = node->resource.directoryEntry;
                    output->kind = RDT_DIRECTORY_ENTRY;
                    output->node_type.directoryEntry.NodeType = node->nodeLevel;
                    output->node_type.directoryEntry.NameOffset = directoryEntry->DirectoryName.name.NameOffset;
                    output->node_type.directoryEntry.NameIsString = directoryEntry->DirectoryName.name.NameIsString;
                    output->node_type.directoryEntry.OffsetIsDirectory = directoryEntry->DirectoryData.data.OffsetToDirectory;
                    output->node_type.directoryEntry.DataIsDirectory = directoryEntry->DirectoryData.data.DataIsDirectory;

                    break;
                }
                case RDT_DATA_STRING: {
                    const IMAGE_RESOURCE_DATA_STRING *dataString = node->resource.dataString;
                    output->kind = RDT_DATA_STRING;
                    output->node_type.dataString.NodeType = node->nodeLevel;
                    output->node_type.dataString.Strlen = dataString->length;
                    output->node_type.dataString.String = dataString->string[0];
                    break;
                }
                case RDT_DATA_ENTRY: {
                    const IMAGE_RESOURCE_DATA_ENTRY *dataEntry = node->resource.dataEntry;
                    output->kind = RDT_DATA_ENTRY;
                    output->node_type.dataEntry.NodeType = node->nodeLevel;
                    output->node_type.dataEntry.OffsetToData = dataEntry->offsetToData;
                    output->node_type.dataEntry.Size = dataEntry->size;
                    output->node_type.dataEntry.CodePage = dataEntry->codePage;
                    output->node_type.dataEntry.CodePage = dataEntry->codePage;
                    output->node_type.dataEntry.Reserved = dataEntry->reserved;
                    break;
                }
            }

            return output;
        }

        count_output_node_t countNode(NODE_PERES *node) {
            count_output_node_t count;
            memset(&count, 0, sizeof(count_output_node_t));

            switch (node->nodeType) {
                default:
                    return count;
                case RDT_RESOURCE_DIRECTORY:
                    count.kind = RDT_RESOURCE_DIRECTORY;
                    break;
                case RDT_DIRECTORY_ENTRY:
                    count.kind = RDT_DIRECTORY_ENTRY;
                    break;
                case RDT_DATA_STRING:
                    count.kind = RDT_DATA_STRING;
                    break;
                case RDT_DATA_ENTRY:
                    count.kind = RDT_DATA_ENTRY;
                    break;
            }

            return count;
        }

        pe_resources_count_t get_count(NODE_PERES *node) {
            pe_resources_count_t count;
            int resourcesDirectory = 0;
            int directoryEntry = 0;
            int dataString = 0;
            int dataEntry = 0;

            count_output_node_t output;
            while (node->lastNode != nullptr) {
                node = node->lastNode;
            }

            while (node != nullptr) {
                output = countNode(node);
                if (output.kind == RDT_RESOURCE_DIRECTORY)
                    resourcesDirectory++;
                if (output.kind == RDT_DIRECTORY_ENTRY)
                    directoryEntry++;
                if (output.kind == RDT_DATA_STRING)
                    dataString++;
                if (output.kind == RDT_DATA_ENTRY)
                    dataEntry++;
                node = node->nextNode;
            }
            count.resourcesDirectory = resourcesDirectory;
            count.directoryEntry = directoryEntry;
            count.dataString = dataString;
            count.dataEntry = dataEntry;
            return count;
        }

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
                // TODO(jweyrich): Report allocation failure?
                return nullptr;
            }

            memset(newNode, 0, sizeof(NODE_PERES));
            newNode->lastNode = currentNode;
            newNode->nextNode = nullptr;
            newNode->nodeType = typeOfNextNode;
            currentNode->nextNode = newNode;
            return newNode;
        }

    public:
        explicit pe_context(file_reader_interface& );

        ~pe_context() {
            free(pe_.directories);
            free(pe_.sections);
        }
        int parse();
        uint64_t pe_rva2ofs(uint64_t rva);
        IMAGE_DATA_DIRECTORY *pe_directory_by_entry(ImageDirectoryEntry entry);
        NODE_PERES *discoveryNodesPeres();
        //pe_final_output_t pe_get_resources();
        void extract_info(std::map<std::u16string,std::u16string>& );

    private:

        file_mapper fm_;
        pe_file_t pe_;
    };
}
#endif //UTILS_PE_CONTEXT_HPP