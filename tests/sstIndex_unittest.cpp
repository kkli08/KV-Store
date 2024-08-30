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

namespace fs = std::filesystem;


#include <gtest/gtest.h>
#include "SSTIndex.h"
#include <filesystem>

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

TEST(SSTIndexTest, FlushIndexToDisk) {
    SSTIndex* index = new SSTIndex();
    index->set_path(fs::path("test_db"));

    index->addSST("sst_1.sst", 10, 100);
    index->addSST("sst_2.sst", 200, 300);

    // Flush to disk
    index->flushToDisk();

    // // Verify the file exists and has correct content
    string filename = "Index.sst";
    std::ifstream infile(fs::path("test_db") / filename);
    ASSERT_TRUE(infile.is_open());

    std::string line;
    std::getline(infile, line);
    EXPECT_EQ(line, "sst_1.sst 10 100");
    std::getline(infile, line);
    EXPECT_EQ(line, "sst_2.sst 200 300");

    // infile.close();
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
