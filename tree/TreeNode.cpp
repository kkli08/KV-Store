//
// Created by Damian Li on 2024-08-27.
//

#include "TreeNode.h"

// Constructor
TreeNode::TreeNode(long long _value) : value(_value), left(nullptr), right(nullptr), color(RED) {}
TreeNode::TreeNode(long long val, Color c) : value(val), left(nullptr), right(nullptr), color(c) {}

// Destructor
TreeNode::~TreeNode() {
  if(left) delete left;
  if(right) delete right;
}