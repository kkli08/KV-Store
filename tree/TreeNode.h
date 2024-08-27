//
// Created by Damian Li on 2024-08-27.
//

#ifndef TREENODE_H
#define TREENODE_H

enum Color {RED, BLACK, NA};

class TreeNode {
    public:
        long long key;
        long long value;
        TreeNode* left;
        TreeNode* right;
        Color color;
        // Constructor
        TreeNode(long long key, long long value);
        TreeNode(long long key, long long value, Color c);
        // Destructor
        virtual ~TreeNode();

};

#endif //TREENODE_H
