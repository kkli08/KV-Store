//
// Created by Damian Li on 2024-09-07.
//

#include <gtest/gtest.h>
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