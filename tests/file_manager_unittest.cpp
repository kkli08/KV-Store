//
// Created by Damian Li on 2024-09-08.
//

#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include "FileManager.h"

namespace fs = std::filesystem;

TEST(FileManagerTest, FlushEmptyKVPairVector) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> empty_kv_pairs;

    FlushSSTInfo info = fileManager.flushToDisk(empty_kv_pairs);

    // Verify the filename is generated correctly
    EXPECT_FALSE(info.fileName.empty());
    EXPECT_EQ(info.smallest_key.isEmpty(), true);
    EXPECT_EQ(info.largest_key.isEmpty(), true);

    // Clean up: Delete the test directory
    fs::remove_all("test_db");
}

TEST(FileManagerTest, FlushSingleKVPair) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> kv_pairs = { KeyValue(1, "one") };

    FlushSSTInfo info = fileManager.flushToDisk(kv_pairs);

    // Verify filename is generated
    EXPECT_FALSE(info.fileName.empty());

    // Verify smallest and largest keys
    EXPECT_EQ(std::get<int>(info.smallest_key.getKey()), 1);
    EXPECT_EQ(std::get<int>(info.largest_key.getKey()), 1);

    // Clean up: Delete the test directory
    fs::remove_all("test_db");
}

TEST(FileManagerTest, FlushMultipleKVPairSameType) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> kv_pairs = { KeyValue(1, "one"), KeyValue(2, "two"), KeyValue(3, "three") };

    FlushSSTInfo info = fileManager.flushToDisk(kv_pairs);

    // Verify filename is generated
    EXPECT_FALSE(info.fileName.empty());

    // Verify smallest and largest keys
    EXPECT_EQ(std::get<int>(info.smallest_key.getKey()), 1);
    EXPECT_EQ(std::get<int>(info.largest_key.getKey()), 3);

    // Clean up: Delete the test directory
    fs::remove_all("test_db");
}

TEST(FileManagerTest, FlushMultipleKVPairDifferentTypes) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> kv_pairs = { KeyValue(1, "one"), KeyValue(3.14, 'a'), KeyValue(100LL, 200LL) };

    FlushSSTInfo info = fileManager.flushToDisk(kv_pairs);

    // Verify filename is generated
    EXPECT_FALSE(info.fileName.empty());

    // Verify smallest and largest keys
    EXPECT_EQ(std::get<int>(info.smallest_key.getKey()), 1);
    EXPECT_EQ(std::get<long long>(info.largest_key.getKey()), 100LL);

    // Clean up: Delete the test directory
    fs::remove_all("test_db");
}

TEST(FileManagerTest, VerifyFlushedFileContent) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> kv_pairs = { KeyValue(1, 100), KeyValue(2, 200) };

    FlushSSTInfo info = fileManager.flushToDisk(kv_pairs);

    // Open the SST file and check its content
    std::ifstream file("test_db/" + info.fileName, std::ios::binary);
    ASSERT_TRUE(file.is_open());

    // Deserialize the header
    SSTHeader header = SSTHeader::deserialize(file);
    EXPECT_EQ(header.num_key_values, 2);

    // Deserialize and check the first KeyValue pair
    SerializedKeyValue skv1 = SerializedKeyValue::deserialize(file);
    EXPECT_EQ(std::get<int>(skv1.kv.getKey()), 1);
    EXPECT_EQ(std::get<int>(skv1.kv.getValue()), 100);

    // Deserialize and check the second KeyValue pair
    SerializedKeyValue skv2 = SerializedKeyValue::deserialize(file);
    EXPECT_EQ(std::get<int>(skv2.kv.getKey()), 2);
    EXPECT_EQ(std::get<int>(skv2.kv.getValue()), 200);

    file.close();

    // Clean up: Delete the test directory
    fs::remove_all("test_db");
}


