//
// Created by Damian Li on 2024-08-29.
//
#include <gtest/gtest.h>
#include "SSTIndex.h"
#include "Memtable.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <deque>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <string>
namespace fs = std::filesystem;

/*
 * SSTIndex Unit Tests
 */

/*
 * void SSTIndex::addSST(const string& filename, KeyValue smallest_key, KeyValue largest_key);
 */
TEST(SSTIndexTest, AddSingleSST) {
    // Create an SSTIndex instance
    SSTIndex sstIndex;

    // Define a filename and key-value pairs for smallest and largest keys
    string filename = "sst_001.sst";
    KeyValue smallestKey(1, "one");
    KeyValue largestKey(100, "hundred");

    // Add the SST to the index
    sstIndex.addSST(filename, smallestKey, largestKey);

    // Verify the SST was added correctly
    deque<SSTInfo*> index = sstIndex.getSSTsIndex();
    ASSERT_EQ(index.size(), 1);
    EXPECT_EQ(index.front()->filename, filename);
    EXPECT_EQ(std::get<int>(index.front()->smallest_key.getKey()), 1);
    EXPECT_EQ(std::get<string>(index.front()->largest_key.getValue()), "hundred");
}


TEST(SSTIndexTest, AddMultipleSSTs) {
    // Create an SSTIndex instance
    SSTIndex sstIndex;

    // Add multiple SST files to the index
    sstIndex.addSST("sst_001.sst", KeyValue(1, "one"), KeyValue(100, "hundred"));
    sstIndex.addSST("sst_002.sst", KeyValue(101, "hundred and one"), KeyValue(200, "two hundred"));

    // Verify the SST files were added correctly
    deque<SSTInfo*> index = sstIndex.getSSTsIndex();
    ASSERT_EQ(index.size(), 2);

    // Check first SST
    EXPECT_EQ(index[0]->filename, "sst_001.sst");
    EXPECT_EQ(std::get<int>(index[0]->smallest_key.getKey()), 1);
    EXPECT_EQ(std::get<string>(index[0]->largest_key.getValue()), "hundred");

    // Check second SST
    EXPECT_EQ(index[1]->filename, "sst_002.sst");
    EXPECT_EQ(std::get<int>(index[1]->smallest_key.getKey()), 101);
    EXPECT_EQ(std::get<string>(index[1]->largest_key.getValue()), "two hundred");
}

TEST(SSTIndexTest, AddSSTUpdatesSize) {
    // Create an SSTIndex instance
    SSTIndex sstIndex;

    // Verify the index starts empty
    EXPECT_EQ(sstIndex.getSSTsIndex().size(), 0);

    // Add an SST file
    sstIndex.addSST("sst_001.sst", KeyValue(1, "one"), KeyValue(100, "hundred"));

    // Verify the index size increases
    EXPECT_EQ(sstIndex.getSSTsIndex().size(), 1);

    // Add another SST file
    sstIndex.addSST("sst_002.sst", KeyValue(101, "hundred and one"), KeyValue(200, "two hundred"));

    // Verify the index size increases again
    EXPECT_EQ(sstIndex.getSSTsIndex().size(), 2);
}

TEST(SSTIndexTest, AddSSTWithComplexKeyValues) {
    // Create an SSTIndex instance
    SSTIndex sstIndex;

    // Add SST with different types for smallest and largest keys
    sstIndex.addSST("sst_003.sst", KeyValue(1.23, "one point two three"), KeyValue('z', "zebra"));

    // Verify the SST was added with correct key-value pairs
    deque<SSTInfo*> index = sstIndex.getSSTsIndex();
    ASSERT_EQ(index.size(), 1);

    EXPECT_EQ(index.front()->filename, "sst_003.sst");
    EXPECT_EQ(std::get<double>(index.front()->smallest_key.getKey()), 1.23);
    EXPECT_EQ(std::get<string>(index.front()->largest_key.getValue()), "zebra");
}

/*
 *  // Retrieve all SSTs into index (e.g., when reopening the database)
 *  void getAllSSTs();  // updated with kv 2024-09-10
 *  // flush index info into "Index.sst"
 *  void flushToDisk(); // updated with kv 2024-09-10
 */
