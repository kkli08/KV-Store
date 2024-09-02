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


TEST(RedBlackTreeTest, InOrderFlushSingleNode) {
    RedBlackTree* rbt = new RedBlackTree();
    long long key = 10;
    long long value = 100;
    rbt->insert(key, value);

    // Perform inOrderFlushToSst
    std::vector<std::pair<long long, long long>> kv_pairs = rbt->inOrderFlushToSst();

    // Verify the output
    ASSERT_EQ(kv_pairs.size(), 1);
    EXPECT_EQ(kv_pairs[0].first, key);
    EXPECT_EQ(kv_pairs[0].second, value);

    delete rbt;
}


TEST(RedBlackTreeTest, InOrderFlushMultipleNodes) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(10, 100);
    rbt->insert(20, 200);
    rbt->insert(5, 50);

    // Perform inOrderFlushToSst
    std::vector<std::pair<long long, long long>> kv_pairs = rbt->inOrderFlushToSst();

    // Verify the output
    ASSERT_EQ(kv_pairs.size(), 3);
    EXPECT_EQ(kv_pairs[0].first, 5);
    EXPECT_EQ(kv_pairs[0].second, 50);
    EXPECT_EQ(kv_pairs[1].first, 10);
    EXPECT_EQ(kv_pairs[1].second, 100);
    EXPECT_EQ(kv_pairs[2].first, 20);
    EXPECT_EQ(kv_pairs[2].second, 200);

    delete rbt;
}

TEST(RedBlackTreeTest, InOrderFlushCompleteTree) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(30, 300);
    rbt->insert(20, 200);
    rbt->insert(40, 400);
    rbt->insert(10, 100);
    rbt->insert(25, 250);

    std::vector<std::pair<long long, long long>> kv_pairs = rbt->inOrderFlushToSst();

    // Verify the output
    ASSERT_EQ(kv_pairs.size(), 5);
    EXPECT_EQ(kv_pairs[0].first, 10);
    EXPECT_EQ(kv_pairs[0].second, 100);
    EXPECT_EQ(kv_pairs[1].first, 20);
    EXPECT_EQ(kv_pairs[1].second, 200);
    EXPECT_EQ(kv_pairs[2].first, 25);
    EXPECT_EQ(kv_pairs[2].second, 250);
    EXPECT_EQ(kv_pairs[3].first, 30);
    EXPECT_EQ(kv_pairs[3].second, 300);
    EXPECT_EQ(kv_pairs[4].first, 40);
    EXPECT_EQ(kv_pairs[4].second, 400);

    delete rbt;
}

TEST(RedBlackTreeTest, InOrderFlushDuplicateKeys) {
    RedBlackTree* rbt = new RedBlackTree();
    rbt->insert(15, 150);
    rbt->insert(15, 250); // Attempt to insert duplicate key with a different value
    rbt->insert(5, 50);

    // Perform inOrderFlushToSst
    std::vector<std::pair<long long, long long>> kv_pairs = rbt->inOrderFlushToSst();

    // Verify the output, depending on your tree's handling of duplicates
    ASSERT_EQ(kv_pairs.size(), 2); // Assuming the duplicate is either ignored or updated
    EXPECT_EQ(kv_pairs[0].first, 5);
    EXPECT_EQ(kv_pairs[0].second, 50);
    EXPECT_EQ(kv_pairs[1].first, 15);
    EXPECT_EQ(kv_pairs[1].second, 250); // Or 150 depending on your implementation

    delete rbt;
}

TEST(RedBlackTreeTest, ScanWithinRange) {
    RedBlackTree* rbtree = new RedBlackTree();
    rbtree->insert(10, 100);
    rbtree->insert(20, 200);
    rbtree->insert(30, 300);
    rbtree->insert(40, 400);
    rbtree->insert(50, 500);

    unordered_map<long long, long long> result;
    rbtree->Scan(rbtree->getRoot(), 20, 40, result);

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[20], 200);
    EXPECT_EQ(result[30], 300);
    EXPECT_EQ(result[40], 400);

    delete rbtree;
}

TEST(RedBlackTreeTest, ScanEntireRange) {
    RedBlackTree* rbtree = new RedBlackTree();
    rbtree->insert(10, 100);
    rbtree->insert(20, 200);
    rbtree->insert(30, 300);
    rbtree->insert(40, 400);
    rbtree->insert(50, 500);

    unordered_map<long long, long long> result;
    rbtree->Scan(rbtree->getRoot(), 10, 50, result);

    EXPECT_EQ(result.size(), 5);
    EXPECT_EQ(result[10], 100);
    EXPECT_EQ(result[20], 200);
    EXPECT_EQ(result[30], 300);
    EXPECT_EQ(result[40], 400);
    EXPECT_EQ(result[50], 500);

    delete rbtree;
}
