//
// Created by Damian Li on 2024-08-26.
//

#ifndef BINARYTREE_H
#define BINARYTREE_H

#include "TreeNode.h"
#include "KeyValue.h"
#include <set>
using namespace std;

class BinaryTree {
public:
    BinaryTree();
    virtual ~BinaryTree();

    // Templated insert method
    template<typename K, typename V>
    void insert(K key, V value);
    bool search(const KeyValue& kv);  // Search using KeyValue
    // Templated search method
    template<typename K>
    bool search(K key);

    TreeNode*& getRoot();

    // Traverse methods
    virtual void inorderTraversal();
    void preorderTraversal();
    void postorderTraversal();

    // void Scan(TreeNode* node, long long small_key, long long large_key, unordered_map<long long, KeyValue>& res);
    void Scan(TreeNode* node, const KeyValue& small_key, const KeyValue& large_key, std::set<KeyValue>& res);

protected:
    TreeNode *root;

    // Internal insert method using KeyValue
    void insert(TreeNode*& node, KeyValue kv);

    bool search(TreeNode* node, const KeyValue& kv);  // Internal search

    // Traverse methods
    void inorderTraversal(TreeNode* node);
    void preorderTraversal(TreeNode* node);
    void postorderTraversal(TreeNode* node);

    static void destroyTree(const TreeNode* node);
};

#include "BinaryTree.tpp"  // Include the templated method implementations

#endif // BINARYTREE_H