TEST(SSTIndexTest, BasicFlushAndRetrieve) {
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Add a single SST file
    sstIndex.addSST("sst_001.sst", KeyValue(1, "one"), KeyValue(100, "hundred"));

    // Flush to disk
    sstIndex.flushToDisk();

    // Clear the index and retrieve SSTs from disk
    sstIndex.getAllSSTs();

    // Verify that the index has been restored
    deque<SSTInfo*> index = sstIndex.getSSTsIndex();
    ASSERT_EQ(index.size(), 1);
    EXPECT_EQ(index.front()->filename, "sst_001.sst");
    EXPECT_EQ(std::get<int>(index.front()->smallest_key.getKey()), 1);
    EXPECT_EQ(std::get<string>(index.front()->largest_key.getValue()), "hundred");

    // Clean up
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, MultipleFilesFlushAndRetrieve) {
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Add multiple SST files
    sstIndex.addSST("sst_001.sst", KeyValue(1, "one"), KeyValue(100, "hundred"));
    sstIndex.addSST("sst_002.sst", KeyValue(101, "hundred and one"), KeyValue(200, "two hundred"));
    sstIndex.addSST("sst_003.sst", KeyValue(201, "two hundred and one"), KeyValue(300, "three hundred"));

    // Flush to disk
    sstIndex.flushToDisk();

    // Clear the index and retrieve SSTs from disk
    sstIndex.getAllSSTs();

    // Verify that the index has been restored
    deque<SSTInfo*> index = sstIndex.getSSTsIndex();
    ASSERT_EQ(index.size(), 3);
    EXPECT_EQ(index[0]->filename, "sst_001.sst");
    EXPECT_EQ(index[1]->filename, "sst_002.sst");
    EXPECT_EQ(index[2]->filename, "sst_003.sst");

    // Clean up
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, LargeNumberOfFilesFlushAndRetrieve) {
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Add 1000 SST files
    for (int i = 0; i < 1000; ++i) {
        sstIndex.addSST("sst_" + std::to_string(i) + ".sst", KeyValue(i, "smallest"), KeyValue(i + 100, "largest"));
    }

    // Flush to disk
    sstIndex.flushToDisk();

    // Clear the index and retrieve SSTs from disk
    sstIndex.getAllSSTs();

    // Verify that the index has been restored
    deque<SSTInfo*> index = sstIndex.getSSTsIndex();
    ASSERT_EQ(index.size(), 1000);
    EXPECT_EQ(index[0]->filename, "sst_0.sst");

    // Clean up
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, RetrieveWithoutFlush) {
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Add SST files without flushing to disk
    sstIndex.addSST("sst_001.sst", KeyValue(1, "one"), KeyValue(100, "hundred"));

    // Clear the index and attempt to retrieve SSTs from disk (nothing should be loaded)
    sstIndex.getAllSSTs();

    // Verify that the index is still empty
    deque<SSTInfo*> index = sstIndex.getSSTsIndex();
    ASSERT_EQ(index.size(), 1);

    // Clean up
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, FlushWithOneInstanceRetrieveWithAnother) {
    fs::path test_path = "test_db";

    // First instance of SSTIndex for flushing data to disk
    SSTIndex sstIndexFlush;
    sstIndexFlush.set_path(test_path);

    // Add 500 SST files to the first SSTIndex instance
    for (int i = 0; i < 500; ++i) {
        sstIndexFlush.addSST("sst_" + std::to_string(i) + ".sst", KeyValue(i, "smallest"), KeyValue(i + 10, "largest"));
    }

    // Flush to disk using the first instance
    sstIndexFlush.flushToDisk();

    // Second instance of SSTIndex for retrieving data from disk
    SSTIndex sstIndexRetrieve;
    sstIndexRetrieve.set_path(test_path);

    // Retrieve SST files using the second instance
    sstIndexRetrieve.getAllSSTs();
    sstIndexFlush.getAllSSTs();

    // Get the index from both instances
    deque<SSTInfo*> flushedIndex = sstIndexFlush.getSSTsIndex();
    deque<SSTInfo*> retrievedIndex = sstIndexRetrieve.getSSTsIndex();

    // Verify the size of the indices
    ASSERT_EQ(flushedIndex.size(), retrievedIndex.size());

    // Verify that all SST files and key-value pairs match between the two instances
    for (size_t i = 0; i < flushedIndex.size(); ++i) {
        EXPECT_EQ(flushedIndex[i]->filename, retrievedIndex[i]->filename);
        EXPECT_EQ(std::get<int>(flushedIndex[i]->smallest_key.getKey()), std::get<int>(retrievedIndex[i]->smallest_key.getKey()));
        EXPECT_EQ(std::get<int>(flushedIndex[i]->largest_key.getKey()), std::get<int>(retrievedIndex[i]->largest_key.getKey()));
        EXPECT_EQ(std::get<string>(flushedIndex[i]->smallest_key.getValue()), std::get<string>(retrievedIndex[i]->smallest_key.getValue()));
        EXPECT_EQ(std::get<string>(flushedIndex[i]->largest_key.getValue()), std::get<string>(retrievedIndex[i]->largest_key.getValue()));
    }

    // Clean up
    fs::remove_all(test_path);
}

