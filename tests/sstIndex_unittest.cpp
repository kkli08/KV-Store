//
// Created by Damian Li on 2024-08-29.
//
#include <gtest/gtest.h>
#include "SSTIndex.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <deque>
#include <stdexcept>
#include <memory>
#include <algorithm>

namespace fs = std::filesystem;

TEST(SSTIndexTest, AddSingleSSTEntry) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    index->addSST("sst_1.sst", 10, 100);

    std::deque<SSTInfo*> sst_index = index->getSSTsIndex();
    ASSERT_EQ(sst_index.size(), 1);
    EXPECT_EQ(sst_index[0]->filename, "sst_1.sst");
    EXPECT_EQ(sst_index[0]->smallest_key, 10);
    EXPECT_EQ(sst_index[0]->largest_key, 100);

    delete index;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, AddMultipleSSTEntries) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    index->addSST("sst_1.sst", 10, 100);
    index->addSST("sst_2.sst", 200, 300);
    index->addSST("sst_3.sst", 400, 500);

    std::deque<SSTInfo*> sst_index = index->getSSTsIndex();
    ASSERT_EQ(sst_index.size(), 3);
    EXPECT_EQ(sst_index[0]->filename, "sst_1.sst");
    EXPECT_EQ(sst_index[1]->filename, "sst_2.sst");
    EXPECT_EQ(sst_index[2]->filename, "sst_3.sst");

    delete index;
    fs::remove_all("test_db");
}


// Helper function to remove carriage returns (for Windows)
std::string normalize_line_endings(const std::string& str) {
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    return result;
}

TEST(SSTIndexTest, FlushIndexToDisk) {
    SSTIndex* index = new SSTIndex();
    index->set_path(fs::path("test_db"));

    index->addSST("sst_1.sst", 10, 100);
    index->addSST("sst_2.sst", 200, 300);

    // Flush to disk
    index->flushToDisk();

    // Verify the file exists and has correct content
    std::string filename = "Index.sst";
    std::ifstream infile(fs::path("test_db") / filename);
    ASSERT_TRUE(infile.is_open());

    std::string line;

    // Read the first line and normalize the line endings
    std::getline(infile, line);
    line = normalize_line_endings(line);
    EXPECT_EQ(line, "sst_1.sst 10 100");

    // Read the second line and normalize the line endings
    std::getline(infile, line);
    line = normalize_line_endings(line);
    EXPECT_EQ(line, "sst_2.sst 200 300");

    infile.close();
    delete index;
    fs::remove_all("test_db");
}


TEST(SSTIndexTest, RetrieveAllSSTsFromFile) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    // Create an index file
    std::ofstream outfile(fs::path("test_db") / "Index.sst");
    outfile << "sst_1.sst 10 100\n";
    outfile << "sst_2.sst 200 300\n";
    outfile.close();

    // Retrieve all SSTs
    index->getAllSSTs();

    std::deque<SSTInfo*> sst_index = index->getSSTsIndex();
    ASSERT_EQ(sst_index.size(), 2);
    EXPECT_EQ(sst_index[0]->filename, "sst_1.sst");
    EXPECT_EQ(sst_index[1]->filename, "sst_2.sst");

    delete index;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, RetrieveAllSSTsFromEmptyFile) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    // Create an empty index file
    std::ofstream outfile(fs::path("test_db") / "Index.sst");
    outfile.close();

    // Retrieve all SSTs
    index->getAllSSTs();

    std::deque<SSTInfo*> sst_index = index->getSSTsIndex();
    EXPECT_EQ(sst_index.size(), 0);

    delete index;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, RetrieveAllSSTsWhenFileDoesNotExist) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    // Ensure the index file does not exist
    fs::remove(fs::path("test_db") / "Index.sst");

    // Retrieve all SSTs
    index->getAllSSTs();

    std::deque<SSTInfo*> sst_index = index->getSSTsIndex();
    EXPECT_EQ(sst_index.size(), 0);

    delete index;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, AddAndRetrieveMultipleSSTs) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    index->addSST("sst_1.sst", 10, 100);
    index->addSST("sst_2.sst", 200, 300);

    std::deque<SSTInfo*> sst_index = index->getSSTsIndex();
    ASSERT_EQ(sst_index.size(), 2);
    EXPECT_EQ(sst_index[0]->filename, "sst_1.sst");
    EXPECT_EQ(sst_index[0]->smallest_key, 10);
    EXPECT_EQ(sst_index[0]->largest_key, 100);
    EXPECT_EQ(sst_index[1]->filename, "sst_2.sst");
    EXPECT_EQ(sst_index[1]->smallest_key, 200);
    EXPECT_EQ(sst_index[1]->largest_key, 300);

    delete index;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, FlushIndexWithNoSSTs) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    // Flush to disk with no SSTs
    index->flushToDisk();

    // Verify the file exists and is empty
    std::ifstream infile(fs::path("test_db") / "Index.sst");
    ASSERT_TRUE(infile.is_open());
    infile.seekg(0, std::ios::end);
    EXPECT_EQ(infile.tellg(), 0);

    infile.close();
    delete index;
    fs::remove_all("test_db");
}


