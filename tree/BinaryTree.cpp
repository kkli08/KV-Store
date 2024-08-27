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

// Helper function to destroy the tree
void BinaryTree::destroyTree(TreeNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

// Insert a node into the tree
void BinaryTree::insert(long long value) {
    insert(root, value);
}

void BinaryTree::insert(TreeNode*& node, long long value) {
    if (node == nullptr) {
        node = new TreeNode(value);
    } else if (value < node->value) {
        insert(node->left, value);
    } else {
        insert(node->right, value);
    }
}

// Search for a value in the tree
bool BinaryTree::search(long long value) {
    return search(root, value);
}

bool BinaryTree::search(TreeNode* node, long long value) {
    if (node == nullptr) {
        return false;
    } else if (node->value == value) {
        return true;
    } else if (value < node->value) {
        return search(node->left, value);
    } else {
        return search(node->right, value);
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
        std::cout << node->value << " ";
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
        std::cout << node->value << " ";
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
        std::cout << node->value << " ";
    }
}