/*
 * KeyValue SearchInSST(const string& filename, KeyValue _key);
 * KeyValue Search(KeyValue);
 */

TEST(SSTIndexTest, SearchInSingleSSTFile) {
    // Create a Memtable and insert key-value pairs
    Memtable* memtable = new Memtable(10); // small threshold
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs into the memtable
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));

    // Flush the Memtable to an SST file
    FlushSSTInfo info = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create an SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Search for an existing key in the SST file
    KeyValue result = sstIndex.SearchInSST(info.fileName, KeyValue(2, ""));
    EXPECT_FALSE(result.isEmpty());
    EXPECT_EQ(std::get<int>(result.getKey()), 2);
    EXPECT_EQ(std::get<string>(result.getValue()), "two");

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, SearchNonExistentKeyInSingleSSTFile) {
    // Create a Memtable and insert key-value pairs
    Memtable* memtable = new Memtable(10);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs into the memtable
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));

    // Flush the Memtable to an SST file
    FlushSSTInfo info = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create an SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Search for a non-existent key in the SST file
    KeyValue result = sstIndex.SearchInSST(info.fileName, KeyValue(4, ""));
    EXPECT_TRUE(result.isEmpty());

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, SearchInMultipleSSTFiles) {
    // Create a Memtable and insert key-value pairs
    Memtable* memtable = new Memtable(10);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs into the memtable and flush them
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    FlushSSTInfo info1 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    memtable->put(KeyValue(3, "three"));
    memtable->put(KeyValue(4, "four"));
    FlushSSTInfo info2 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create an SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Search for a key in the second SST file
    KeyValue result = sstIndex.SearchInSST(info2.fileName, KeyValue(3, ""));
    EXPECT_FALSE(result.isEmpty());
    EXPECT_EQ(std::get<int>(result.getKey()), 3);
    EXPECT_EQ(std::get<string>(result.getValue()), "three");

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, SearchInLargeNumberOfSSTFiles) {
    // Create a Memtable and insert large number of key-value pairs
    Memtable* memtable = new Memtable(100);
    memtable->set_path(fs::path("test_db"));

    // Insert 1000 key-value pairs and flush them to SST files
    for (int i = 0; i < 1000; ++i) {
        memtable->put(KeyValue(i, "value_" + std::to_string(i)));
    }

    // Create an SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));
    // sstIndex.getAllSSTs();

    // Search for a key in the middle of the range
    KeyValue result = sstIndex.SearchInSST("sst_5.sst", KeyValue(500, ""));
    EXPECT_FALSE(result.isEmpty());
    EXPECT_EQ(std::get<int>(result.getKey()), 500);
    EXPECT_EQ(std::get<string>(result.getValue()), "value_500");

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

// Search
TEST(SSTIndexTest, SearchForExistingKey) {
    // Create a Memtable with a threshold of 3 (flushes after 3 key-value pairs)
    Memtable* memtable = new Memtable(3);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs (memtable will automatically flush after reaching threshold)
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));  // Flushing occurs here

    // Manually add the SST information to SSTIndex after the flush
    FlushSSTInfo info = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create an SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Manually add the flushed SST file info into SSTIndex
    sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);

    // Search for an existing key in the SST file
    KeyValue result = sstIndex.Search(KeyValue(3, ""));

    // Verify that the key-value pair was found
    EXPECT_FALSE(result.isEmpty());
    EXPECT_EQ(std::get<int>(result.getKey()), 3);
    EXPECT_EQ(std::get<string>(result.getValue()), "three");

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, SearchForNonExistentKey) {
    // Create a Memtable with a threshold of 3
    Memtable* memtable = new Memtable(3);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs (flush occurs automatically)
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));

    // Manually add the SST information to SSTIndex after the flush
    FlushSSTInfo info = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create an SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Manually add the flushed SST file info into SSTIndex
    sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);

    // Search for a non-existent key in the SST file
    KeyValue result = sstIndex.Search(KeyValue(10, ""));

    // Verify that the key-value pair was not found (empty result)
    EXPECT_TRUE(result.isEmpty());

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, SearchAcrossMultipleSSTFiles) {
    // Create a Memtable with a threshold of 3
    Memtable* memtable = new Memtable(3);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs to trigger multiple flushes
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));  // First flush occurs here
    FlushSSTInfo info1 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    memtable->put(KeyValue(4, "four"));
    memtable->put(KeyValue(5, "five"));
    memtable->put(KeyValue(6, "six"));    // Second flush occurs here
    FlushSSTInfo info2 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    memtable->put(KeyValue(7, "seven"));

    // Create an SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Manually add the flushed SST files into SSTIndex
    sstIndex.addSST(info1.fileName, info1.smallest_key, info1.largest_key);
    sstIndex.addSST(info2.fileName, info2.smallest_key, info2.largest_key);

    // Search for keys across multiple SST files
    KeyValue result1 = sstIndex.Search(KeyValue(2, ""));
    KeyValue result2 = sstIndex.Search(KeyValue(6, ""));

    // Verify both keys were found
    EXPECT_FALSE(result1.isEmpty());
    EXPECT_EQ(std::get<int>(result1.getKey()), 2);
    EXPECT_EQ(std::get<string>(result1.getValue()), "two");

    EXPECT_FALSE(result2.isEmpty());
    EXPECT_EQ(std::get<int>(result2.getKey()), 6);
    EXPECT_EQ(std::get<string>(result2.getValue()), "six");

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, SearchIn1e3OfSSTFiles) {
    // Create a Memtable with a threshold of 100 (flushes after 100 key-value pairs)
    Memtable* memtable = new Memtable(100);
    memtable->set_path(fs::path("test_db"));

    // Create SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Insert 1000 key-value pairs (this will trigger multiple flushes)
    for (int i = 0; i < 1e3; ++i) {
        KeyValue kv(i, "value_" + std::to_string(i));

        // After every flush (each 1,000 key-value pairs), manually add the SST info to SSTIndex
        if (i % 100 == 0 && i != 0) {
            FlushSSTInfo info = memtable->put(kv);
            sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);
            continue;
        }
        memtable->put(kv);
    }

    // Search for a key in the middle of the range
    KeyValue result = sstIndex.Search(KeyValue(500, ""));

    // Verify that the key-value pair was found
    EXPECT_FALSE(result.isEmpty());
    EXPECT_EQ(std::get<int>(result.getKey()), 500);
    EXPECT_EQ(std::get<string>(result.getValue()), "value_500");

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

