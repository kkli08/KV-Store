//
// Created by Damian Li on 2024-08-29.
//

#include "SSTIndex.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include "FileManager.h"
using namespace std;

#define RECORDE_SIZE 18

/*
 * Index.sst SSTIndexHeader Methods
 *
 */
uint32_t SSTIndexHeader::calculateChecksum() const {
  // Simple checksum: sum up the sizes of the header fields
  return sizeof(num_files) + sizeof(header_checksum);
}

void SSTIndexHeader::serialize(std::ofstream& file) const {
  file.write(reinterpret_cast<const char*>(&num_files), sizeof(num_files));
  file.write(reinterpret_cast<const char*>(&header_checksum), sizeof(header_checksum));
}

SSTIndexHeader SSTIndexHeader::deserialize(std::ifstream& file) {
  SSTIndexHeader header;
  file.read(reinterpret_cast<char*>(&header.num_files), sizeof(header.num_files));
  file.read(reinterpret_cast<char*>(&header.header_checksum), sizeof(header.header_checksum));
  return header;
}

/*
 * Index.sst SerializedIndexSSTInfo Methods
 *
 */
void SerializedIndexSSTInfo::serializeFileName(std::ofstream& file) const {
  uint32_t filename_len = filename.size();

  // Write the length of the filename
  file.write(reinterpret_cast<const char*>(&filename_len), sizeof(filename_len));

  // Write the actual filename string
  file.write(filename.data(), filename_len);
}

void SerializedIndexSSTInfo::serialize(std::ofstream& file) const {
  // Serialize the filename
  serializeFileName(file);

  // Serialize the smallest key
  smallest_key.serialize(file);

  // Serialize the largest key
  largest_key.serialize(file);
}

SerializedIndexSSTInfo SerializedIndexSSTInfo::deserialize(std::ifstream& file) {
  SerializedIndexSSTInfo sstInfo;

  // Deserialize the filename
  uint32_t filename_len;
  file.read(reinterpret_cast<char*>(&filename_len), sizeof(filename_len));

  sstInfo.filename.resize(filename_len);
  file.read(&sstInfo.filename[0], filename_len);  // Read the actual filename

  // Deserialize the smallest key
  sstInfo.smallest_key = SerializedKeyValue::deserialize(file);

  // Deserialize the largest key
  sstInfo.largest_key = SerializedKeyValue::deserialize(file);

  return sstInfo;
}


SSTIndex::SSTIndex() {
  path = fs::path("defaultDB");
  if (!fs::exists(path)) {
    fs::create_directories(path);  // Ensure the directory exists
  }
}

// Retrieve all SSTs into index (e.g., when reopening the database)
void SSTIndex::getAllSSTs() {
  // Open the file "Index.sst" in binary mode
  std::ifstream infile(path / "Index.sst", std::ios::binary);

  if (!infile.is_open()) {
    // If the file doesn't exist, just return
    return;
  }

  // Check if the file is empty
  infile.seekg(0, std::ios::end);
  if (infile.tellg() == 0) {
    // If the file is empty, close it and return
    infile.close();
    return;
  }

  // Reset the file pointer to the beginning
  infile.seekg(0, std::ios::beg);

  // Clear the current index to avoid duplication
  index.clear();

  // Step 1: Deserialize the header
  SSTIndexHeader header = SSTIndexHeader::deserialize(infile);

  // Step 2: Loop through and deserialize each SST entry
  for (uint32_t i = 0; i < header.num_files; ++i) {
    // Deserialize the individual SerializedIndexSSTInfo
    SerializedIndexSSTInfo sstInfo = SerializedIndexSSTInfo::deserialize(infile);

    // Convert SerializedIndexSSTInfo into SSTInfo and add it to the index
    addSST(sstInfo.filename, sstInfo.smallest_key.kv, sstInfo.largest_key.kv);
  }

  // Close the input file
  infile.close();
}



// flush index info into "Index.sst"
void SSTIndex::flushToDisk() {
  // Attempt to open (or create) the file "Index.sst"
  std::ofstream outfile(path / "Index.sst", std::ios::binary | std::ios::trunc);

  if (!outfile.is_open()) {
    throw std::runtime_error("SSTIndex::flushToDisk() >>>> Failed to open Index.sst for writing.");
  }

  // Step 1: Write the SSTIndexHeader
  SSTIndexHeader header;
  header.num_files = index.size();
  header.header_checksum = header.calculateChecksum();
  header.serialize(outfile);

  // Step 2: Serialize and write each SSTInfo in the deque to the file
  for (const auto& sst_info : index) {
    SerializedIndexSSTInfo serialized_info;
    serialized_info.filename = sst_info->filename;
    serialized_info.smallest_key = SerializedKeyValue{sst_info->smallest_key, serialized_info.smallest_key.calculateChecksum()};
    serialized_info.largest_key = SerializedKeyValue{sst_info->largest_key, serialized_info.largest_key.calculateChecksum()};

    serialized_info.serialize(outfile);  // Serialize and write to the file
  }

  // Step 3: Close the file
  outfile.close();
  if (!outfile) {
    throw std::runtime_error("SSTIndex::flushToDisk() >>>> Failed to write to Index.sst.");
  }
  // clear index
  index.clear();
}


