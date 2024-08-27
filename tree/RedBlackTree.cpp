//
// Created by Damian Li on 2024-08-26.
//

#include "RedBlackTree.h"

// Override insert method
void RedBlackTree::insert(long long _key, long long _value) {
  insert(root, _key, _value);
  if (root != nullptr){
    root->color = BLACK;  // root always black
  }
}

void RedBlackTree::insert(TreeNode*& node, long long _key, long long _value){
  if (node == nullptr) {
    node = new TreeNode(_key, _value);
  } else if (_key < node->key) {
    insert(node->left, _key, _value);
  } else {
    insert(node->right, _key, _value);
  }
}
