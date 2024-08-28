//
// Created by Damian Li on 2024-08-28.
//
#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
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

TEST(APITest, CloseAndCleanup) {
    API* api = new API();
    std::string db_name = "test_db_cleanup";

    // Open a database to initialize the API
    api->Open(db_name);

    testing::internal::CaptureStdout();
    api->Close();
    std::string output = testing::internal::GetCapturedStdout();

    // Check that the cleanup message is printed
    EXPECT_TRUE(output.find("Cleanup completed.") != std::string::npos);

    // Verify that memtable is cleaned up (set to nullptr)
    EXPECT_EQ(api->GetMemtable(), nullptr);  // Assuming GetMemtable() is a getter for the memtable pointer

    // Clean up
    delete api;
    fs::remove_all(db_name);  // Clean up the created directory
}