/*
 * The update feature has not yet been implemented -- 2024-08-29
 * Issue link to this: https://github.com/kkli08/KV-Store/issues/42
 *
 */
// TEST(SSTIndexTest, UpdateExistingSSTInIndex) {
//     SSTIndex* index = new SSTIndex();
//     index->set_path("test_db");
//
//     index->addSST("sst_1.sst", 10, 100);
//     index->addSST("sst_1.sst", 50, 150); // Update with new keys
//
//     std::deque<SSTInfo*> sst_index = index->getSSTsIndex();
//     ASSERT_EQ(sst_index.size(), 1);
//     EXPECT_EQ(sst_index[0]->filename, "sst_1.sst");
//     EXPECT_EQ(sst_index[0]->smallest_key, 50);
//     EXPECT_EQ(sst_index[0]->largest_key, 150);
//
//     delete index;
//     fs::remove_all("test_db");
// }


/*
 * Unit Tests for Binary Search in SST file
 *
 * Test Case 1: Verifies that SearchInSST correctly finds an existing key.
 * Test Case 2: Ensures that SearchInSST returns -1 when the key does not exist.
 * Test Case 3: Checks that SearchInSST returns -1 when searching in an empty SST file.
 * Test Case 4: Tests the behavior of SearchInSST when the SST file contains a malformed line.
 *
 */

TEST(SSTIndexTest, SearchForExistingKey) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    // Create a sample SST file in binary format
    std::ofstream outfile(fs::path("test_db") / "test.sst", std::ios::binary);
    long long key = 10, value = 100;
    outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
    outfile.write(",", 1);
    outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
    outfile.write("\n", 1);

    key = 20; value = 200;
    outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
    outfile.write(",", 1);
    outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
    outfile.write("\n", 1);

    key = 30; value = 300;
    outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
    outfile.write(",", 1);
    outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
    outfile.write("\n", 1);

    outfile.close();

    // Search for a key
    long long search_value = index->SearchInSST("test.sst", 20);
    EXPECT_EQ(search_value, 200);

    delete index;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, SearchForNonExistingKey) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    // Create a sample SST file in binary format
    std::ofstream outfile(fs::path("test_db") / "test.sst", std::ios::binary);
    long long key = 10, value = 100;
    outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
    outfile.write(",", 1);
    outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
    outfile.write("\n", 1);

    key = 20; value = 200;
    outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
    outfile.write(",", 1);
    outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
    outfile.write("\n", 1);

    key = 30; value = 300;
    outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
    outfile.write(",", 1);
    outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
    outfile.write("\n", 1);

    outfile.close();

    // Search for a key that doesn't exist
    long long search_value = index->SearchInSST("test.sst", 40);
    EXPECT_EQ(search_value, -1);

    delete index;
    fs::remove_all("test_db");
}


TEST(SSTIndexTest, SearchInEmptySSTFile) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    // Create an empty SST file
    std::ofstream outfile(fs::path("test_db") / "empty.sst");
    outfile.close();

    // Search for a key in an empty file
    long long value = index->SearchInSST("empty.sst", 10);
    EXPECT_EQ(value, -1);

    delete index;
    fs::remove_all("test_db");
}

