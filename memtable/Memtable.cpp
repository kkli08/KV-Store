//
// Created by Damian Li on 2024-08-26.
//

#include "Memtable.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <string>
#include <filesystem>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

// Constructor
Memtable::Memtable(int threshold) {
    memtable_size = threshold;
    current_size = 0;
    tree = new RedBlackTree();
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
        // flush to disk and reset current_size to 0
        flushToDisk();
        current_size = 0;
    }
}

long long Memtable::get(long long key) {
    return tree->getValue(key);
}

void Memtable::set_path(fs::path _path) {
    path = _path;
}

// save data into .sst file
void Memtable::flushToDisk() {
    int_flush();
}

void Memtable::int_flush() {
    auto kv_pairs = tree->inOrderFlushToSst();
    string filename = generateSstFilename();
    fs::path filepath = path / filename;

    ofstream sst_file(filepath);
    for (const auto& pair : kv_pairs) {
        sst_file << pair.first << ", " << pair.second << "\n";
    }
    sst_file.close();
}

string Memtable::generateSstFilename() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y_%m_%d_%H%M%S");
    return ss.str() + ".sst";
}