// Performance issue
TEST(SSTIndexTest, Search1e5KeyValuePairs) {
    const int numKVPairs = 1e3;    // Total number of key-value pairs (1e5)
    const int memtableSize = 1e2;    // Memtable threshold (1e3)

    // Create a Memtable with a threshold of 1,000 (flushes after 1,000 key-value pairs)
    Memtable* memtable = new Memtable(memtableSize);
    memtable->set_path(fs::path("test_db"));

    // Create an SSTIndex instance to store the SST information
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Insert 100,000 key-value pairs into the Memtable (this will trigger multiple flushes)
    for (int i = 0; i < numKVPairs; ++i) {
        KeyValue kv(i, "value_" + std::to_string(i));

        // After every flush (each 1,000 key-value pairs), manually add the SST info to SSTIndex
        if (i % memtableSize == 0 && i != 0) {
            FlushSSTInfo info = memtable->put(kv);
            sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);
            continue;
        }
        memtable->put(kv);
    }

    // After all insertions, there might still be data left in the Memtable, flush it
    FlushSSTInfo finalInfo = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());
    sstIndex.addSST(finalInfo.fileName, finalInfo.smallest_key, finalInfo.largest_key);

    // Search for every inserted key-value pair and verify it exists
    for (int i = 1; i < numKVPairs; ++i) {
        // cout << "i = " << i << endl;
        KeyValue result = sstIndex.Search(KeyValue(i, ""));
        EXPECT_FALSE(result.isEmpty());
        EXPECT_EQ(std::get<int>(result.getKey()), i);
        EXPECT_EQ(std::get<string>(result.getValue()), "value_" + std::to_string(i));
    }

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}


