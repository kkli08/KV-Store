//
// Created by Damian Li on 2024-08-27.
//

#ifndef TREENODE_H
#define TREENODE_H

enum Color {RED, BLACK, DOUBLE_BLACK, NA};

class TreeNode {
    public:
        long long key;
        long long value;
        TreeNode* left;
        TreeNode* right;
        TreeNode* parent;
        int color;
        // Constructor
        TreeNode(long long key, long long value); // insert RED node as default
        TreeNode(long long key, long long value, Color c);
        // Destructor
        ~TreeNode();

};

#endif //TREENODE_H
