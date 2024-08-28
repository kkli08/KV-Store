//
// Created by Damian Li on 2024-08-26.
//

#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H
#include "TreeNode.h"
#include "BinaryTree.h"


class RedBlackTree final : public BinaryTree {
    public:
        void insert(long long key, long long value) override;   // done
        void deleteKey(long long key);  // added
        void updateExistedKeyValue(TreeNode *&root, long long key, long long value);
        void merge(RedBlackTree);
        void inorderTraversal() override;
        void preorder();
        long long getValue(long long key);

    protected:
        TreeNode* insert(TreeNode *&, TreeNode *&); // done
        void rotateLeft(TreeNode *&); // done
        void rotateRight(TreeNode *&); // done
        void fixInsertRBTree(TreeNode *&);  // done
        void fixDeleteRBTree(TreeNode *&);  // added
        void inorderRBT(TreeNode *&); // done
        void preorderRBT(TreeNode *&);
        int getColor(TreeNode *&);  // done
        void setColor(TreeNode *&, int);    // done
        TreeNode *minValueNode(TreeNode *&); // added
        TreeNode *maxValueNode(TreeNode *&); // added
        // TreeNode* insertBST(TreeNode *&, TreeNode *&); // same as insert(TreeNode *&, TreeNode *&)
        TreeNode* deleteBST(TreeNode *&, long long);    // added
        int getBlackHeight(TreeNode *); // added
        long long getValue(TreeNode *&, long long);
};

#endif //REDBLACKTREE_H
