//
// Created by Damian Li on 2024-08-27.
//

#include "TreeNode.h"

// Constructor
TreeNode::TreeNode(long long _key, long long _value) : key(_key), value(_value), left(nullptr), right(nullptr), parent(nullptr), color(RED) {}
TreeNode::TreeNode(long long _key, long long _value, Color c) : key(_key), value(_value), left(nullptr), right(nullptr), parent(nullptr), color(c) {}

// Destructor
TreeNode::~TreeNode() {
  // if(left) delete left;
  // if(right) delete right;
}