//
// Created by Damian Li on 2024-08-26.
//

#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H
#include "TreeNode.h"
#include "BinaryTree.h"


class RedBlackTree : public BinaryTree {
    public:
        void insert(long long key, long long value) override;

    private:
        void insert(TreeNode*& node, long long key, long long value) override;
        void balanceInsert(TreeNode*& node);
};

#endif //REDBLACKTREE_H
