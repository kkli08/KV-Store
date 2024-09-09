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
 * void Memtable::put(const KeyValue&)
 * Memtable::get(const KeyValue&) -> return KeyValue
 */
TEST(MemtableTest, BasicInsertAndGet) {
    Memtable* memtable = new Memtable(3);  // Set a small threshold for testing

    // Insert a KeyValue pair (int key, int value)
    KeyValue kv1(10, 100);
    memtable->put(kv1);

    // Retrieve the value using the key
    KeyValue result = memtable->get(KeyValue(10, ""));
    EXPECT_EQ(std::get<int>(result.getValue()), 100);  // Verify that the value is correct

    delete memtable;
}

TEST(MemtableTest, InsertMultipleKeyValuePairs) {
    Memtable* memtable = new Memtable(3);  // Set a small threshold for testing

    // Insert multiple KeyValue pairs
    memtable->put(KeyValue(10, 100));
    memtable->put(KeyValue(20, 200));
    memtable->put(KeyValue(30, 300));

    // Retrieve the values using the keys
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(10, "")).getValue()), 100);
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(20, "")).getValue()), 200);
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(30, "")).getValue()), 300);

    delete memtable;
}

TEST(MemtableTest, GetValueNotPresent) {
    Memtable* memtable = new Memtable(3);

    // Insert a KeyValue pair
    memtable->put(KeyValue(10, 100));

    // Attempt to retrieve a non-existent key
    KeyValue result = memtable->get(KeyValue(20, ""));
    EXPECT_TRUE(result.isEmpty());  // Check if the returned KeyValue is empty

    delete memtable;
}


TEST(MemtableTest, InsertBeyondThreshold) {
    Memtable* memtable = new Memtable(2);  // Set a threshold of 2

    // Insert two KeyValue pairs (should not trigger a flush)
    memtable->put(KeyValue(10, 100));
    memtable->put(KeyValue(20, 200));
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(10, "")).getValue()), 100);
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(20, "")).getValue()), 200);

    // Insert one more KeyValue pair (should trigger a flush)
    memtable->put(KeyValue(30, 300));
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(30, "")).getValue()), 300);

    delete memtable;
    fs::remove_all("defaultDB");
}

TEST(MemtableTest, ResetCurrentSizeAfterFlush) {
    Memtable* memtable = new Memtable(2);

    // Insert three KeyValue pairs (should trigger a flush after the second)
    memtable->put(KeyValue(10, 100));
    memtable->put(KeyValue(20, 200));
    memtable->put(KeyValue(30, 300));  // This should trigger a flush

    // Insert another KeyValue pair (should be stored after flush)
    memtable->put(KeyValue(40, 400));
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(40, "")).getValue()), 400);

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
    memtable->put(KeyValue(10, 100));
    memtable->put(KeyValue(20, 200));
    memtable->put(KeyValue(30, 300));  // Trigger flush

    // Second flush
    memtable->put(KeyValue(40, 400));
    memtable->put(KeyValue(50, 500));
    memtable->put(KeyValue(60, 600));

    // Retrieve the most recent entries
    EXPECT_NE(std::get<int>(memtable->get(KeyValue(40, "")).getValue()), 400);
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(50, "")).getValue()), 500);
    EXPECT_EQ(std::get<int>(memtable->get(KeyValue(60, "")).getValue()), 600);

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
    std::regex pattern(R"(\d{1}_\d{4}_\d{2}_\d{2}_\d{4}\.sst)");
    EXPECT_TRUE(std::regex_match(filename, pattern));

    delete memtable;
}

TEST(MemtableTest, GenerateSstFilenameConsecutiveCalls) {
    Memtable* memtable = new Memtable();

    std::string filename1 = memtable->generateSstFilename();
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Ensure at least 1-second difference
    std::string filename2 = memtable->generateSstFilename();

    // The filenames should be different if the function is called at different times
    EXPECT_NE(filename1, filename2);

    delete memtable;
}

TEST(MemtableTest, GenerateSstFilenameIncludesCurrentTime) {
    Memtable* memtable = new Memtable();

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "0_%Y_%m_%d_%H%M");

    std::string expected_prefix = ss.str();
    std::string filename = memtable->generateSstFilename();

    // The generated filename should start with the current date and time
    EXPECT_TRUE(filename.find(expected_prefix) == 0);

    delete memtable;
}


