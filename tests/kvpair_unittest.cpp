//
// Created by Damian Li on 2024-09-07.
//

#include <gtest/gtest.h>
#include <sstream>
#include "KeyValue.h"

// Test for Key and Value Type Deduction
TEST(KeyValueTest, TypeDeduction) {
    KeyValue kv1(42, "Hello");
    KeyValue kv2(3.14, 'A');

    EXPECT_EQ(kv1.getKeyType(), KeyValue::KeyValueType::INT);
    EXPECT_EQ(kv1.getValueType(), KeyValue::KeyValueType::STRING);
    EXPECT_EQ(kv2.getKeyType(), KeyValue::KeyValueType::DOUBLE);
    EXPECT_EQ(kv2.getValueType(), KeyValue::KeyValueType::CHAR);
}

// Test for Key and Value Accessors
TEST(KeyValueTest, Accessors) {
    KeyValue kv(100, "TestValue");

    EXPECT_EQ(std::get<int>(kv.getKey()), 100);
    EXPECT_EQ(std::get<std::string>(kv.getValue()), "TestValue");
}

// Test for Numeric Comparison of Keys
TEST(KeyValueTest, NumericComparison) {
    KeyValue kv1(10, "A");
    KeyValue kv2(20, "B");

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

// Test for Lexicographical Comparison of Keys
TEST(KeyValueTest, LexicographicalComparison) {
    KeyValue kv1("Apple", 1);
    KeyValue kv2("Banana", 2);

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

// Test for Mixed Type Comparison Failure
TEST(KeyValueTest, MixedTypeComparison) {
    KeyValue kv1(100, "Test");    // Numeric key
    KeyValue kv2("Key", 50);      // String key

    EXPECT_TRUE(kv1 < kv2);  // Numeric is always smaller than string
}

// Test for Key and Value Printing
TEST(KeyValueTest, PrintKeyValue) {
    KeyValue kv(42, "Hello");

    testing::internal::CaptureStdout();
    kv.printKeyValue();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Key: 42"), std::string::npos);
    EXPECT_NE(output.find("Value: Hello"), std::string::npos);
}

// Test for Large Integer Keys and Values
TEST(KeyValueTest, LargeIntegers) {
    long long largeKey = 9223372036854775807LL;  // Largest 64-bit signed integer
    KeyValue kv(largeKey, largeKey);

    EXPECT_EQ(std::get<long long>(kv.getKey()), largeKey);
    EXPECT_EQ(std::get<long long>(kv.getValue()), largeKey);
}

// Test for Double Keys and Values
TEST(KeyValueTest, DoubleValues) {
    KeyValue kv(3.14159, 2.71828);

    EXPECT_EQ(std::get<double>(kv.getKey()), 3.14159);
    EXPECT_EQ(std::get<double>(kv.getValue()), 2.71828);
}

// Test for Character Keys and Values
TEST(KeyValueTest, CharValues) {
    KeyValue kv('A', 'B');

    EXPECT_EQ(std::get<char>(kv.getKey()), 'A');
    EXPECT_EQ(std::get<char>(kv.getValue()), 'B');
}

TEST(KeyValueTest, CompareIntWithLong) {
    KeyValue kv1(100, 0);  // int key
    KeyValue kv2(100LL, 0);  // long long key

    EXPECT_FALSE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
    EXPECT_TRUE(kv1 == kv2);
}

TEST(KeyValueTest, CompareIntWithDouble) {
    KeyValue kv1(100, 0);  // int key
    KeyValue kv2(100.0, 0);  // double key

    EXPECT_FALSE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
    EXPECT_TRUE(kv1 == kv2);
}

TEST(KeyValueTest, CompareIntWithString) {
    KeyValue kv1(100, 0);  // int key
    KeyValue kv2("abc", 0);  // string key

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

TEST(KeyValueTest, CompareDoubleWithString) {
    KeyValue kv1(100.5, 0);  // double key
    KeyValue kv2("xyz", 0);  // string key

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

TEST(KeyValueTest, CompareCharWithString) {
    KeyValue kv1('a', 0);  // char key
    KeyValue kv2("abc", 0);  // string key

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

TEST(KeyValueTest, CompareCharWithDouble) {
    KeyValue kv1('a', 0);  // char key
    KeyValue kv2(99.99, 0);  // double key

    EXPECT_FALSE(kv1 > kv2);
    EXPECT_TRUE(kv2 > kv1);
}

TEST(KeyValueTest, CompareStringWithString) {
    KeyValue kv1("abc", 0);  // string key
    KeyValue kv2("xyz", 0);  // string key

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

TEST(KeyValueTest, CompareDefaultKeyValues) {
    KeyValue kv1;  // Default KeyValue (int key initialized to 0)
    KeyValue kv2(0, 0);  // Explicitly initialized to 0

    EXPECT_TRUE(kv1 == kv2);  // Default and explicit 0 should be equal
}

// Test the printKeyValue method
TEST(KeyValueTest, Print_KeyValue) {
    // Create a KeyValue instance
    KeyValue kv(100, "testValue");

    // Redirect the output to a stringstream
    std::ostringstream output;
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();  // Save old buffer
    std::cout.rdbuf(output.rdbuf());  // Redirect std::cout to stringstream

    // Call printKeyValue() which should print the key-value to the stream
    kv.printKeyValue();

    // Restore std::cout to its original buffer
    std::cout.rdbuf(oldCoutStreamBuf);

    // Expected output
    std::string expectedOutput =
        "Key Type: INT\n"
        "Key: 100\n"
        "Value Type: STRING\n"
        "Value: testValue\n";

    // Compare the redirected output to the expected output
    EXPECT_EQ(output.str(), expectedOutput);
}