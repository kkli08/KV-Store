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


// replace destructor with smart pointer

// TEST(APITest, CloseAndCleanup) {
//     API* api = new API();
//     std::string db_name = "test_db_cleanup";
//
//     // Open a database to initialize the API
//     api->Open(db_name);
//
//     testing::internal::CaptureStdout();
//     api->Close();
//     std::string output = testing::internal::GetCapturedStdout();
//
//     // Check that the cleanup message is printed
//     EXPECT_TRUE(output.find("Cleanup completed.") != std::string::npos);
//
//     // Verify that memtable is cleaned up (set to nullptr)
//     EXPECT_EQ(api->GetMemtable(), nullptr);  // Assuming GetMemtable() is a getter for the memtable pointer
//
//     // Clean up
//     delete api;
//     fs::remove_all(db_name);  // Clean up the created directory
// }

TEST(APITest, reOpen) {
    API* api = new API();
    std::string db_name = "test_db_reopen";
    api->Open(db_name);
    api->Close();
    // api = new API();
    api->Open(db_name);
    api->Close();
}

// 10 k pairs Put & Get
TEST(APITest, InsertAndRetrieve10KKeyValuePairs) {
    API* api = new API();
    std::string db_name = "test_db_performance";

    // Ensure the directory exists before the test
    if (!fs::exists(db_name)) {
        fs::create_directory(db_name);
    }

    // Open the database
    api->Open(db_name);

    // Measure the time taken to put 10,000 key-value pairs
    auto start_put = std::chrono::high_resolution_clock::now();
    for (long long i = 1; i <= 10000; ++i) {
        api->Put(i, i * 10);  // Inserting key i with value i*10
    }
    cout << endl << "Perform Index Check: " << endl;
    api->IndexCheck();
    auto end_put = std::chrono::high_resolution_clock::now();
    auto put_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_put - start_put).count();

    std::cout << "Total time taken for Put operations: " << put_duration << " ms" << std::endl;

    // Measure the time taken to get 10,000 key-value pairs
    auto start_get = std::chrono::high_resolution_clock::now();
    bool all_values_correct = true;
    for (long long i = 1; i <= 10000; ++i) {
        long long value = api->Get(i);
        if (value != i * 10) {
            all_values_correct = false;
            break;
        }
    }
    auto end_get = std::chrono::high_resolution_clock::now();
    auto get_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_get - start_get).count();
    double avg_get_time_per_search = static_cast<double>(get_duration) / 10000.0;

    std::cout << "Total time taken for Get operations: " << get_duration << " ms" << std::endl;
    std::cout << "Average time per Get operation: " << avg_get_time_per_search << " ms" << std::endl;

    // Verify all values were retrieved correctly
    EXPECT_TRUE(all_values_correct);

    // Close the database and clean up
    api->Close();
    delete api;
    fs::remove_all(db_name);  // Clean up the created directory
}

TEST(APITest, InsertAndRetrieve1MKeyValuePairs) {
    const long long num_pairs = 1e5;  // 1 million key-value pairs
    API* api = new API();
    std::string db_name = "test_db_performance_1M";

    // Ensure the directory exists before the test
    if (!fs::exists(db_name)) {
        fs::create_directory(db_name);
    }

    // Open the database
    api->Open(db_name);

    // Measure the time taken to put 1 million key-value pairs
    auto start_put = std::chrono::high_resolution_clock::now();
    for (long long i = 1; i <= num_pairs; ++i) {
        api->Put(i, i * 10);  // Inserting key i with value i*10
    }

    // Debug
    // api->IndexCheck();

    auto end_put = std::chrono::high_resolution_clock::now();
    auto put_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_put - start_put).count();

    // int failures = 0;
    std::cout << "Total time taken for Put operations (1M pairs): " << put_duration << " ms" << std::endl;

    // Measure the time taken to get 1 million key-value pairs
    auto start_get = std::chrono::high_resolution_clock::now();
    bool all_values_correct = true;
    for (long long i = 1; i <= num_pairs; ++i) {
        long long value = api->Get(i);
        if (value != i * 10) {
            all_values_correct = false;
            // failures++;
            break;
        }
    }
    auto end_get = std::chrono::high_resolution_clock::now();
    auto get_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_get - start_get).count();
    double avg_get_time_per_search = static_cast<double>(get_duration) / num_pairs;

    std::cout << "Total time taken for Get operations (1M pairs): " << get_duration << " ms" << std::endl;
    std::cout << "Average time per Get operation: " << avg_get_time_per_search << " ms" << std::endl;
    // std::cout << "Failures: " << failures << std::endl;
    // Verify all values were retrieved correctly
    EXPECT_TRUE(all_values_correct);

    // Close the database and clean up
    api->Close();
    delete api;
    fs::remove_all(db_name);  // Clean up the created directory
}

/*
 * Unit Tests for SCAN(LL k1, LL k2)
 *
 */
TEST(APITest, ScanAcrossSSTsAndMemtable) {
    API* api = new API();
    std::string db_name = "test_db";

    // Open the database
    api->Open(db_name);

    // Insert 1e5 key-value pairs using the API's Put method
    for (long long i = 1; i <= 1e6; ++i) {
        api->Put(i, i * 10);
    }

    // Perform a scan that includes both SST files and the memtable
    long long small_key = 150000;
    long long large_key = 600000;
    unordered_map<long long, long long> result = api->Scan(small_key, large_key);

    // Validate the results
    EXPECT_EQ(result.size(), large_key - small_key + 1);
    for (long long i = small_key; i <= large_key; ++i) {
        EXPECT_EQ(result[i], i * 10);
    }

    // Cleanup
    api->Close();
    delete api;
    fs::remove_all(db_name);  // Remove the test database directory
}

TEST(APITest, ScanWithOverlappingSSTAndMemtableData) {
    API* api = new API();
    std::string db_name = "test_db";

    // Open the database
    api->Open(db_name);

    // Insert 1e5 key-value pairs using the API's Put method
    for (long long i = 1; i <= 1e6; ++i) {
        api->Put(i, i * 10);
    }

    // Flush memtable to SST (simulate this by closing and reopening the database)
    api->Close();
    api->Open(db_name);

    // Insert another set of data into the memtable
    for (long long i = 1e5 + 1; i <= 1e5 + 5000; ++i) {
        api->Put(i, i * 10);
    }

    // Perform a scan that overlaps SST files and the memtable
    long long small_key = 99000;
    long long large_key = 105000;
    unordered_map<long long, long long> result = api->Scan(small_key, large_key);

    // Validate the results
    EXPECT_EQ(result.size(), large_key - small_key + 1);
    for (long long i = small_key; i <= large_key; ++i) {
        EXPECT_EQ(result[i], i * 10);
    }

    // Cleanup
    api->Close();
    delete api;
    fs::remove_all(db_name);  // Remove the test database directory
}
