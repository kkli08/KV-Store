//
// Created by Damian Li on 2024-08-28.
//
#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iostream>
#include "api.h"

namespace fs = std::filesystem;
using namespace kvdb;

TEST(APITest, OpenNewDatabase) {
    API* api = new API();
    std::string db_name = "test_db";

    // Ensure the directory does not exist before the test
    if (fs::exists(db_name)) {
        fs::remove_all(db_name);
    }

    testing::internal::CaptureStdout();
    api->Open(db_name);
    std::string output = testing::internal::GetCapturedStdout();

    // Verify the directory was created
    EXPECT_TRUE(fs::exists(db_name));
    // Verify output
    EXPECT_TRUE(output.find("Created database directory: " + db_name) != std::string::npos);

    // Clean up
    delete api;
    fs::remove_all(db_name);  // Clean up the created directory
}

TEST(APITest, OpenExistingDatabase) {
    API* api = new API();
    std::string db_name = "test_db_existing";

    // Create the directory before the test
    if (!fs::exists(db_name)) {
        fs::create_directory(db_name);
    }

    testing::internal::CaptureStdout();
    api->Open(db_name);
    std::string output = testing::internal::GetCapturedStdout();

    // Verify the directory exists
    EXPECT_TRUE(fs::exists(db_name));
    // Verify output
    EXPECT_TRUE(output.find("Existed database directory: " + db_name) != std::string::npos);

    // Clean up
    delete api;
    fs::remove_all(db_name);  // Clean up the created directory
}


TEST(APITest, reOpen) {
    API* api = new API();
    std::string db_name = "test_db_reopen";
    api->Open(db_name);
    api->Close();
    // api = new API();
    api->Open(db_name);
    api->Close();
}

TEST(APITest, BasicInsertAndGet) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert key-value pairs (int key, int value)
    db->Put(1, 100);
    db->Put(2, 200);

    // Retrieve the values using the keys
    KeyValue result1 = db->Get(KeyValue(1, ""));
    KeyValue result2 = db->Get(KeyValue(2, ""));

    // Verify the returned values are correct
    EXPECT_EQ(std::get<int>(result1.getValue()), 100);
    EXPECT_EQ(std::get<int>(result2.getValue()), 200);

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}

TEST(APITest, StringInsertAndGet) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert key-value pairs (string key, string value)
    db->Put("key1", "value1");
    db->Put("key2", "value2");

    // Retrieve the values using the keys
    KeyValue result1 = db->Get(KeyValue("key1", ""));
    KeyValue result2 = db->Get(KeyValue("key2", ""));

    // Verify the returned values are correct
    EXPECT_EQ(std::get<std::string>(result1.getValue()), "value1");
    EXPECT_EQ(std::get<std::string>(result2.getValue()), "value2");

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}


TEST(APITest, MixedDataTypesInsertAndGet) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert key-value pairs with different types
    db->Put(1, "value1");
    db->Put("key2", 200);
    db->Put(3.5, "value3");
    db->Put("key4", 400.5);

    // Retrieve the values using the keys
    KeyValue result1 = db->Get(KeyValue(1, ""));
    KeyValue result2 = db->Get(KeyValue("key2", ""));
    KeyValue result3 = db->Get(KeyValue(3.5, ""));
    KeyValue result4 = db->Get(KeyValue("key4", ""));

    // Verify the returned values are correct
    EXPECT_EQ(std::get<std::string>(result1.getValue()), "value1");
    EXPECT_EQ(std::get<int>(result2.getValue()), 200);
    EXPECT_EQ(std::get<std::string>(result3.getValue()), "value3");
    EXPECT_EQ(std::get<double>(result4.getValue()), 400.5);

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}

TEST(APITest, InsertAndGetLargeNumberOfKVPairs) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert 100,000 key-value pairs (int key, int value)
    cout << "Inserting 100,000 key-value pairs ..." << endl;
    for (int i = 0; i < 100000; ++i) {
        db->Put(i, i * 10);
    }
    cout << "Put operations finished. " << endl;
    cout << "Retrieve 10,000 key-value pairs..." << endl;
    // Retrieve 10,000 key-value pairs and verify them
    for (int i = 400; i < 500; ++i) {
        KeyValue result = db->Get(KeyValue(i, ""));
        EXPECT_EQ(std::get<int>(result.getValue()), i * 10);
    }
    cout << "Retrieve operations finished. " << endl;
    // Clean up
    db->Close();
    fs::remove_all("test_db");
}


/*
 * API::Scan
 */
