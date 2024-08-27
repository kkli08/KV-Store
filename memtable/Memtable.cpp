//
// Created by Damian Li on 2024-08-26.
//

#include "Memtable.h"

// Constructor
Memtable::Memtable(size_t threshold) {
    memtable_size = threshold;
}

// // Destructor
// Memtable::~Memtable() {
//     destroyTree(root);
// }
//
// // Helper function to destroy the tree
// void Memtable::destroyMemtable(RedBlackTreeNode* node) {
//     if (node != nullptr) {
//         destroyTree(node->left);
//         destroyTree(node->right);
//         delete node;
//     }
// }