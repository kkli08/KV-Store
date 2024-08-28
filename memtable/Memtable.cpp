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


void put(long long key, long long value) {

}

long long get(long long key) {

    return 0;
}