TEST(APITest, BasicScanRange) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert key-value pairs (int key, int value)
    db->Put(1, 100);
    db->Put(2, 200);
    db->Put(3, 300);
    db->Put(4, 400);
    db->Put(5, 500);

    // Perform a scan for key-value pairs between 2 and 4
    set<KeyValue> resultSet = db->Scan(KeyValue(2, ""), KeyValue(4, ""));

    // Verify the result
    ASSERT_EQ(resultSet.size(), 3);
    auto it = resultSet.begin();
    EXPECT_EQ(std::get<int>(it->getKey()), 2);
    EXPECT_EQ(std::get<int>(it->getValue()), 200);
    ++it;
    EXPECT_EQ(std::get<int>(it->getKey()), 3);
    EXPECT_EQ(std::get<int>(it->getValue()), 300);
    ++it;
    EXPECT_EQ(std::get<int>(it->getKey()), 4);
    EXPECT_EQ(std::get<int>(it->getValue()), 400);

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}

TEST(APITest, ScanNonExistentRange) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert key-value pairs
    db->Put(1, 100);
    db->Put(2, 200);
    db->Put(3, 300);

    // Perform a scan for a range with no matching key-value pairs
    set<KeyValue> resultSet = db->Scan(KeyValue(10, ""), KeyValue(20, ""));

    // Verify that no key-value pairs were found
    EXPECT_EQ(resultSet.size(), 0);

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}


TEST(APITest, ScanSingleKeyValuePair) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert key-value pairs
    db->Put(1, 100);
    db->Put(2, 200);
    db->Put(3, 300);

    // Perform a scan for a single key-value pair (key = 2)
    set<KeyValue> resultSet = db->Scan(KeyValue(2, ""), KeyValue(2, ""));

    // Verify that only one key-value pair was found
    ASSERT_EQ(resultSet.size(), 1);
    auto it = resultSet.begin();
    EXPECT_EQ(std::get<int>(it->getKey()), 2);
    EXPECT_EQ(std::get<int>(it->getValue()), 200);

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}

TEST(APITest, LargeScaleScan) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert 1000 key-value pairs
    for (int i = 0; i < 1000; ++i) {
        db->Put(i, i * 10);
    }

    // Perform a scan for key-value pairs between 100 and 900
    set<KeyValue> resultSet = db->Scan(KeyValue(100, ""), KeyValue(900, ""));

    // Verify that the correct number of key-value pairs were found
    EXPECT_EQ(resultSet.size(), 801);

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}

TEST(APITest, ScanForStringKeyValuePairs) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert key-value pairs with string keys and values
    db->Put("a", "value1");
    db->Put("b", "value2");
    db->Put("c", "value3");

    // Perform a scan for key-value pairs between "a" and "b"
    set<KeyValue> resultSet = db->Scan(KeyValue("a", ""), KeyValue("b", ""));

    // Verify that the correct key-value pairs were found
    ASSERT_EQ(resultSet.size(), 2);
    auto it = resultSet.begin();
    EXPECT_EQ(std::get<std::string>(it->getKey()), "a");
    EXPECT_EQ(std::get<std::string>(it->getValue()), "value1");
    ++it;
    EXPECT_EQ(std::get<std::string>(it->getKey()), "b");
    EXPECT_EQ(std::get<std::string>(it->getValue()), "value2");

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}

// Performance issue
TEST(APITest, LargeScaleInsertAndScanRange) {
    // Set memtable size and create API instance
    int memtableSize = 1000;
    auto db = std::make_unique<kvdb::API>(memtableSize);
    db->Open("test_db");

    // Insert 100,000 key-value pairs (int key, int value)
    for (int i = 0; i < 1000000; ++i) {
        // db->Put(i, i * 10);  // Key is i, Value is i * 10
        db->Put(i, "value_"+std::to_string(i));
    }

    // Perform a scan for key-value pairs between 50,000 and 51,000
    set<KeyValue> resultSet = db->Scan(KeyValue(1, ""), KeyValue(50000, ""));

    // Verify that 1,001 key-value pairs were found
    ASSERT_EQ(resultSet.size(), 50000);

    // Verify the content of the result set
    auto it = resultSet.begin();
    for (int i = 1; i <= 50000; ++i, ++it) {
        EXPECT_EQ(std::get<int>(it->getKey()), i);
        // EXPECT_EQ(std::get<int>(it->getValue()), i * 10);
    }

    // Clean up
    db->Close();
    fs::remove_all("test_db");
}
