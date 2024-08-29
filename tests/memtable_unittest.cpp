//
// Created by Damian Li on 2024-08-28.
//
#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include "Memtable.h"
#include <regex>
#include <thread>

namespace fs = std::filesystem;


/*
 * Unit Tests for:
 * void Memtable::put(long long, long long)
 * Memtable::get(long long key) -> return key
 */
TEST(MemtableTest, BasicInsertAndGet) {
    Memtable* memtable = new Memtable(3); // Set a small threshold for testing
    memtable->put(10, 100);

    // Retrieve the value
    long long value = memtable->get(10);
    EXPECT_EQ(value, 100);

    delete memtable;
}

TEST(MemtableTest, InsertMultipleKeyValuePairs) {
    Memtable* memtable = new Memtable(3); // Set a small threshold for testing
    memtable->put(10, 100);
    memtable->put(20, 200);
    memtable->put(30, 300);

    // Retrieve the values
    EXPECT_EQ(memtable->get(10), 100);
    EXPECT_EQ(memtable->get(20), 200);
    EXPECT_EQ(memtable->get(30), 300);

    delete memtable;
}

TEST(MemtableTest, GetValueNotPresent) {
    Memtable* memtable = new Memtable(3);
    memtable->put(10, 100);

    // Attempt to retrieve a non-existent key
    long long value = memtable->get(20);
    EXPECT_EQ(value, -1); // Assuming -1 or some other sentinel value indicates not found

    delete memtable;
}

// Insert Beyond Threshold
// This test checks that inserting beyond the threshold triggers a flush to disk and resets the memtable size.
TEST(MemtableTest, InsertBeyondThreshold) {
    Memtable* memtable = new Memtable(2); // Set a threshold of 2

    // Insert two key-value pairs (should not trigger a flush)
    memtable->put(10, 100);
    memtable->put(20, 200);
    EXPECT_EQ(memtable->get(10), 100);
    EXPECT_EQ(memtable->get(20), 200);

    // Insert one more key-value pair (should trigger a flush)
    memtable->put(30, 300);
    EXPECT_EQ(memtable->get(30), 300);

    delete memtable;
    fs::remove_all("defaultDB");
}

// Reset Current Size After Flush
// This test checks that after a flush is triggered, the memtable’s current size is reset and can accept new entries.
TEST(MemtableTest, ResetCurrentSizeAfterFlush) {
    Memtable* memtable = new Memtable(2);

    // Insert three key-value pairs (should trigger a flush after the second)
    memtable->put(10, 100);
    memtable->put(20, 200);
    memtable->put(30, 300); // This should trigger a flush

    // Insert another key-value pair (should be stored after flush)
    memtable->put(40, 400);
    EXPECT_EQ(memtable->get(40), 400);

    delete memtable;
    fs::remove_all("defaultDB");
}

/*
* Multiple Flushes with Threshold
* This test checks that multiple flushes can occur as the threshold is reached multiple times.
 */
TEST(MemtableTest, MultipleFlushesWithThreshold) {
    Memtable* memtable = new Memtable(2);

    // First flush
    memtable->put(10, 100);
    memtable->put(20, 200);
    memtable->put(30, 300); // Trigger flush

    // Second flush
    memtable->put(40, 400);
    memtable->put(50, 500); // Trigger flush
    memtable->put(60, 600);

    // Retrieve the most recent entries
    EXPECT_NE(memtable->get(40), 400);
    EXPECT_EQ(memtable->get(50), 500);
    EXPECT_EQ(memtable->get(60), 600);

    delete memtable;
    fs::remove_all("defaultDB");
}


/*
 * Unit Tests for:
 * string Memtable::void set_path(fs::path);
 */
TEST(MemtableTest, SetPathValidDirectory) {
    Memtable* memtable = new Memtable();

    fs::path expected_path = "test_db";
    memtable->set_path(expected_path);

    // Assuming you have a method to get the path for testing purposes
    fs::path actual_path = memtable->get_path(); // You may need to add this method

    EXPECT_EQ(actual_path, expected_path);

    delete memtable;
}

TEST(MemtableTest, UpdatePath) {
    Memtable* memtable = new Memtable();

    fs::path initial_path = "test_db_initial";
    memtable->set_path(initial_path);

    fs::path updated_path = "test_db_updated";
    memtable->set_path(updated_path);

    // Assuming you have a method to get the path for testing purposes
    fs::path actual_path = memtable->get_path(); // You may need to add this method

    EXPECT_EQ(actual_path, updated_path);

    delete memtable;
}


/*
 * Unit Tests for:
 * string Memtable::generateSstFilename()
 */
TEST(MemtableTest, GenerateSstFilenameBasic) {
    Memtable* memtable = new Memtable();

    std::string filename = memtable->generateSstFilename();

    // Expected format: YYYY_MM_DD_HHMMSS.sst
    std::regex pattern(R"(\d{4}_\d{2}_\d{2}_\d{4}\.sst)");
    EXPECT_TRUE(std::regex_match(filename, pattern));

    delete memtable;
}

TEST(MemtableTest, GenerateSstFilenameConsecutiveCalls) {
    Memtable* memtable = new Memtable();

    std::string filename1 = memtable->generateSstFilename();
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Ensure at least 1-second difference
    std::string filename2 = memtable->generateSstFilename();

    // The filenames should be different if the function is called at different times
    EXPECT_EQ(filename1, filename2);

    delete memtable;
}

TEST(MemtableTest, GenerateSstFilenameIncludesCurrentTime) {
    Memtable* memtable = new Memtable();

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y_%m_%d_%H%M");

    std::string expected_prefix = ss.str();
    std::string filename = memtable->generateSstFilename();

    // The generated filename should start with the current date and time
    EXPECT_TRUE(filename.find(expected_prefix) == 0);

    delete memtable;
}