// Issue: https://github.com/kkli08/KV-Store/issues/43
// TEST(SSTIndexTest, SearchInSSTWithMalformedLine) {
//     SSTIndex* index = new SSTIndex();
//     index->set_path("test_db");
//
//     // Create a sample SST file with a malformed line in binary mode
//     std::ofstream outfile(fs::path("test_db") / "malformed.sst", std::ios::binary);
//     long long key = 10, value = 100;
//     outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
//     outfile.write(",", 1);
//     outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
//     outfile.write("\n", 1);
//
//     // Write a malformed line (in binary, this would be some invalid data)
//     std::string malformed = "malformed_line";
//     outfile.write(malformed.c_str(), malformed.size());
//     outfile.write("\n", 1);
//
//     // Write a valid key-value pair after the malformed line
//     key = 30; value = 300;
//     outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
//     outfile.write(",", 1);
//     outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
//     outfile.write("\n", 1);
//
//     outfile.close();
//
//     // Search for a key that exists after the malformed line
//     long long search_value = index->SearchInSST("malformed.sst", 30);
//     EXPECT_EQ(search_value, 300);
//
//     delete index;
//     fs::remove_all("test_db");
// }


/*
 * Unit test for SSTIndex::Search(long long _key)
 *
 */

void createSSTFile(const fs::path& filepath, const std::vector<std::pair<long long, long long>>& kv_pairs) {
    std::ofstream sst_file(filepath, std::ios::binary);
    if (!sst_file.is_open()) {
        throw std::runtime_error("Failed to create SST file: " + filepath.string());
    }

    for (const auto& pair : kv_pairs) {
        sst_file.write(reinterpret_cast<const char*>(&pair.first), sizeof(long long));
        sst_file.write(",", 1);
        sst_file.write(reinterpret_cast<const char*>(&pair.second), sizeof(long long));
        sst_file.write("\n", 1);
    }

    sst_file.close();
}


void deleteSSTFile(const fs::path& filepath) {
    if (fs::exists(filepath)) {
        fs::remove(filepath);
    }
}

// Test when SSTIndex is empty
TEST(SSTIndexTest, SearchInEmptyIndex) {
    SSTIndex* sstIndex = new SSTIndex();
    long long result = sstIndex->Search(100);
    EXPECT_EQ(result, -1);  // No SST files, so should return -1
    delete sstIndex;
}

// Test when key is in the youngest SST file
TEST(SSTIndexTest, SearchInYoungestSST) {
    SSTIndex* sstIndex = new SSTIndex();
    sstIndex->set_path(fs::current_path());

    fs::path sst1 = "sst1.sst";
    fs::path sst2 = "sst2.sst";

    createSSTFile(sst1, {{100, 100}, {150, 150}});
    createSSTFile(sst2, {{201, 201}, {250, 250}, {300, 300}});

    sstIndex->addSST("sst1.sst", 100, 200);
    sstIndex->addSST("sst2.sst", 201, 300);

    long long result = sstIndex->Search(250);
    EXPECT_EQ(result, 250);  // Key 250 should be found in sst2.sst

    deleteSSTFile(sst1);
    deleteSSTFile(sst2);
    delete sstIndex;
}

// Test when key is in the oldest SST file
TEST(SSTIndexTest, SearchInOldestSST) {
    SSTIndex* sstIndex = new SSTIndex();
    sstIndex->set_path(fs::current_path());

    fs::path sst1 = "sst1.sst";
    fs::path sst2 = "sst2.sst";

    createSSTFile(sst1, {{100, 100}, {150, 150}, {199, 200}});
    createSSTFile(sst2, {{201, 201}, {250, 250}, {300, 300}});

    sstIndex->addSST("sst1.sst", 100, 200);
    sstIndex->addSST("sst2.sst", 201, 300);

    long long result = sstIndex->Search(150);
    EXPECT_EQ(result, 150);  // Key 150 should be found in sst1.sst

    deleteSSTFile(sst1);
    deleteSSTFile(sst2);
    delete sstIndex;
}

// Test when key is not in any SST file
TEST(SSTIndexTest, KeyNotInAnySST) {
    SSTIndex* sstIndex = new SSTIndex();
    sstIndex->set_path(fs::current_path());

    fs::path sst1 = "sst1.sst";
    fs::path sst2 = "sst2.sst";

    createSSTFile(sst1, {{100, 100}, {150, 150}});
    createSSTFile(sst2, {{201, 201}, {250, 250}, {300, 300}});

    sstIndex->addSST("sst1.sst", 100, 200);
    sstIndex->addSST("sst2.sst", 201, 300);

    long long result = sstIndex->Search(400);
    EXPECT_EQ(result, -1);  // Key 400 is not in any SST, should return -1

    deleteSSTFile(sst1);
    deleteSSTFile(sst2);
    delete sstIndex;
}

