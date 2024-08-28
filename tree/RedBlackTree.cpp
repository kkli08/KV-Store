//
// Created by Damian Li on 2024-08-26.
// RBTree ideas from : https://github.com/anandarao/Red-Black-Tree
#include <utility>
#include <iostream>
#include "RedBlackTree.h"

// helper functions:
int RedBlackTree::getColor(TreeNode *&node){
  if (node == nullptr) return BLACK;
  return node->color;
}
void RedBlackTree::setColor(TreeNode *&node, int color){
  if (node == nullptr) return;
  node->color = color;
}

void RedBlackTree::inorderTraversal() {
    inorderRBT(root);
}

void RedBlackTree::inorderRBT(TreeNode *&node) {
    if (node == nullptr) {
        return; // Base case: if the node is null, return
    }
    // Recursively traverse the left subtree
    inorderRBT(node->left);
    // Print the key and value of the current node
    std::cout << "Key: " << node->key << ", Value: " << node->value << ", Color: "
              << (node->color == RED ? "Red" : (node->color == BLACK ? "Black" :
                  (node->color == DOUBLE_BLACK ? "Double Black" : "N/A"))) << std::endl;
    // Recursively traverse the right subtree
    inorderRBT(node->right);
}


/*
 * Red Black Tree Insertions
 *  - rotateLeft
 *  - rotateRight
 *  - public insert
 *  - insert
 *  - fixInsertRBTree
 */
// RBTree insert method
void RedBlackTree::insert(long long _key, long long _value) {
    // search key first
    if (search(_key)) {
        // update the value
        updateExistedKeyValue(root, _key, _value);
    }else {
        TreeNode* newNode = new TreeNode(_key, _value);
        root = insert(root, newNode);
        fixInsertRBTree(newNode);
    }

}

void RedBlackTree::updateExistedKeyValue(TreeNode *&node, long long _key, long long _value) {
    if (node == nullptr) {
        // return;
    } else if (node->key == _key) {
        node->value = _value;
    } else if (_key < node->key) {
        updateExistedKeyValue(node->left, _key, _value);
    } else {
        updateExistedKeyValue(node->right, _key, _value);
    }
}

// BST insertions
TreeNode* RedBlackTree::insert(TreeNode*& root, TreeNode*& newNode){
  if (root == nullptr)
    return newNode;

  if (newNode->key < root->key) {
    root->left = insert(root->left, newNode);
    root->left->parent = root;
  } else if (newNode->key > root->key) {
    root->right = insert(root->right, newNode);
    root->right->parent = root;
  }

  return root;
}

void RedBlackTree::fixInsertRBTree(TreeNode *&ptr){
  TreeNode *parent = nullptr;
  TreeNode *grandparent = nullptr;
  while (ptr != root && getColor(ptr) == RED && getColor(ptr->parent) == RED) {
    parent = ptr->parent;
    grandparent = parent->parent;
    if (parent == grandparent->left) {
      TreeNode *uncle = grandparent->right;
      if (getColor(uncle) == RED) {
        setColor(uncle, BLACK);
        setColor(parent, BLACK);
        setColor(grandparent, RED);
        ptr = grandparent;
      } else {
        if (ptr == parent->right) {
          rotateLeft(parent);
          ptr = parent;
          parent = ptr->parent;
        }
        rotateRight(grandparent);
        std::swap(parent->color, grandparent->color);
        ptr = parent;
      }
    } else {
      TreeNode *uncle = grandparent->left;
      if (getColor(uncle) == RED) {
        setColor(uncle, BLACK);
        setColor(parent, BLACK);
        setColor(grandparent, RED);
        ptr = grandparent;
      } else {
        if (ptr == parent->left) {
          rotateRight(parent);
          ptr = parent;
          parent = ptr->parent;
        }
        rotateLeft(grandparent);
        std::swap(parent->color, grandparent->color);
        ptr = parent;
      }
    }
  }
  setColor(root, BLACK);
}

void RedBlackTree::rotateLeft(TreeNode *&ptr) {
  TreeNode *right_child = ptr->right;
  ptr->right = right_child->left;

  if (ptr->right != nullptr)
    ptr->right->parent = ptr;

  right_child->parent = ptr->parent;

  if (ptr->parent == nullptr)
    root = right_child;
  else if (ptr == ptr->parent->left)
    ptr->parent->left = right_child;
  else
    ptr->parent->right = right_child;

  right_child->left = ptr;
  ptr->parent = right_child;
}

