//
// Created by Damian Li on 2024-08-26.
//
#include <gtest/gtest.h>
#include "BinaryTree.h"

TEST(BinaryTreeTest, InsertAndSearch) {
    BinaryTree tree;
    tree.insert(5, 29);
    tree.insert(3, 23);
    tree.insert(7, 47);

    EXPECT_TRUE(tree.search(5));
    EXPECT_TRUE(tree.search(3));
    EXPECT_TRUE(tree.search(7));
    EXPECT_FALSE(tree.search(10));
}

TEST(BinaryTreeTest, InorderTraversal) {
    BinaryTree tree;
    tree.insert(5, 29);
    tree.insert(3, 28);
    tree.insert(7, 27);

    testing::internal::CaptureStdout();
    tree.inorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "3 5 7 \n");
}

TEST(BinaryTreeTest, PreorderTraversal) {
    BinaryTree tree;
    tree.insert(5, 29);
    tree.insert(3, 28);
    tree.insert(7, 27);

    testing::internal::CaptureStdout();
    tree.preorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "5 3 7 \n");
}

TEST(BinaryTreeTest, PostorderTraversal) {
    BinaryTree tree;
    tree.insert(5, 29);
    tree.insert(3, 28);
    tree.insert(7, 27);

    testing::internal::CaptureStdout();
    tree.postorderTraversal();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "3 7 5 \n");
}
