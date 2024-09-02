//
// Created by Damian Li on 2024-08-26.
//
#include <iostream>
#include "BinaryTree.h"
// Constructor
BinaryTree::BinaryTree() : root(nullptr) {}

// Destructor
BinaryTree::~BinaryTree() {
    destroyTree(root);
}

TreeNode* BinaryTree::getRoot() {
    return root;
}

// Helper function to destroy the tree
void BinaryTree::destroyTree(const TreeNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

// Insert a node into the tree
void BinaryTree::insert(long long key, long long value) {
    insert(root, key, value);
}

void BinaryTree::insert(TreeNode*& node, long long _key, long long _value) {
    if (node == nullptr) {
        node = new TreeNode(_key, _value);
    } else if (_key < node->key) {
        insert(node->left, _key, _value);
    } else {
        insert(node->right, _key, _value);
    }
}

// Search for a key in the tree
bool BinaryTree::search(long long _key) {
    return search(root, _key);
}

bool BinaryTree::search(TreeNode* node, long long _key) {
    if (node == nullptr) {
        return false;
    } else if (node->key == _key) {
        return true;
    } else if (_key < node->key) {
        return search(node->left, _key);
    } else {
        return search(node->right, _key);
    }
}

// Inorder traversal
void BinaryTree::inorderTraversal() {
    inorderTraversal(root);
    std::cout << std::endl;
}

void BinaryTree::inorderTraversal(TreeNode* node) {
    if (node != nullptr) {
        inorderTraversal(node->left);
        std::cout << node->key << " ";
        inorderTraversal(node->right);
    }
}

// Preorder traversal
void BinaryTree::preorderTraversal() {
    preorderTraversal(root);
    std::cout << std::endl;
}

void BinaryTree::preorderTraversal(TreeNode* node) {
    if (node != nullptr) {
        std::cout << node->key << " ";
        preorderTraversal(node->left);
        preorderTraversal(node->right);
    }
}

// Postorder traversal
void BinaryTree::postorderTraversal() {
    postorderTraversal(root);
    std::cout << std::endl;
}

void BinaryTree::postorderTraversal(TreeNode* node) {
    if (node != nullptr) {
        postorderTraversal(node->left);
        postorderTraversal(node->right);
        std::cout << node->key << " ";
    }
}



void BinaryTree::Scan(TreeNode* node, long long small_key, long long large_key, unordered_map<long long, long long>& res) {
    // Inorder scanning
    if (!node) return;
    // Prune left subtree if the current node's key is greater than the small_key
    if (node->key > small_key) {
        Scan(node->left, small_key, large_key, res);
    }

    // If the current node's key is within the range, add it to the result map
    if (node->key >= small_key && node->key <= large_key) {
        res[node->key] = node->value;
    }

    // Prune right subtree if the current node's key is less than the large_key
    if (node->key < large_key) {
        Scan(node->right, small_key, large_key, res);
    }
}
