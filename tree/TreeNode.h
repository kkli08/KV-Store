//
// Created by Damian Li on 2024-08-27.
//

#ifndef TREENODE_H
#define TREENODE_H

enum Color {RED, BLACK, NA};

class TreeNode {
    public:
        long long value;
        TreeNode* left;
        TreeNode* right;
        Color color;
        // Constructor
        TreeNode(long long val);
        TreeNode(long long val, Color c);
        // Destructor
        virtual ~TreeNode();

};

#endif //TREENODE_H
