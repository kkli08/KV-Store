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

void Memtable::put(long long key, long long value) {
    if (current_size < memtable_size) {
        tree->insert(key, value);
        current_size++;
    } else if (current_size == memtable_size) {
        // search() first to see if the value need to be updated
        if(!tree->search(key)) {
            // flush to disk and reset current_size to 0
            flushToDisk();
            current_size = 0;
            // relocate memory for tree
            delete tree;
            tree = new RedBlackTree();
        }
        // insert pair
        tree->insert(key, value);
        current_size++;
    }
}

long long Memtable::get(long long key) {
    return tree->getValue(key);
}

void Memtable::set_path(fs::path _path) {
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

    ofstream sst_file(filepath);
    for (const auto& pair : kv_pairs) {
        sst_file << pair.first << ", " << pair.second << "\n";
    }
    sst_file.close();

    FlushSSTInfo* info = new FlushSSTInfo{filename, kv_pairs.size() > 0 ? kv_pairs[0].first : 1, kv_pairs.size() > 0 ? kv_pairs[kv_pairs.size()-1].first : -1};
    return info;
}

string Memtable::generateSstFilename() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y_%m_%d_%H%M");
    return ss.str() + ".sst";
}