TEST(FileManagerTest, LoadSingleKVPairFromDisk) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> kv_pairs = { KeyValue(1, "one") };

    // Flush the data to disk
    FlushSSTInfo info = fileManager.flushToDisk(kv_pairs);

    // Load data back into RedBlackTree
    RedBlackTree* tree = fileManager.loadFromDisk(info.fileName);

    // Verify the tree contains the key-value pair
    KeyValue result = tree->getValue(KeyValue(1, ""));
    EXPECT_FALSE(result.isEmpty());
    EXPECT_EQ(std::get<int>(result.getKey()), 1);
    EXPECT_EQ(std::get<std::string>(result.getValue()), "one");

    delete tree;
    fs::remove_all("test_db");
}


TEST(FileManagerTest, LoadMultipleKVPairsFromDisk) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> kv_pairs = { KeyValue(1, "one"), KeyValue(2, "two"), KeyValue(3, "three") };

    // Flush the data to disk
    FlushSSTInfo info = fileManager.flushToDisk(kv_pairs);

    // Load data back into RedBlackTree
    RedBlackTree* tree = fileManager.loadFromDisk(info.fileName);

    // Verify the tree contains all key-value pairs
    KeyValue result1 = tree->getValue(KeyValue(1, ""));
    KeyValue result2 = tree->getValue(KeyValue(2, ""));
    KeyValue result3 = tree->getValue(KeyValue(3, ""));

    EXPECT_FALSE(result1.isEmpty());
    EXPECT_FALSE(result2.isEmpty());
    EXPECT_FALSE(result3.isEmpty());

    EXPECT_EQ(std::get<int>(result1.getKey()), 1);
    EXPECT_EQ(std::get<std::string>(result1.getValue()), "one");

    EXPECT_EQ(std::get<int>(result2.getKey()), 2);
    EXPECT_EQ(std::get<std::string>(result2.getValue()), "two");

    EXPECT_EQ(std::get<int>(result3.getKey()), 3);
    EXPECT_EQ(std::get<std::string>(result3.getValue()), "three");

    delete tree;
    fs::remove_all("test_db");
}


TEST(FileManagerTest, LoadDifferentTypeKVPairsFromDisk) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> kv_pairs = {
        KeyValue(1, "one"), KeyValue(3.14, 'a'), KeyValue(100LL, 200LL)
    };

    // Flush the data to disk
    FlushSSTInfo info = fileManager.flushToDisk(kv_pairs);

    // Load data back into RedBlackTree
    RedBlackTree* tree = fileManager.loadFromDisk(info.fileName);

    // Verify the tree contains all key-value pairs
    KeyValue result1 = tree->getValue(KeyValue(1, ""));
    KeyValue result2 = tree->getValue(KeyValue(100LL, 0LL));
    KeyValue result3 = tree->getValue(KeyValue(3.14, 0));

    EXPECT_FALSE(result1.isEmpty());
    EXPECT_FALSE(result2.isEmpty());
    EXPECT_FALSE(result3.isEmpty());

    EXPECT_EQ(std::get<int>(result1.getKey()), 1);
    EXPECT_EQ(std::get<std::string>(result1.getValue()), "one");

    EXPECT_EQ(std::get<long long>(result2.getKey()), 100LL);
    EXPECT_EQ(std::get<long long>(result2.getValue()), 200LL);

    EXPECT_EQ(std::get<double>(result3.getKey()), 3.14);
    EXPECT_EQ(std::get<char>(result3.getValue()), 'a');

    delete tree;
    fs::remove_all("test_db");
}

TEST(FileManagerTest, LoadFromEmptyFile) {
    FileManager fileManager(fs::path("test_db"));
    std::vector<KeyValue> empty_kv_pairs;

    // Flush empty data to disk
    FlushSSTInfo info = fileManager.flushToDisk(empty_kv_pairs);

    // Load data back into RedBlackTree
    RedBlackTree* tree = fileManager.loadFromDisk(info.fileName);

    // Verify the tree contains no key-value pairs
    KeyValue result = tree->getValue(KeyValue(1, ""));
    EXPECT_TRUE(result.isEmpty());

    delete tree;
    fs::remove_all("test_db");
}


