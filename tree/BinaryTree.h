//
// Created by Damian Li on 2024-08-26.
//

#ifndef BINARYTREE_H
#define BINARYTREE_H
#include "TreeNode.h"

class BinaryTree {
    public:
        BinaryTree();
        virtual ~BinaryTree();

        virtual void insert(long long value);
        bool search(long long value);
        void inorderTraversal();
        void preorderTraversal();
        void postorderTraversal();

    protected:
        TreeNode* root;
        virtual void insert(TreeNode*& node, long long value);
        bool search(TreeNode* node, long long value);
        void inorderTraversal(TreeNode* node);
        void preorderTraversal(TreeNode* node);
        void postorderTraversal(TreeNode* node);
        void destroyTree(TreeNode* node);
};

#endif //BINARYTREE_H