// Test with multiple SST files, key in the middle file
TEST(SSTIndexTest, KeyInMiddleSST) {
    SSTIndex* sstIndex = new SSTIndex();
    sstIndex->set_path(fs::current_path());

    fs::path sst1 = "sst1.sst";
    fs::path sst2 = "sst2.sst";
    fs::path sst3 = "sst3.sst";

    createSSTFile(sst1, {{100, 100}, {150, 150}});
    createSSTFile(sst2, {{201, 201}, {250, 250}, {300, 300}});
    createSSTFile(sst3, {{301, 301}, {350, 350}, {400, 400}});

    sstIndex->addSST("sst1.sst", 100, 200);
    sstIndex->addSST("sst2.sst", 201, 300);
    sstIndex->addSST("sst3.sst", 301, 400);

    long long result = sstIndex->Search(250);
    EXPECT_EQ(result, 250);  // Key 250 should be found in sst2.sst

    deleteSSTFile(sst1);
    deleteSSTFile(sst2);
    deleteSSTFile(sst3);
    delete sstIndex;
}

// Test when key is exactly the smallest key in the youngest SST
TEST(SSTIndexTest, KeyIsSmallestInYoungestSST) {
    SSTIndex* sstIndex = new SSTIndex();
    sstIndex->set_path(fs::current_path());

    fs::path sst1 = "sst1.sst";
    fs::path sst2 = "sst2.sst";

    createSSTFile(sst1, {{100, 100}, {150, 150}});
    createSSTFile(sst2, {{201, 202}, {250, 250}, {300, 300}});

    sstIndex->addSST("sst1.sst", 100, 200);
    sstIndex->addSST("sst2.sst", 201, 300);

    long long result = sstIndex->Search(201);
    EXPECT_EQ(result, 202);  // Key 201 should be found in sst2.sst

    deleteSSTFile(sst1);
    deleteSSTFile(sst2);
    delete sstIndex;
}

// Test when key is exactly the largest key in the oldest SST
TEST(SSTIndexTest, KeyIsLargestInOldestSST) {
    SSTIndex* sstIndex = new SSTIndex();
    sstIndex->set_path(fs::current_path());

    fs::path sst1 = "sst1.sst";
    fs::path sst2 = "sst2.sst";

    createSSTFile(sst1, {{100, 100}, {150, 150}, {200, 200}});
    createSSTFile(sst2, {{200, 211}, {250, 250}, {300, 300}});

    sstIndex->addSST("sst1.sst", 100, 200);
    sstIndex->addSST("sst2.sst", 200, 300);

    long long result = sstIndex->Search(200);
    EXPECT_EQ(result, 211);  // Key 200 should be found in sst1.sst

    deleteSSTFile(sst1);
    deleteSSTFile(sst2);
    delete sstIndex;
}

// Test with a key that is in a file but near the boundary of another
TEST(SSTIndexTest, KeyNearBoundary) {
    SSTIndex* sstIndex = new SSTIndex();
    sstIndex->set_path(fs::current_path());

    fs::path sst1 = "sst1.sst";
    fs::path sst2 = "sst2.sst";

    createSSTFile(sst1, {{100, 100}, {150, 150}});
    createSSTFile(sst2, {{201, 201}, {250, 250}, {300, 300}});

    sstIndex->addSST("sst1.sst", 100, 200);
    sstIndex->addSST("sst2.sst", 201, 300);

    long long result = sstIndex->Search(300);
    EXPECT_EQ(result, 300);  // Key 300 should be found in sst2.sst

    deleteSSTFile(sst1);
    deleteSSTFile(sst2);
    delete sstIndex;
}

// Test with empty SSTIndex
TEST(SSTIndexTest, EmptySSTIndex) {
    SSTIndex* sstIndex = new SSTIndex();
    long long result = sstIndex->Search(12345);
    EXPECT_EQ(result, -1);  // Should return -1 since index is empty
    delete sstIndex;
}

