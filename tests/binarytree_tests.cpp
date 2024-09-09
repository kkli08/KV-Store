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

TEST(BinaryTreeTest, InsertAndSearchStringKey) {
    BinaryTree* tree = new BinaryTree();
    tree->insert(std::string("apple"), 100);
    tree->insert(std::string("banana"), 200);
    tree->insert(std::string("cherry"), 300);

    EXPECT_TRUE(tree->search(std::string("apple")));
    EXPECT_TRUE(tree->search(std::string("banana")));
    EXPECT_TRUE(tree->search(std::string("cherry")));
    EXPECT_FALSE(tree->search(std::string("grape")));

    delete tree;  // Clean up memory
}

TEST(BinaryTreeTest, InsertAndSearchDoubleKey) {
    BinaryTree* tree = new BinaryTree();
    tree->insert(1.1, 10);
    tree->insert(2.2, 20);
    tree->insert(3.3, 30);

    EXPECT_TRUE(tree->search(1.1));
    EXPECT_TRUE(tree->search(2.2));
    EXPECT_TRUE(tree->search(3.3));
    EXPECT_FALSE(tree->search(4.4));

    delete tree;  // Clean up memory
}

TEST(BinaryTreeTest, InsertAndSearchMixedKeys) {
    BinaryTree* tree = new BinaryTree();
    tree->insert(100, 1000);
    tree->insert(3.14, 314);
    tree->insert(std::string("key123"), 123);

    EXPECT_TRUE(tree->search(100));
    EXPECT_TRUE(tree->search(3.14));
    EXPECT_TRUE(tree->search(std::string("key123")));
    EXPECT_FALSE(tree->search(200));
    EXPECT_FALSE(tree->search(6.28));
    EXPECT_FALSE(tree->search(std::string("non_existent_key")));

    delete tree;  // Clean up memory
}

// Test the inorderTraversal method
TEST(BinaryTreeTest, InorderTraversal) {
    // Create a BinaryTree instance
    BinaryTree* tree = new BinaryTree();

    // Insert some key-value pairs
    tree->insert(10, "value10");
    tree->insert(5, "value5");
    tree->insert(15, "value15");
    tree->insert(3, "value3");
    tree->insert(7, "value7");

    // Redirect the output to a stringstream
    std::ostringstream output;
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();  // Save old buffer
    std::cout.rdbuf(output.rdbuf());  // Redirect std::cout to stringstream

    // Call inorderTraversal() which should print the key-value pairs in order
    tree->inorderTraversal();

    // Restore std::cout to its original buffer
    std::cout.rdbuf(oldCoutStreamBuf);

    // Expected inorder traversal output
    std::string expectedOutput =
        "Key Type: INT\n"
        "Key: 3\n"
        "Value Type: STRING\n"
        "Value: value3\n"
        "Key Type: INT\n"
        "Key: 5\n"
        "Value Type: STRING\n"
        "Value: value5\n"
        "Key Type: INT\n"
        "Key: 7\n"
        "Value Type: STRING\n"
        "Value: value7\n"
        "Key Type: INT\n"
        "Key: 10\n"
        "Value Type: STRING\n"
        "Value: value10\n"
        "Key Type: INT\n"
        "Key: 15\n"
        "Value Type: STRING\n"
        "Value: value15\n"
        "\n";  // There is an extra newline printed at the end of inorderTraversal

    // Compare the redirected output to the expected output
    EXPECT_EQ(output.str(), expectedOutput);

    delete tree;  // Clean up memory
}

TEST(BinaryTreeTest, BasicScanBetweenKeys) {
    BinaryTree* tree = new BinaryTree();

    // Insert some key-value pairs
    tree->insert(10, "value10");
    tree->insert(5, "value5");
    tree->insert(15, "value15");
    tree->insert(3, "value3");
    tree->insert(7, "value7");

    // Set up the result set to store the output of the scan
    std::set<KeyValue> result;

    // Scan between keys 3 and 15 (inclusive)
    tree->Scan(tree->getRoot(), KeyValue(3, ""), KeyValue(15, ""), result);

    // Verify the output
    EXPECT_EQ(result.size(), 5);  // There should be 5 nodes in range
    auto it = result.begin();
    EXPECT_EQ(std::get<int>(it->getKey()), 3); it++;
    EXPECT_EQ(std::get<int>(it->getKey()), 5); it++;
    EXPECT_EQ(std::get<int>(it->getKey()), 7); it++;
    EXPECT_EQ(std::get<int>(it->getKey()), 10); it++;
    EXPECT_EQ(std::get<int>(it->getKey()), 15);

    delete tree;
}

TEST(BinaryTreeTest, ScanWithOneKeyInRange) {
    BinaryTree* tree = new BinaryTree();

    // Insert some key-value pairs
    tree->insert(10, "value10");
    tree->insert(5, "value5");
    tree->insert(15, "value15");

    // Set up the result set to store the output of the scan
    std::set<KeyValue> result;

    // Scan between keys 8 and 12 (only key 10 should be in the range)
    tree->Scan(tree->getRoot(), KeyValue(8, ""), KeyValue(12, ""), result);

    // Verify the output
    EXPECT_EQ(result.size(), 1);  // Only one node should be in range
    auto it = result.begin();
    EXPECT_EQ(std::get<int>(it->getKey()), 10);

    delete tree;
}

