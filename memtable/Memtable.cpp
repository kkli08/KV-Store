//
// Created by Damian Li on 2024-08-26.
//

#include "Memtable.h"

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

}