// large test case with 1e3 lines in the file
TEST(SSTIndexTest, LargeAmountOfDataWithMultipleSearches) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    // Create a large SST file with 1,000 key-value pairs
    std::ofstream outfile(fs::path("test_db") / "large.sst", std::ios::binary);
    if (!outfile.is_open()) {
        throw std::runtime_error("Failed to open SST file for writing");
    }

    for (long long i = 1; i <= 1000; ++i) {
        outfile.write(reinterpret_cast<const char*>(&i), sizeof(long long));
        outfile.write(",", 1);
        long long value = i * 10;
        outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
        outfile.write("\n", 1);
    }
    outfile.close();

    // Perform 1000 searches
    for (int i = 0; i < 1000; ++i) {
        // Randomly generate a key within the range
        // long long key_to_search = rand() % 1000 + 1;
        long long key_to_search = i + 1;
        long long expected_value = key_to_search * 10;
        long long value = index->SearchInSST("large.sst", key_to_search);
        EXPECT_EQ(value, expected_value);
    }

    // Edge case 1: Search for the smallest key
    EXPECT_EQ(index->SearchInSST("large.sst", 1), 10);

    // Edge case 2: Search for the largest key
    EXPECT_EQ(index->SearchInSST("large.sst", 1000), 10000);

    // Edge case 3: Search for a non-existent key (lower than the smallest)
    EXPECT_EQ(index->SearchInSST("large.sst", 0), -1);

    // Edge case 4: Search for a non-existent key (higher than the largest)
    EXPECT_EQ(index->SearchInSST("large.sst", 1001), -1);

    // Edge case 5: Search for a key in the middle
    EXPECT_EQ(index->SearchInSST("large.sst", 500), 5000);

    delete index;
    fs::remove_all("test_db");
}


/*
 * generate 100 sst files with each one contains 1000 kv pairs
 */
TEST(SSTIndexTest, SearchAcrossMultipleSSTFiles) {
    SSTIndex* index = new SSTIndex();
    index->set_path("test_db");

    const int num_files = 100;      // Number of SST files
    const int pairs_per_file = 1000; // Key-value pairs per SST file
    const long long start_key = 1;  // Start key for the first SST file

    // Create 100 SST files with 1,000 key-value pairs each
    for (int file_num = 0; file_num < num_files; ++file_num) {
        std::string filename = "sst_" + std::to_string(file_num) + ".sst";
        std::ofstream outfile(fs::path("test_db") / filename, std::ios::binary);
        if (!outfile.is_open()) {
            throw std::runtime_error("Failed to open SST file for writing");
        }

        long long base_key = start_key + file_num * pairs_per_file;
        for (long long i = 0; i < pairs_per_file; ++i) {
            long long key = base_key + i;
            long long value = key * 10;
            outfile.write(reinterpret_cast<const char*>(&key), sizeof(long long));
            outfile.write(",", 1);
            outfile.write(reinterpret_cast<const char*>(&value), sizeof(long long));
            outfile.write("\n", 1);
        }
        outfile.close();

        // Add this SST file to the index
        index->addSST(filename, base_key, base_key + pairs_per_file - 1);
    }

    // Perform searches across all files
    for (int file_num = 0; file_num < num_files; ++file_num) {
        long long base_key = start_key + file_num * pairs_per_file;
        for (long long i = 0; i < pairs_per_file; ++i) {
            long long key_to_search = base_key + i;
            long long expected_value = key_to_search * 10;
            long long value = index->Search(key_to_search);
            EXPECT_EQ(value, expected_value);
        }
    }

    // Edge case 1: Search for the smallest key
    EXPECT_EQ(index->Search(start_key), start_key * 10);

    // Edge case 2: Search for the largest key
    EXPECT_EQ(index->Search(start_key + num_files * pairs_per_file - 1),
              (start_key + num_files * pairs_per_file - 1) * 10);

    // Edge case 3: Search for a non-existent key (lower than the smallest)
    EXPECT_EQ(index->Search(start_key - 1), -1);

    // Edge case 4: Search for a non-existent key (higher than the largest)
    EXPECT_EQ(index->Search(start_key + num_files * pairs_per_file), -1);

    // Edge case 5: Search for a key in the middle
    long long middle_key = start_key + (num_files * pairs_per_file) / 2;
    EXPECT_EQ(index->Search(middle_key), middle_key * 10);

    delete index;
    fs::remove_all("test_db");
}