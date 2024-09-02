//
// Created by Damian Li on 2024-08-26.
//

#include "Memtable.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <string>
#include <filesystem> // c++17 features
#include <sstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

// Constructor
Memtable::Memtable(int threshold) {
    memtable_size = threshold;
    current_size = 0;
    tree = new RedBlackTree();
    path = fs::path("defaultDB");
}

Memtable::Memtable() {
    memtable_size = 1e4;
    current_size = 0;
    tree = new RedBlackTree();
}
// Destructor
Memtable::~Memtable() {
    delete tree;
}

FlushSSTInfo* Memtable::put(long long key, long long value) {
    FlushSSTInfo* info = nullptr;
    if (current_size < memtable_size) {
        tree->insert(key, value);
        current_size++;
    } else if (current_size == memtable_size) {
        // search() first to see if the value need to be updated
        if(!tree->search(key)) {
            // flush to disk and reset current_size to 0
            info = flushToDisk();
            current_size = 0;
            // relocate memory for tree
            delete tree;
            tree = new RedBlackTree();
        }
        // insert pair
        tree->insert(key, value);
        current_size++;
    }
    return info;
}

long long Memtable::get(long long key) {
    return tree->getValue(key);
}

void Memtable::set_path(fs::path _path) {
    // Check if the directory exists
    if (!fs::exists(_path)) {
        // If the directory does not exist, create it
        fs::create_directories(_path);
    }
    path = _path;
}
fs::path Memtable::get_path() {
    return path;
}

// save data into .sst file
FlushSSTInfo* Memtable::flushToDisk() {
    return int_flush();
}


FlushSSTInfo* Memtable::int_flush() {
    // Check if the directory exists
    if (!fs::exists(path)) {
        // Directory does not exist, so create it
        if (fs::create_directory(path)) {
            // std::cout << "Created database directory: " << path << std::endl;
        } else {
            std::cerr << "Failed to create directory: " << path << std::endl;
        }
    }

    auto kv_pairs = tree->inOrderFlushToSst();
    string filename = generateSstFilename();
    fs::path filepath = path / filename;

    // Open the SST file in binary mode
    ofstream sst_file(filepath, ios::binary);
    if (!sst_file) {
        cerr << "Failed to open SST file for writing: " << filepath << std::endl;
        return nullptr;
    }
    // for (const auto& pair : kv_pairs) {
    //     // convert into string type with exactly 8-byte-long
    //     string line = longLongToString(pair.first)+", "+longLongToString(pair.second);
    //     sst_file << line << "\n";
    // }
    for (const auto& pair : kv_pairs) {
        // Write the key and value directly as binary data
        sst_file.write(reinterpret_cast<const char*>(&pair.first), sizeof(long long int));
        sst_file.write(",", 1);  // Write a comma separator
        sst_file.write(reinterpret_cast<const char*>(&pair.second), sizeof(long long int));
        sst_file.write("\n", 1);  // Write a newline at the end of the line
    }
    sst_file.close();

    FlushSSTInfo* info = new FlushSSTInfo{
        filename,
        kv_pairs.size() > 0 ? kv_pairs[0].first : 1,
        kv_pairs.size() > 0 ? kv_pairs[kv_pairs.size()-1].first : -1
    };
    return info;
}

std::string Memtable::generateSstFilename() {
    // Get the current SST file size count
    int sstId = getSSTFileSize();

    // Generate the current timestamp
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y_%m_%d_%H%M");

    // Combine the SST file size and the timestamp to create the filename
    std::string filename = std::to_string(sstId) + "_" + ss.str() + ".sst";

    // Increase the SST file size count for the next filename
    increaseSSTFileSize();

    return filename;
}


/*
 * helper functions:
 * string longLongToString(long long value)
 *     --> convert LL int type to 8 byte string
 * ll stringToLongLong(const std::string &str)
 *     --> convert string into LL int type
 */
std::string Memtable::longLongToString(long long value) {
    std::string result(sizeof(long long int), '\0');
    std::memcpy(&result[0], &value, sizeof(long long int));
    return result;
}

long long Memtable::stringToLongLong(const std::string &str) {
    if (str.size() != sizeof(long long int)) {
        throw std::invalid_argument("String size does not match long long int size");
    }
    long long value;
    std::memcpy(&value, str.data(), sizeof(long long int));
    return value;
}