// Add a new SST to the index
void SSTIndex::addSST(const string& filename, KeyValue smallest_key, KeyValue largest_key){
  SSTInfo* info = new SSTInfo{filename, smallest_key, largest_key};
  index.push_back(info);

  // // Debug Purpose :-D
  // if (!index.empty()) {
  //   SSTInfo* info = index.back();
  //   cout << "\n-->Inside SSTIndex::addSST()" << endl;
  //   std::cout << info->filename << endl << "Smallest KeyValue:" << endl;
  //   info->smallest_key.printKeyValue();
  //   cout << "\nLargest KeyValue:" << endl;
  //   info->largest_key.printKeyValue();
  //
  // } else {
  //   std::cout << "The deque is empty, no elements to print." << std::endl;
  // }
}

void SSTIndex::set_path(fs::path _path) {
  // Check if the directory exists
  if (!fs::exists(_path)) {
    // If the directory does not exist, create it
    fs::create_directories(_path);
  }
  fileManager.setDirectory(_path);
  path = _path;
}


// SST file search by using RedBlackTree* FileManager::loadFromDisk(const std::string& sst_filename);
KeyValue SSTIndex::SearchInSST(const string& filename, KeyValue _key) {
  // Append the directory path to the filename
  fs::path fullFilePath = path / filename;

  // Check if the SST file exists
  if (!fs::exists(fullFilePath)) {
    throw std::runtime_error("SSTIndex::SearchInSST() >>>> SST file does not exist: " + fullFilePath.string());
  }

  // Use the FileManager to load the SST file into a RedBlackTree
  RedBlackTree* tree = fileManager.loadFromDisk(filename);

  // Check if the tree is null or empty (in case the file couldn't be loaded)
  if (!tree) {
    throw std::runtime_error("SSTIndex::SearchInSST() >>>> Failed to load SST file: " + fullFilePath.string());
  }

  // Search for the key in the loaded RedBlackTree
  KeyValue result = tree->getValue(_key);

  // Clean up: delete the tree once we're done
  delete tree;

  // Return the result of the search
  return result;
}




// search value for key
KeyValue SSTIndex::Search(KeyValue _key) {
  // Traverse the deque from the youngest (back) to the oldest (front)
  for (auto it = index.rbegin(); it != index.rend(); ++it) {
    SSTInfo* sst_info = *it;

    // Check if the range overlaps with the key range in this SST file
    if (sst_info->largest_key < _key || sst_info->smallest_key > _key) {
      // No overlap, skip this SST file
      // cout << "skip" << ++i << endl;
      continue;
    }
    // Use SearchInSST to search for the key in the current SST file
    KeyValue result = SearchInSST(sst_info->filename, _key);

    // If the result is not empty, return the found key-value pair
    if (!result.isEmpty()) {
      return result;
    }
  }

  // If no result was found, return an empty KeyValue
  return KeyValue();
}


// scan in all SST files [from OLDEST to YOUNGEST]
void SSTIndex::Scan(KeyValue smallestKey, KeyValue largestKey, set<KeyValue>& res) {
  int i = 0;
  // Traverse the deque from the youngest (back) to the oldest (front)
  for (auto it = index.rbegin(); it != index.rend(); ++it) {
    SSTInfo* sst_info = *it;

    // Check if the range overlaps with the key range in this SST file
    if (sst_info->largest_key < smallestKey || sst_info->smallest_key > largestKey) {
      // No overlap, skip this SST file
      // cout << "skip" << ++i << endl;
      continue;
    }

    // Scan the key-value pairs in the current SST file that fall within the specified range
    ScanInSST(smallestKey, largestKey, sst_info->filename, res);
  }
}


// scan kv-pairs inside sst file
void SSTIndex::ScanInSST(KeyValue smallestKey, KeyValue largestKey, const string& filename, set<KeyValue>& resultSet) {
  // Use the FileManager to load the SST file into a RedBlackTree
  RedBlackTree* tree = fileManager.loadFromDisk(filename);

  // Check if the tree is null or empty (in case the file couldn't be loaded)
  if (!tree) {
    throw std::runtime_error("SSTIndex::ScanInSST() >>>> Failed to load SST file: " + filename);
  }

  // Perform an inorder traversal of the tree
  tree->inOrderTraversal([&](const KeyValue& kv) {
      // Print all traversed key-values to debug
      // std::cout << "Traversed Key: " << std::get<int>(kv.getKey()) << ", Value: " << std::get<string>(kv.getValue()) << "\n";

      // Check if the current key is within the specified range [smallestKey, largestKey]
      if (kv >= smallestKey && kv <= largestKey) {
          // std::cout << "Key within range: " << std::get<int>(kv.getKey()) << "\n";
          resultSet.insert(kv);  // Insert into the result set if within range
      }
  });

  // Clean up: delete the tree once we're done
  delete tree;
}


