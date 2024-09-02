//
// Created by Damian Li on 2024-08-26.
//
#include <gtest/gtest.h>
#include "BinaryTree.h"

// Test for Insert and Search operations
TEST(BinaryTreeTest, InsertAndSearch) {
    BinaryTree *tree = new BinaryTree();
    tree->insert(5, 29);
    tree->insert(3, 23);
    tree->insert(7, 47);

    EXPECT_TRUE(tree->search(5));
    EXPECT_TRUE(tree->search(3));
    EXPECT_TRUE(tree->search(7));
    EXPECT_FALSE(tree->search(10));

    delete tree;  // Clean up memory
}

// Test for Inorder Traversal
TEST(BinaryTreeTest, InorderTraversal) {
    BinaryTree *tree = new BinaryTree();
    tree->insert(5, 29);
    tree->insert(3, 28);
    tree->insert(7, 27);

    testing::internal::CaptureStdout();
    tree->inorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "3 5 7 \n");

    delete tree;  // Clean up memory
}

// Test for Preorder Traversal
TEST(BinaryTreeTest, PreorderTraversal) {
    BinaryTree *tree = new BinaryTree();
    tree->insert(5, 29);
    tree->insert(3, 28);
    tree->insert(7, 27);

    testing::internal::CaptureStdout();
    tree->preorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "5 3 7 \n");

    delete tree;  // Clean up memory
}

// Test for Postorder Traversal
TEST(BinaryTreeTest, PostorderTraversal) {
    BinaryTree *tree = new BinaryTree();
    tree->insert(5, 29);
    tree->insert(3, 28);
    tree->insert(7, 27);

    testing::internal::CaptureStdout();
    tree->postorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "3 7 5 \n");

    delete tree;  // Clean up memory
}

TEST(BinaryTreeTest, ScanWithinRange) {
    BinaryTree* tree = new BinaryTree();
    tree->insert(10, 100);
    tree->insert(20, 200);
    tree->insert(30, 300);
    tree->insert(40, 400);
    tree->insert(50, 500);

    unordered_map<long long, long long> result;
    tree->Scan(tree->getRoot(), 20, 40, result);

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[20], 200);
    EXPECT_EQ(result[30], 300);
    EXPECT_EQ(result[40], 400);

    delete tree;
}

TEST(BinaryTreeTest, ScanEntireRange) {
    BinaryTree* tree = new BinaryTree();
    tree->insert(10, 100);
    tree->insert(20, 200);
    tree->insert(30, 300);
    tree->insert(40, 400);
    tree->insert(50, 500);

    unordered_map<long long, long long> result;
    tree->Scan(tree->getRoot(), 10, 50, result);

    EXPECT_EQ(result.size(), 5);
    EXPECT_EQ(result[10], 100);
    EXPECT_EQ(result[20], 200);
    EXPECT_EQ(result[30], 300);
    EXPECT_EQ(result[40], 400);
    EXPECT_EQ(result[50], 500);

    delete tree;
}
