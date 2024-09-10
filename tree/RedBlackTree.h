//
// Created by Damian Li on 2024-08-26.
//

#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H
#include "TreeNode.h"
#include "BinaryTree.h"
#include <fstream>
#include <utility>
#include <vector>
#include <unordered_map>
using namespace std;

class RedBlackTree final : public BinaryTree {
    public:
        void merge(RedBlackTree);
        void inorderTraversal() override;
        void preorder();
        // update with KeyValue Class
        vector<KeyValue> inOrderFlushToSst(); // tested
        KeyValue getValue(const KeyValue& kv); // tested
        void insert(KeyValue kv);   // done tested
        void updateExistedKeyValue(TreeNode *&root, KeyValue& kv); // tested
        void deleteKey(KeyValue kv);  // added
        int getColor(TreeNode *&);  // done tested
        int getBlackHeight(TreeNode *); // added tested

        void inOrderTraversal(std::function<void(const KeyValue&)> callback) const;
        void inOrderTraversal(TreeNode* node, std::function<void(const KeyValue&)> callback) const;


    protected:
        void rotateLeft(TreeNode *&); // done
        void rotateRight(TreeNode *&); // done
        void fixInsertRBTree(TreeNode *&);  // done
        void fixDeleteRBTree(TreeNode *&);  // added
        void inorderRBT(TreeNode *&); // done
        void preorderRBT(TreeNode *&);
        void setColor(TreeNode *&, int);    // done
        TreeNode *minValueNode(TreeNode *&); // added
        TreeNode *maxValueNode(TreeNode *&); // added
        // update with KeyValue Class
        void inorderTraversal(TreeNode *, vector<KeyValue> &);
        KeyValue getValue(TreeNode *&, const KeyValue& kv);
        TreeNode* insert(TreeNode *&, TreeNode *&); // done
        TreeNode* deleteBST(TreeNode *&, KeyValue kv);    // added

};

#endif //REDBLACKTREE_H
