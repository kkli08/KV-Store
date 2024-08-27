//
// Created by Damian Li on 2024-08-26.
//

#include "RedBlackTree.h"

// Override insert method
void RedBlackTree::insert(long long value) {
  insert(root, value);
  if (root != nullptr){
    root->color = BLACK;  // root always black
  }
}

void RedBlackTree::insert(TreeNode*& node, long long value){
  if (node == nullptr) {
    node = new TreeNode(value);
  } else if (value < node->value) {
    insert(node->left, value);
  } else {
    insert(node->right, value);
  }
}
