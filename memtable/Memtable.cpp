//
// Created by Damian Li on 2024-08-26.
//

#include "Memtable.h"

// Constructor
Memtable::Memtable(size_t threshold) {
    memtable_size = threshold;
    current_size = 0;
    tree = new RedBlackTree();
}

// Destructor
Memtable::~Memtable() {
    delete tree;
}


void put(long long key, long long value) {

}

long long get(long long key) {

    return 0;
}