/*
 * Scan in SST
 */

TEST(SSTIndexTest, ScanRangeOfKeyValuePairs) {
    // Create a Memtable with a small threshold to trigger flushes quickly
    Memtable* memtable = new Memtable(3);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs into the memtable
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));  // Flushing occurs here

    // Flush the Memtable to an SST file
    FlushSSTInfo info = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Manually add the flushed SST file to the SSTIndex
    sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);

    // Perform a scan for the range of key-value pairs
    set<KeyValue> resultSet;
    sstIndex.ScanInSST(KeyValue(1, ""), KeyValue(3, ""), info.fileName, resultSet);

    // Verify the scanned result
    ASSERT_EQ(resultSet.size(), 3);
    EXPECT_EQ(std::get<int>(resultSet.begin()->getKey()), 1);
    EXPECT_EQ(std::get<string>(resultSet.rbegin()->getValue()), "three");

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, ScanForNonExistentRange) {
    // Create a Memtable with a small threshold to trigger flushes quickly
    Memtable* memtable = new Memtable(3);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs into the memtable
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));  // Flushing occurs here

    // Flush the Memtable to an SST file
    FlushSSTInfo info = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Manually add the flushed SST file to the SSTIndex
    sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);

    // Perform a scan for a range with no matching key-value pairs
    set<KeyValue> resultSet;
    sstIndex.ScanInSST(KeyValue(10, ""), KeyValue(20, ""), info.fileName, resultSet);

    // Verify that no key-value pairs were found
    EXPECT_EQ(resultSet.size(), 0);

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, ScanLargeNumberOfKeyValuePairs) {
    // Create a Memtable with a large threshold
    Memtable* memtable = new Memtable(1000);
    memtable->set_path(fs::path("test_db"));

    // Insert 1000 key-value pairs into the memtable
    for (int i = 0; i < 1000; ++i) {
        memtable->put(KeyValue(i, "value_" + std::to_string(i)));
    }

    // Flush the Memtable to an SST file
    FlushSSTInfo info = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create SSTIndex instance
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Manually add the flushed SST file to the SSTIndex
    sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);

    // Perform a scan for a large range of key-value pairs
    set<KeyValue> resultSet;
    sstIndex.ScanInSST(KeyValue(100, ""), KeyValue(900, ""), info.fileName, resultSet);

    // // Debug: Print the keys in the resultSet
    // for (const auto& kv : resultSet) {
    //     std::cout << "Key: " << std::get<int>(kv.getKey()) << ", Value: " << std::get<string>(kv.getValue()) << "\n";
    // }

    // Verify that the correct number of key-value pairs were found
    EXPECT_EQ(resultSet.size(), 801);

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

/*
 * Scan
 */
