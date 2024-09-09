//
// Created by Damian Li on 2024-08-27.
//
#include <gtest/gtest.h>
#include <string>
#include "RedBlackTree.h"

TEST(RedBlackTreeTest, InsertIntoEmptyTree) {
    RedBlackTree tree;

    // Insert a KeyValue pair into the empty tree
    KeyValue kv(10, "value10");
    tree.insert(kv);

    // Check if the key-value pair was inserted
    KeyValue result = tree.getValue(kv);
    EXPECT_TRUE(result == kv);
}


TEST(RedBlackTreeTest, InsertWithDifferentKeyTypes) {
    RedBlackTree tree;

    // Insert KeyValue pairs with different key types
    KeyValue kv1(10, "value10");
    KeyValue kv2(5.5, "valueDouble");

    tree.insert(kv1);
    tree.insert(kv2);

    // Check if both key-value pairs were inserted
    KeyValue result1 = tree.getValue(kv1);
    KeyValue result2 = tree.getValue(kv2);

    EXPECT_TRUE(result1 == kv1);
    EXPECT_TRUE(result2 == kv2);
}

TEST(RedBlackTreeTest, UpdateExistedKeyValue) {
    RedBlackTree tree;

    // Insert a KeyValue pair
    KeyValue kv1(20, "initialValue");
    tree.insert(kv1);

    // Update the existing key with a new value
    KeyValue updatedKv(20, "updatedValue");
    tree.insert(updatedKv);  // Should update the value for key 20

    // Check if the value was updated
    KeyValue result = tree.getValue(updatedKv);
    EXPECT_TRUE(result.getValue() == updatedKv.getValue());
}

TEST(RedBlackTreeTest, InsertMaintainRBTreeProperties) {
    RedBlackTree tree;

    // Insert several KeyValue pairs
    tree.insert(KeyValue(15, "value15"));
    tree.insert(KeyValue(10, "value10"));
    tree.insert(KeyValue(20, "value20"));

    // The root node should be black to maintain red-black tree properties
    EXPECT_EQ(tree.getColor(tree.getRoot()), BLACK);
}

TEST(RedBlackTreeTest, InsertMixedKeyTypesAndInorderTraversal) {
    // Use heap memory to create a new RedBlackTree
    RedBlackTree* tree = new RedBlackTree();

    // Insert different types of KeyValue pairs
    tree->insert(KeyValue(10, "value10"));              // int
    tree->insert(KeyValue(5.5, "valueDouble5.5"));      // double
    tree->insert(KeyValue("keyString", "valueString")); // string
    tree->insert(KeyValue('A', "valueCharA"));          // char

    // Capture the inorder traversal output
    std::vector<KeyValue> inorderResults = tree->inOrderFlushToSst();

    // Check the size of the result
    EXPECT_EQ(inorderResults.size(), 4);

    // Iterate over the results and safely check the type of each key
    auto it = inorderResults.begin();

    if (std::holds_alternative<int>(it->getKey())) {
        EXPECT_EQ(std::get<int>(it->getKey()), 10);
    } else if (std::holds_alternative<double>(it->getKey())) {
        EXPECT_EQ(std::get<double>(it->getKey()), 5.5);
    } else if (std::holds_alternative<std::string>(it->getKey())) {
        EXPECT_EQ(std::get<std::string>(it->getKey()), "keyString");
    } else if (std::holds_alternative<char>(it->getKey())) {
        EXPECT_EQ(std::get<char>(it->getKey()), 'A');
    }

    // Clean up memory
    delete tree;
}

TEST(RedBlackTreeTest, InsertDeleteNodeAndCheckBalancing) {
    RedBlackTree tree;

    // Insert KeyValue pairs
    tree.insert(KeyValue(20, "value20"));
    tree.insert(KeyValue(10, "value10"));
    tree.insert(KeyValue(30, "value30"));
    tree.insert(KeyValue(5, "value5"));
    tree.insert(KeyValue(15, "value15"));

    // Delete a key from the tree
    tree.deleteKey(KeyValue(10, ""));

    // Ensure the key is no longer found
    EXPECT_FALSE(tree.search(KeyValue(10, "")));

    // Ensure other keys are still in the tree
    EXPECT_TRUE(tree.search(KeyValue(5, "")));
    EXPECT_TRUE(tree.search(KeyValue(15, "")));
    EXPECT_TRUE(tree.search(KeyValue(20, "")));
    EXPECT_TRUE(tree.search(KeyValue(30, "")));

    // Ensure the tree is still balanced and maintains red-black tree properties
    EXPECT_EQ(tree.getColor(tree.getRoot()), BLACK);
    int blackHeight = tree.getBlackHeight(tree.getRoot());
    EXPECT_EQ(tree.getBlackHeight(tree.getRoot()->left)+1, blackHeight);
    EXPECT_EQ(tree.getBlackHeight(tree.getRoot()->right)+1, blackHeight);
}


TEST(RedBlackTreeTest, InsertDuplicateKeyAndUpdateValue) {
    RedBlackTree tree;

    // Insert a KeyValue pair
    KeyValue kv1(42, "initialValue");
    tree.insert(kv1);

    // Insert a KeyValue pair with the same key but a new value
    KeyValue kvUpdated(42, "updatedValue");
    tree.insert(kvUpdated);

    // Ensure the value is updated
    KeyValue result = tree.getValue(kvUpdated);
    EXPECT_EQ(std::get<std::string>(result.getValue()), "updatedValue");

    // Ensure the key is still present
    EXPECT_TRUE(tree.search(KeyValue(42, "")));
}

TEST(RedBlackTreeTest, LargeTreeAndCheckBlackHeight) {
    // Use heap memory to create a new RedBlackTree
    RedBlackTree* tree = new RedBlackTree();

    // Insert a large number of nodes
    const int largeTreeSize = 1000;
    for (int i = 1; i <= largeTreeSize; ++i) {
        tree->insert(KeyValue(i, "value" + std::to_string(i)));
    }

    // Check if the tree is balanced by comparing black heights
    int blackHeight = tree->getBlackHeight(tree->getRoot());
    EXPECT_GT(blackHeight, 0);  // Ensure there's at least one black node

    // Verify that the black height is consistent for both left and right subtrees
    int leftBlackHeight = tree->getBlackHeight(tree->getRoot()->left);
    int rightBlackHeight = tree->getBlackHeight(tree->getRoot()->right);

    EXPECT_EQ(leftBlackHeight, rightBlackHeight);

    // Cleanup memory
    delete tree;
}
