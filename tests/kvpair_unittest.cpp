//
// Created by Damian Li on 2024-09-07.
//

#include <gtest/gtest.h>
#include <sstream>
#include "KeyValue.h"

// Test for Key and Value Type Deduction
TEST(KeyValueWrapperTest, TypeDeduction) {
    KeyValueWrapper kv1(42, "Hello");
    KeyValueWrapper kv2(3.14, 'A');

    EXPECT_EQ(kv1.getKeyType(), KeyValue::INT);
    EXPECT_EQ(kv1.getValueType(), KeyValue::STRING);
    EXPECT_EQ(kv2.getKeyType(), KeyValue::DOUBLE);
    EXPECT_EQ(kv2.getValueType(), KeyValue::CHAR);
}

// Test for Key and Value Accessors
TEST(KeyValueWrapperTest, Accessors) {
    KeyValueWrapper kv(100, "TestValue");

    EXPECT_EQ(kv.kv.int_key(), 100);
    EXPECT_EQ(kv.kv.string_value(), "TestValue");
}

// Test for Numeric Comparison of Keys
TEST(KeyValueWrapperTest, NumericComparison) {
    KeyValueWrapper kv1(10, "A");
    KeyValueWrapper kv2(20, "B");

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

// Test for Lexicographical Comparison of Keys
TEST(KeyValueWrapperTest, LexicographicalComparison) {
    KeyValueWrapper kv1("Apple", 1);
    KeyValueWrapper kv2("Banana", 2);

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

// Test for Mixed Type Comparison Failure
TEST(KeyValueWrapperTest, MixedTypeComparison) {
    KeyValueWrapper kv1(100, "Test");    // Numeric key
    KeyValueWrapper kv2("Key", 50);      // String key

    EXPECT_TRUE(kv1 < kv2);  // Numeric is always smaller than string
}

// Test for Key and Value Printing
TEST(KeyValueWrapperTest, PrintKeyValue) {
    KeyValueWrapper kv(42, "Hello");

    testing::internal::CaptureStdout();
    kv.printKeyValue();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Key: 42"), std::string::npos);
    EXPECT_NE(output.find("Value: Hello"), std::string::npos);
}

// Test for Large Integer Keys and Values
TEST(KeyValueWrapperTest, LargeIntegers) {
    long long largeKey = 9223372036854775807LL;  // Largest 64-bit signed integer
    KeyValueWrapper kv(largeKey, largeKey);

    EXPECT_EQ(kv.kv.long_key(), largeKey);
    EXPECT_EQ(kv.kv.long_value(), largeKey);
}

// Test for Double Keys and Values
TEST(KeyValueWrapperTest, DoubleValues) {
    KeyValueWrapper kv(3.14159, 2.71828);

    EXPECT_EQ(kv.kv.double_key(), 3.14159);
    EXPECT_EQ(kv.kv.double_value(), 2.71828);
}

// Test for Character Keys and Values
TEST(KeyValueWrapperTest, CharValues) {
    KeyValueWrapper kv('A', 'B');

    EXPECT_EQ(kv.kv.char_key(), "A");
    EXPECT_EQ(kv.kv.char_value(), "B");
}

TEST(KeyValueWrapperTest, CompareIntWithLong) {
    KeyValueWrapper kv1(100, 0);  // int key
    KeyValueWrapper kv2(100LL, 0);  // long long key

    EXPECT_FALSE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
    EXPECT_TRUE(kv1 == kv2);
}

TEST(KeyValueWrapperTest, CompareIntWithDouble) {
    KeyValueWrapper kv1(100, 0);  // int key
    KeyValueWrapper kv2(100.0, 0);  // double key

    EXPECT_FALSE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
    EXPECT_TRUE(kv1 == kv2);
}

TEST(KeyValueWrapperTest, CompareIntWithString) {
    KeyValueWrapper kv1(100, 0);  // int key
    KeyValueWrapper kv2("abc", 0);  // string key

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

TEST(KeyValueWrapperTest, CompareDoubleWithString) {
    KeyValueWrapper kv1(100.5, 0);  // double key
    KeyValueWrapper kv2("xyz", 0);  // string key

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

TEST(KeyValueWrapperTest, CompareCharWithString) {
    KeyValueWrapper kv1('a', 0);  // char key
    KeyValueWrapper kv2("abc", 0);  // string key

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

TEST(KeyValueWrapperTest, CompareCharWithDouble) {
    KeyValueWrapper kv1('a', 0);  // char key
    KeyValueWrapper kv2(99.99, 0);  // double key

    EXPECT_FALSE(kv1 > kv2);
    EXPECT_TRUE(kv2 > kv1);
}

TEST(KeyValueWrapperTest, CompareStringWithString) {
    KeyValueWrapper kv1("abc", 0);  // string key
    KeyValueWrapper kv2("xyz", 0);  // string key

    EXPECT_TRUE(kv1 < kv2);
    EXPECT_FALSE(kv2 < kv1);
}

TEST(KeyValueWrapperTest, CompareDefaultKeyValues) {
    KeyValueWrapper kv1;  // Default KeyValueWrapper (no key set)
    KeyValueWrapper kv2(0, 0);  // Explicitly initialized to 0

    EXPECT_TRUE(kv1 == kv2);  // Default and explicit 0 should be equal
}

// Test the printKeyValue method
TEST(KeyValueWrapperTest, Print_KeyValue) {
    // Create a KeyValueWrapper instance
    KeyValueWrapper kv(100, "testValue");

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
