//
// Created by Damian Li on 2024-08-27.
//
#include <gtest/gtest.h>
#include <string>
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

// RBTree visualization tool: https://www.cs.usfca.edu/~galles/visualization/RedBlack.html
TEST(RedBlackTreeTest, InsertSimpleSequence) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(10, 100);
    rbt->insert(20, 200);
    rbt->insert(30, 300);

    testing::internal::CaptureStdout();
    rbt->inorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    std::string expectedOutput =
        "Key: 10, Value: 100, Color: Red\n"
        "Key: 20, Value: 200, Color: Black\n"
        "Key: 30, Value: 300, Color: Red\n";

    EXPECT_EQ(output, expectedOutput);
    delete rbt;
}

TEST(RedBlackTreeTest, InsertReverseSequence) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(30, 300);
    rbt->insert(20, 200);
    rbt->insert(10, 100);

    testing::internal::CaptureStdout();
    rbt->inorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    std::string expectedOutput =
        "Key: 10, Value: 100, Color: Red\n"
        "Key: 20, Value: 200, Color: Black\n"
        "Key: 30, Value: 300, Color: Red\n";

    EXPECT_EQ(output, expectedOutput);
    delete rbt;
}

TEST(RedBlackTreeTest, InsertMixedSequence) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(20, 200);
    rbt->insert(10, 100);
    rbt->insert(30, 300);
    rbt->insert(15, 150);

    testing::internal::CaptureStdout();
    rbt->inorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    std::string expectedOutput =
        "Key: 10, Value: 100, Color: Black\n"
        "Key: 15, Value: 150, Color: Red\n"
        "Key: 20, Value: 200, Color: Black\n"
        "Key: 30, Value: 300, Color: Black\n";

    EXPECT_EQ(output, expectedOutput);
    delete rbt;
}

TEST(RedBlackTreeTest, InsertLargerSet) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(40, 400);
    rbt->insert(20, 200);
    rbt->insert(10, 100);
    rbt->insert(30, 300);
    rbt->insert(50, 500);
    rbt->insert(60, 600);

    testing::internal::CaptureStdout();
    rbt->inorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    std::string expectedOutput =
        "Key: 10, Value: 100, Color: Black\n"
        "Key: 20, Value: 200, Color: Black\n"
        "Key: 30, Value: 300, Color: Black\n"
        "Key: 40, Value: 400, Color: Red\n"
        "Key: 50, Value: 500, Color: Black\n"
        "Key: 60, Value: 600, Color: Red\n";

    EXPECT_EQ(output, expectedOutput);
    delete rbt;
}


TEST(RedBlackTreeTest, UpdateExistedValue) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(40, 400);
    rbt->insert(20, 200);
    rbt->insert(10, 100);
    rbt->insert(30, 300);
    rbt->insert(50, 500);
    rbt->insert(60, 600);
    rbt->insert(10, 900);

    testing::internal::CaptureStdout();
    rbt->inorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    std::string expectedOutput =
        "Key: 10, Value: 900, Color: Black\n"
        "Key: 20, Value: 200, Color: Black\n"
        "Key: 30, Value: 300, Color: Black\n"
        "Key: 40, Value: 400, Color: Red\n"
        "Key: 50, Value: 500, Color: Black\n"
        "Key: 60, Value: 600, Color: Red\n";

    EXPECT_EQ(output, expectedOutput);
    delete rbt;
}