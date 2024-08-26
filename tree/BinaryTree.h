//
// Created by Damian Li on 2024-08-26.
//

#ifndef BINARYTREE_H
#define BINARYTREE_H

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int val) {
      value = val;
      left = right = nullptr;
    }
};

class BinaryTree {
public:
    BinaryTree();
    ~BinaryTree();

    void insert(int value);
    bool search(int value);
    void inorderTraversal();
    void preorderTraversal();
    void postorderTraversal();

private:
    TreeNode* root;

    void insert(TreeNode*& node, int value);
    bool search(TreeNode* node, int value);
    void inorderTraversal(TreeNode* node);
    void preorderTraversal(TreeNode* node);
    void postorderTraversal(TreeNode* node);
    void destroyTree(TreeNode* node);
};

#endif //BINARYTREE_H