TEST(SSTIndexTest, BasicScanRangeAcrossSSTFiles) {
    // Create a Memtable with a small threshold to trigger flushes quickly
    Memtable* memtable = new Memtable(3);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs into the memtable
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));  // First flush
    FlushSSTInfo info1 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    memtable->put(KeyValue(4, "four"));
    memtable->put(KeyValue(5, "five"));
    memtable->put(KeyValue(6, "six"));    // Second flush
    FlushSSTInfo info2 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create SSTIndex instance and manually add SST files to the index
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));
    sstIndex.addSST(info1.fileName, info1.smallest_key, info1.largest_key);
    sstIndex.addSST(info2.fileName, info2.smallest_key, info2.largest_key);

    // Perform a scan across SST files for a range of key-value pairs
    set<KeyValue> resultSet;
    sstIndex.Scan(KeyValue(2, ""), KeyValue(5, ""), resultSet);

    // Verify the scanned result
    ASSERT_EQ(resultSet.size(), 4);
    EXPECT_EQ(std::get<int>(resultSet.begin()->getKey()), 2);
    EXPECT_EQ(std::get<int>(resultSet.rbegin()->getKey()), 5);

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, ScanForNonExistentRangeAcrossSSTFiles) {
    // Create a Memtable with a small threshold
    Memtable* memtable = new Memtable(3);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs and flush the memtable
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));  // First flush
    FlushSSTInfo info1 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    memtable->put(KeyValue(4, "four"));
    memtable->put(KeyValue(5, "five"));
    memtable->put(KeyValue(6, "six"));    // Second flush
    FlushSSTInfo info2 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create SSTIndex instance and manually add SST files to the index
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));
    sstIndex.addSST(info1.fileName, info1.smallest_key, info1.largest_key);
    sstIndex.addSST(info2.fileName, info2.smallest_key, info2.largest_key);

    // Perform a scan for a non-existent range
    set<KeyValue> resultSet;
    sstIndex.Scan(KeyValue(10, ""), KeyValue(20, ""), resultSet);

    // Verify that no key-value pairs were found
    EXPECT_EQ(resultSet.size(), 0);

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, ScanForSingleKeyValuePairAcrossSSTFiles) {
    // Create a Memtable with a small threshold
    Memtable* memtable = new Memtable(3);
    memtable->set_path(fs::path("test_db"));

    // Insert key-value pairs and flush the memtable
    memtable->put(KeyValue(1, "one"));
    memtable->put(KeyValue(2, "two"));
    memtable->put(KeyValue(3, "three"));  // First flush
    FlushSSTInfo info1 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    memtable->put(KeyValue(4, "four"));
    memtable->put(KeyValue(5, "five"));
    memtable->put(KeyValue(6, "six"));    // Second flush
    FlushSSTInfo info2 = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());

    // Create SSTIndex instance and manually add SST files to the index
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));
    sstIndex.addSST(info1.fileName, info1.smallest_key, info1.largest_key);
    sstIndex.addSST(info2.fileName, info2.smallest_key, info2.largest_key);

    // Perform a scan for a single key-value pair
    set<KeyValue> resultSet;
    sstIndex.Scan(KeyValue(4, ""), KeyValue(4, ""), resultSet);

    // Verify that only one key-value pair was found
    ASSERT_EQ(resultSet.size(), 1);
    EXPECT_EQ(std::get<int>(resultSet.begin()->getKey()), 4);
    EXPECT_EQ(std::get<string>(resultSet.begin()->getValue()), "four");

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}

TEST(SSTIndexTest, LargeScaleScanAcrossSSTFiles) {
    // Create a Memtable with a large threshold
    Memtable* memtable = new Memtable(100);
    memtable->set_path(fs::path("test_db"));

    // Create SSTIndex instance and manually add SST files to the index
    SSTIndex sstIndex;
    sstIndex.set_path(fs::path("test_db"));

    // Insert 1000 key-value pairs and flush the memtable
    for (int i = 0; i < 1000; ++i) {
        KeyValue kv(i, "value_" + std::to_string(i));

        // After every flush (each 1,000 key-value pairs), manually add the SST info to SSTIndex
        if (i % 100 == 0 && i != 0) {
            FlushSSTInfo info = memtable->put(kv);
            sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);
            continue;
        }
        memtable->put(kv);
    }

    FlushSSTInfo info = memtable->file_manager.flushToDisk(memtable->getTree()->inOrderFlushToSst());
    sstIndex.addSST(info.fileName, info.smallest_key, info.largest_key);

    // Perform a scan for a large range of key-value pairs
    set<KeyValue> resultSet;
    sstIndex.Scan(KeyValue(100, ""), KeyValue(900, ""), resultSet);

    // Verify that the correct number of key-value pairs were found
    EXPECT_EQ(resultSet.size(), 801);

    // Clean up
    delete memtable;
    fs::remove_all("test_db");
}