TEST(BinaryTreeTest, ScanWithNoKeysInRange) {
    BinaryTree* tree = new BinaryTree();

    // Insert some key-value pairs
    tree->insert(10, "value10");
    tree->insert(5, "value5");
    tree->insert(15, "value15");

    // Set up the result set to store the output of the scan
    std::set<KeyValue> result;

    // Scan between keys 20 and 30 (no nodes should be in the range)
    tree->Scan(tree->getRoot(), KeyValue(20, ""), KeyValue(30, ""), result);

    // Verify the output
    EXPECT_TRUE(result.empty());  // The result set should be empty

    delete tree;
}

// Scan with Mixed Key Types (int, double, and string)
TEST(BinaryTreeTest, ScanWithMixedKeyTypes) {
    BinaryTree* tree = new BinaryTree();

    // Insert different types of key-value pairs
    tree->insert(10, "value10");            // int
    tree->insert(5.5, "valueDouble5");      // double
    tree->insert("keyString", "valueStr");  // string
    tree->insert(7, "value7");              // int
    tree->insert(3, "value3");              // int

    // Set up the result set to store the output of the scan
    std::set<KeyValue> result;

    // Scan between int key 3 and string "keyString"
    tree->Scan(tree->getRoot(), KeyValue(3, ""), KeyValue("keyString", ""), result);

    // Verify the output
    EXPECT_EQ(result.size(), 5);  // All nodes should be in range (3, 7, 5.5, 10, "keyString")
    auto it = result.begin();

    // Check for the key types and their expected values
    if (std::holds_alternative<int>(it->getKey())) EXPECT_EQ(std::get<int>(it->getKey()), 3); it++;
    if (std::holds_alternative<int>(it->getKey())) EXPECT_EQ(std::get<int>(it->getKey()), 7); it++;
    if (std::holds_alternative<double>(it->getKey())) EXPECT_EQ(std::get<double>(it->getKey()), 5.5); it++;
    if (std::holds_alternative<int>(it->getKey())) EXPECT_EQ(std::get<int>(it->getKey()), 10); it++;
    if (std::holds_alternative<std::string>(it->getKey())) EXPECT_EQ(std::get<std::string>(it->getKey()), "keyString");

    delete tree;
}

// Scan with Numeric and String Keys
TEST(BinaryTreeTest, ScanWithNumericAndStringKeys) {
    BinaryTree* tree = new BinaryTree();

    // Insert a mix of numeric and string keys
    tree->insert(1, "value1");
    tree->insert(2.2, "valueDouble2");
    tree->insert("apple", "valueApple");
    tree->insert(3, "value3");
    tree->insert("banana", "valueBanana");

    // Set up the result set to store the output of the scan
    std::set<KeyValue> result;

    // Scan between numeric key 2 and string "banana"
    tree->Scan(tree->getRoot(), KeyValue(2, ""), KeyValue("banana", ""), result);

    // Verify the output
    EXPECT_EQ(result.size(), 4);  // Should include 2.2, 3, "apple", "banana"
    auto it = result.begin();

    if (std::holds_alternative<double>(it->getKey())) EXPECT_EQ(std::get<double>(it->getKey()), 2.2); it++;
    if (std::holds_alternative<int>(it->getKey())) EXPECT_EQ(std::get<int>(it->getKey()), 3); it++;
    if (std::holds_alternative<std::string>(it->getKey())) EXPECT_EQ(std::get<std::string>(it->getKey()), "apple"); it++;
    if (std::holds_alternative<std::string>(it->getKey())) EXPECT_EQ(std::get<std::string>(it->getKey()), "banana");

    delete tree;
}

TEST(BinaryTreeTest, ScanWithLongAndCharKeys) {
    BinaryTree* tree = new BinaryTree();

    // Insert different types of key-value pairs
    tree->insert(100LL, "valueLong100");   // long long
    tree->insert(50LL, "valueLong50");     // long long
    tree->insert('a', "valueCharA");       // char
    tree->insert('z', "valueCharZ");       // char
    tree->insert(75LL, "valueLong75");     // long long

    // Set up the result set to store the output of the scan
    std::set<KeyValue> result;

    // Scan between long key 50 and char 'z'
    tree->Scan(tree->getRoot(), KeyValue(50LL, ""), KeyValue('z', ""), result);

    // Verify the output
    EXPECT_EQ(result.size(), 5);  // Should include 50LL, 75LL, 100LL, 'a'
    auto it = result.begin();

    if (std::holds_alternative<long long>(it->getKey())) EXPECT_EQ(std::get<long long>(it->getKey()), 50LL); it++;
    if (std::holds_alternative<long long>(it->getKey())) EXPECT_EQ(std::get<long long>(it->getKey()), 75LL); it++;
    if (std::holds_alternative<long long>(it->getKey())) EXPECT_EQ(std::get<long long>(it->getKey()), 100LL); it++;
    if (std::holds_alternative<char>(it->getKey())) EXPECT_EQ(std::get<char>(it->getKey()), 'a');

    delete tree;
}
