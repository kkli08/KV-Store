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
 * string Memtable::generateSstFilename()
 */
TEST(MemtableTest, GenerateSstFilenameBasic) {
    Memtable* memtable = new Memtable();

    std::string filename = memtable->generateSstFilename();

    // Expected format: YYYY_MM_DD_HHMMSS.sst
    std::regex pattern(R"(\d{4}_\d{2}_\d{2}_\d{6}\.sst)");
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
    ss << std::put_time(std::localtime(&in_time_t), "%Y_%m_%d_%H%M%S");

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
