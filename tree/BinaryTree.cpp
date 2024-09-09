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

TreeNode*& BinaryTree::getRoot() {
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

// Internal insert method (uses KeyValue objects)
void BinaryTree::insert(TreeNode*& node, KeyValue kv) {
    if (node == nullptr) {
        // If the node is null, create a new TreeNode with the KeyValue
        node = new TreeNode(kv);
    } else if (kv < node->keyValue) {
        // Compare KeyValue instances directly using operator<
        insert(node->left, kv);
    } else {
        insert(node->right, kv);
    }
}


// Search for a key in the tree using KeyValue
bool BinaryTree::search(const KeyValue& kv) {
    return search(root, kv);
}

bool BinaryTree::search(TreeNode* node, const KeyValue& kv) {
    if (node == nullptr) {
        return false;  // Not found
    } else if (kv < node->keyValue) {
        return search(node->left, kv);  // Search in the left subtree
    } else if (node->keyValue < kv) {
        return search(node->right, kv);  // Search in the right subtree
    } else {
        return true;  // Found
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
        node->keyValue.printKeyValue();
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
        node->keyValue.printKeyValue();
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
        node->keyValue.printKeyValue();
    }
}

// Scan between two keys
// Scan between two keys using std::map to maintain sorted order
void BinaryTree::Scan(TreeNode* node, const KeyValue& small_key, const KeyValue& large_key, std::set<KeyValue>& res) {
    if (!node) return;

    // Prune left subtree if the current node's key is greater than the small_key
    if (node->keyValue > small_key) {
        Scan(node->left, small_key, large_key, res);
    }

    // If the current node's key is within the range, add it to the result map
    if (!(node->keyValue < small_key) && !(large_key < node->keyValue)) {
        res.insert(node->keyValue);
    }

    // Prune right subtree if the current node's key is less than the large_key
    if (node->keyValue < large_key) {
        Scan(node->right, small_key, large_key, res);
    }
}



