//
// Created by Damian Li on 2024-08-29.
//
#include <vector>
#include <string>
#include <deque>
#include <unordered_map>
#ifndef SSTINDEX_H
#define SSTINDEX_H
#include "FileManager.h"
#include "KeyValue.h"
#include <filesystem> // C++17 lib

namespace fs = std::filesystem;
using namespace std;

struct SSTInfo {
  std::string filename;
  KeyValue smallest_key;
  KeyValue largest_key;
};


/*
 * Index.sst SSTIndexHeader Structure
 * void SSTHeader::serialize(ofstream&)
 * ==============================================================================
 * num_files | header_checksum |
 * ==============================================================================
 */
struct SSTIndexHeader {
    uint32_t num_files;
    uint32_t header_checksum;
    uint32_t calculateChecksum() const;
    // Serialize the header to a binary file
    void serialize(std::ofstream& file) const;
    // Deserialize the header from a binary file
    static SSTIndexHeader deserialize(std::ifstream& file);
};


/*
 * Index.sst SerializedIndexSSTInfo Structure
 *
 * ===================================================================================
 * filename_len | (string) filename |
 * ===================================================================================
 * ---->SerializedKeyValue::smallest_key
 *      ==============================================================================
 *      String Type:
 *      kv_checksum | keyType | str_len | (string) keyValue | valueType | valueValue |
 *      ==============================================================================
 *      Other Type:
 *      kv_checksum | keyType | (T) keyValue | valueType | valueValue |
 *      ==============================================================================
* ---->SerializedKeyValue::largest_key
 *      ==============================================================================
 *      String Type:
 *      kv_checksum | keyType | str_len | (string) keyValue | valueType | valueValue |
 *      ==============================================================================
 *      Other Type:
 *      kv_checksum | keyType | (T) keyValue | valueType | valueValue |
 *      ==============================================================================
 * ===================================================================================
 */
struct SerializedIndexSSTInfo {
    string filename;
    SerializedKeyValue smallest_key;
    SerializedKeyValue largest_key;
    // serialize filename string
    void serializeFileName(ofstream& file) const;
    void serialize(ofstream& file) const;
    static SerializedIndexSSTInfo deserialize(ifstream& file);
};

class SSTIndex {
  public:
  SSTIndex();
  ~SSTIndex(){};
  /*
   * IO Operations
   */
  // Retrieve all SSTs into index (e.g., when reopening the database)
  void getAllSSTs();  // updated with kv 2024-09-10
  // flush index info into "Index.sst"
  void flushToDisk(); // updated with kv 2024-09-10
  // Add a new SST to the index
  void addSST(const string& filename, KeyValue smallest_key, KeyValue largest_key); // updated with kv 2024-09-10
  // get index
  deque<SSTInfo*> getSSTsIndex() {return index;};
  /*
   * Search Operations
   */
  // SST file search by using RedBlackTree* FileManager::loadFromDisk(const std::string& sst_filename);
  KeyValue SearchInSST(const string& filename, KeyValue _key); // updated with kv 2024-09-10
  // Search in all SST files
  KeyValue Search(KeyValue);
  /*
   * Scan Operations
   */
  // scan in all SST files [from YOUNGEST to OLDEST] [Note: currently I'm using set<KeyValue>]
  void Scan(KeyValue smallestKey, KeyValue largestKey, set<KeyValue>&);
  // scan kv-pairs inside sst file
  void ScanInSST(KeyValue smallestKey, KeyValue largestKey, const string& filename, set<KeyValue>&);
  // helper function
  void set_path(fs::path);

private:
  deque<SSTInfo*> index;
  fs::path path;
  FileManager fileManager;

};

#endif //SSTINDEX_H
