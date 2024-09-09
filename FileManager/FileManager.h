//
// Created by Damian Li on 2024-09-08.
//

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "KeyValue.h"
#include <filesystem>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <RedBlackTree.h>

namespace fs = std::filesystem;

struct FlushSSTInfo {
    std::string fileName;
    KeyValue smallest_key;
    KeyValue largest_key;
};

// struct SSTInfileIndex {
//     uint32_t inFileIndex_checksum;
//     vector<uint32_t> kv_idx_checksums;
//     // Serialize KeyValue pair
//     void serialize(std::ofstream& file) const;
//     // Deserialize KeyValue pair
//     static SSTInfileIndex deserialize(std::ifstream& file);
// };

struct SSTHeader {
    uint32_t num_key_values;   // Number of KeyValue pairs in the file
    uint32_t header_checksum;  // Checksum for the header
    // Helper method to calculate checksum for the header
    uint32_t calculateChecksum() const;
    // Serialize the header to a binary file
    void serialize(std::ofstream& file) const;
    // Deserialize the header from a binary file
    static SSTHeader deserialize(std::ifstream& file);
};


struct SerializedKeyValue {
    KeyValue kv;
    uint32_t kv_checksum;
    // Helper method to calculate checksum for KeyValue pair
    uint32_t calculateChecksum() const;
    // Serialize KeyValue pair
    void serialize(std::ofstream& file) const;
    // Deserialize KeyValue pair
    static SerializedKeyValue deserialize(std::ifstream& file);
};



class FileManager {
public:
    FileManager(); // added
    explicit FileManager(fs::path directory); // added
    // Flush KeyValue pairs to disk and return metadata about the SST file
    FlushSSTInfo flushToDisk(const std::vector<KeyValue>& kv_pairs);
    // Load KeyValue pairs from disk into memory
    RedBlackTree* loadFromDisk(const std::string& sst_filename);
    // Generate name for SST
    std::string generateSstFilename(); // added
    // Set the directory for storing SST files
    void setDirectory(const fs::path& path); // added
    // Get the directory for storing SST files
    fs::path getDirectory() const; // added
    // Increase file counter
    int increaseFileCounter() {return ++sstFileCounter - 1;};

private:
    fs::path directory;
    int sstFileCounter = 0;  // To keep track of SST file names
    SSTHeader sstHeader;

};

#endif // FILEMANAGER_H


