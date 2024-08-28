//
// Created by Damian Li on 2024-08-27.
//
#include <gtest/gtest.h>
#include "RedBlackTree.h"

// Test case for inserting a single node
TEST(RedBlackTreeTest, InsertSingleNode) {
    RedBlackTree* rbt = new RedBlackTree();
    // Insert a key-value pair into the tree
    long long key = 10;
    long long value = 100;
    rbt->insert(key, value);

    // Check if the key exists in the tree
    EXPECT_TRUE(rbt->search(key));

    // Verify the value of the inserted node
    TreeNode* node = rbt->search(key) ? rbt->getRoot() : nullptr;
    EXPECT_NE(node, nullptr); // Node should not be null
    EXPECT_EQ(node->value, value); // Value should match

    // Verify that the root node is black (which should be true in a Red-Black Tree)
    EXPECT_EQ(node->color, BLACK);

    delete rbt;
}

// Test case for inserting multiple nodes
TEST(RedBlackTreeTest, InsertMultipleNodes) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(10, 100);
    rbt->insert(20, 200);
    rbt->insert(30, 300);
    rbt->insert(40, 400);

    // Check that all keys are correctly inserted
    EXPECT_TRUE(rbt->search(10));
    EXPECT_TRUE(rbt->search(20));
    EXPECT_TRUE(rbt->search(30));
    EXPECT_TRUE(rbt->search(40));
    delete rbt;

}