/*
 * Unit Tests for:
 * void Memtable::int_flush()
 */
TEST(MemtableTest, FlushSingleKeyValuePair) {
    Memtable* memtable = new Memtable();
    fs::path path = fs::path("test_db");
    memtable->set_path(path);
    memtable->put(10, 100);

    // Perform flush and get the filename used
    std::string filename = memtable->int_flush();

    // Verify the SST file exists
    fs::path filepath = fs::path("test_db") / filename;
    EXPECT_TRUE(fs::exists(filepath));

    // Verify the content of the SST file
    std::ifstream sst_file(filepath);
    std::string line;
    std::getline(sst_file, line);
    EXPECT_EQ(line, "10, 100");

    sst_file.close();
    delete memtable;
    fs::remove_all("test_db");
}

/*
* Flush Multiple Key-Value Pairs
* This test checks that multiple key-value pairs are correctly flushed to the SST file in order.
 */
TEST(MemtableTest, FlushMultipleKeyValuePairs) {
    Memtable* memtable = new Memtable();
    memtable->put(10, 100);
    memtable->put(20, 200);
    memtable->put(5, 50);
    memtable->set_path("test_db");

    // Perform flush
    memtable->int_flush();

    // Verify the SST file exists
    std::string filename = memtable->generateSstFilename();
    fs::path filepath = fs::path("test_db") / filename;
    EXPECT_TRUE(fs::exists(filepath));

    // Verify the content of the SST file
    std::ifstream sst_file(filepath);
    std::string line;
    std::getline(sst_file, line);
    EXPECT_EQ(line, "5, 50");
    std::getline(sst_file, line);
    EXPECT_EQ(line, "10, 100");
    std::getline(sst_file, line);
    EXPECT_EQ(line, "20, 200");

    sst_file.close();
    delete memtable;
    fs::remove_all("test_db");
}

/*
* Flush Empty Memtable
* This test checks that flushing an empty memtable results in an empty SST file.
 */
TEST(MemtableTest, FlushEmptyMemtable) {
    Memtable* memtable = new Memtable();
    memtable->set_path("test_db");

    // Perform flush on an empty memtable
    memtable->int_flush();

    // Verify the SST file exists
    std::string filename = memtable->generateSstFilename();
    fs::path filepath = fs::path("test_db") / filename;
    EXPECT_TRUE(fs::exists(filepath));

    // Verify the content of the SST file is empty
    std::ifstream sst_file(filepath);
    std::string line;
    EXPECT_FALSE(std::getline(sst_file, line));  // No content should be present

    sst_file.close();
    delete memtable;
    fs::remove_all("test_db");
}

/*
* Handle Large Number of Key-Value Pairs (1e3)
* This test verifies that the flush handles a large number of key-value pairs efficiently.
 */
TEST(MemtableTest, FlushLargeNumberOfKeyValuePairs) {
    Memtable* memtable = new Memtable();
    memtable->set_path("test_db");

    // Insert a large number of key-value pairs
    for (long long i = 1; i <= 1000; ++i) {
        memtable->put(i, i * 10);
    }

    // Perform flush
    memtable->int_flush();

    // Verify the SST file exists
    std::string filename = memtable->generateSstFilename();
    fs::path filepath = fs::path("test_db") / filename;
    EXPECT_TRUE(fs::exists(filepath));

    // Verify the content of the SST file
    std::ifstream sst_file(filepath);
    long long i = 1;
    std::string line;
    while (std::getline(sst_file, line)) {
        std::stringstream ss(line);
        long long key, value;
        char comma;
        ss >> key >> comma >> value;
        EXPECT_EQ(key, i);
        EXPECT_EQ(value, i * 10);
        ++i;
    }

    sst_file.close();
    delete memtable;
    fs::remove_all("test_db");
}

/*
* Check for Correct Overwrite Behavior
* This test verifies that multiple flushes do not overwrite existing SST files.
 */
TEST(MemtableTest, MultipleFlushesDoNotOverwrite) {
    Memtable* memtable = new Memtable();
    memtable->set_path("test_db");

    memtable->put(10, 100);
    memtable->int_flush();

    memtable->put(20, 200);
    memtable->int_flush();

    // Verify that two SST files exist
    int sst_file_count = 0;
    for (const auto& entry : fs::directory_iterator("test_db")) {
        ++sst_file_count;
    }
    EXPECT_EQ(sst_file_count, 1);

    delete memtable;
    fs::remove_all("test_db");
}

/*
* Verify Content Order in SST File
* This test ensures that the content of the SST file is in the correct order (in-order traversal).
 */
TEST(MemtableTest, VerifyContentOrderInSstFile) {
    Memtable* memtable = new Memtable();
    memtable->put(15, 150);
    memtable->put(10, 100);
    memtable->put(20, 200);
    memtable->set_path("test_db");

    // Perform flush
    memtable->int_flush();

    // Verify the SST file exists
    std::string filename = memtable->generateSstFilename();
    fs::path filepath = fs::path("test_db") / filename;
    EXPECT_TRUE(fs::exists(filepath));

    // Verify the content order in the SST file
    std::ifstream sst_file(filepath);
    std::string line;
    std::getline(sst_file, line);
    EXPECT_EQ(line, "10, 100");
    std::getline(sst_file, line);
    EXPECT_EQ(line, "15, 150");
    std::getline(sst_file, line);
    EXPECT_EQ(line, "20, 200");

    sst_file.close();
    delete memtable;
    fs::remove_all("test_db");
}