void RedBlackTree::rotateRight(TreeNode *&ptr) {
  TreeNode *left_child = ptr->left;
  ptr->left = left_child->right;

  if (ptr->left != nullptr)
    ptr->left->parent = ptr;

  left_child->parent = ptr->parent;

  if (ptr->parent == nullptr)
    root = left_child;
  else if (ptr == ptr->parent->left)
    ptr->parent->left = left_child;
  else
    ptr->parent->right = left_child;

  left_child->right = ptr;
  ptr->parent = left_child;
}



/*
 * Delete Key
 * - deleteKey public
 * - deleteBST
 * - fixDeleteRBTree
 */
void RedBlackTree::deleteKey(long long _key) {
  TreeNode *node = deleteBST(root, _key);
  fixDeleteRBTree(node);
}

TreeNode* RedBlackTree::deleteBST(TreeNode *&root, long long _key) {
  if (root == nullptr)
    return root;

  if (_key < root->key)
    return deleteBST(root->left, _key);

  if (_key > root->key)
    return deleteBST(root->right, _key);

  if (root->left == nullptr || root->right == nullptr)
    return root;

  TreeNode *temp = minValueNode(root->right);
  root->key = temp->key;
  return deleteBST(root->right, temp->key);
}

void RedBlackTree::fixDeleteRBTree(TreeNode *&node) {
    if (node == nullptr)
        return;

    if (node == root) {
        root = nullptr;
        return;
    }

    if (getColor(node) == RED || getColor(node->left) == RED || getColor(node->right) == RED) {
        TreeNode *child = node->left != nullptr ? node->left : node->right;

        if (node == node->parent->left) {
            node->parent->left = child;
            if (child != nullptr)
                child->parent = node->parent;
            setColor(child, BLACK);
            delete (node);
        } else {
            node->parent->right = child;
            if (child != nullptr)
                child->parent = node->parent;
            setColor(child, BLACK);
            delete (node);
        }
    } else {
        TreeNode *sibling = nullptr;
        TreeNode *parent = nullptr;
        TreeNode *ptr = node;
        setColor(ptr, DOUBLE_BLACK);
        while (ptr != root && getColor(ptr) == DOUBLE_BLACK) {
            parent = ptr->parent;
            if (ptr == parent->left) {
                sibling = parent->right;
                if (getColor(sibling) == RED) {
                    setColor(sibling, BLACK);
                    setColor(parent, RED);
                    rotateLeft(parent);
                } else {
                    if (getColor(sibling->left) == BLACK && getColor(sibling->right) == BLACK) {
                        setColor(sibling, RED);
                        if(getColor(parent) == RED)
                            setColor(parent, BLACK);
                        else
                            setColor(parent, DOUBLE_BLACK);
                        ptr = parent;
                    } else {
                        if (getColor(sibling->right) == BLACK) {
                            setColor(sibling->left, BLACK);
                            setColor(sibling, RED);
                            rotateRight(sibling);
                            sibling = parent->right;
                        }
                        setColor(sibling, parent->color);
                        setColor(parent, BLACK);
                        setColor(sibling->right, BLACK);
                        rotateLeft(parent);
                        break;
                    }
                }
            } else {
                sibling = parent->left;
                if (getColor(sibling) == RED) {
                    setColor(sibling, BLACK);
                    setColor(parent, RED);
                    rotateRight(parent);
                } else {
                    if (getColor(sibling->left) == BLACK && getColor(sibling->right) == BLACK) {
                        setColor(sibling, RED);
                        if (getColor(parent) == RED)
                            setColor(parent, BLACK);
                        else
                            setColor(parent, DOUBLE_BLACK);
                        ptr = parent;
                    } else {
                        if (getColor(sibling->left) == BLACK) {
                            setColor(sibling->right, BLACK);
                            setColor(sibling, RED);
                            rotateLeft(sibling);
                            sibling = parent->left;
                        }
                        setColor(sibling, parent->color);
                        setColor(parent, BLACK);
                        setColor(sibling->left, BLACK);
                        rotateRight(parent);
                        break;
                    }
                }
            }
        }
        if (node == node->parent->left)
            node->parent->left = nullptr;
        else
            node->parent->right = nullptr;
        delete(node);
        setColor(root, BLACK);
    }
}

TreeNode *RedBlackTree::minValueNode(TreeNode *&node) {

    TreeNode *ptr = node;

    while (ptr->left != nullptr)
        ptr = ptr->left;

    return ptr;
}

TreeNode* RedBlackTree::maxValueNode(TreeNode *&node) {
    TreeNode *ptr = node;

    while (ptr->right != nullptr)
        ptr = ptr->right;

    return ptr;
}

int RedBlackTree::getBlackHeight(TreeNode *node) {
    int blackheight = 0;
    while (node != nullptr) {
        if (getColor(node) == BLACK)
            blackheight++;
        node = node->left;
    }
    return blackheight;
}