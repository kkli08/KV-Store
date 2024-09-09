//
// Created by Damian Li on 2024-08-29.
//

#include "SSTIndex.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

#define RECORDE_SIZE 18

SSTIndex::SSTIndex() {
  path = fs::path("defaultDB");
}

// Retrieve all SSTs into index (e.g., when reopening the database)
void SSTIndex::getAllSSTs(){
  // Check if the file exists
  std::ifstream infile(path / "Index.sst");
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

  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    std::string filename;
    long long smallest_key, largest_key;

    if (!(iss >> filename >> smallest_key >> largest_key)) {
      throw std::runtime_error("Failed to parse Index.sst.");
    }

    addSST(filename, smallest_key, largest_key);
  }

  infile.close();
}


void SSTIndex::printSSRsInFile() {
  // Construct the full path to the "Index.sst" file
  fs::path indexFilePath = path / "Index.sst";

  // Check if the directory exists
  if (!fs::exists(path)) {
    std::cerr << "Directory does not exist: " << path << std::endl;
    return;
  }

  // Check if the file exists
  if (!fs::exists(indexFilePath)) {
    std::cerr << "File does not exist: " << indexFilePath << std::endl;
    return;
  }

  // Check if the file exists and open it
  std::ifstream infile(indexFilePath);
  if (!infile.is_open()) {
    std::cerr << "Failed to open file: " << indexFilePath << std::endl;
    return;
  }

  // Check if the file is empty
  infile.seekg(0, std::ios::end);
  if (infile.tellg() == 0) {
    std::cout << "The file is empty." << std::endl;
    infile.close();
    return;
  }

  // Reset the file pointer to the beginning
  infile.seekg(0, std::ios::beg);

  // Read and print each line in the file
  std::string line;
  while (std::getline(infile, line)) {
    std::cout << line << std::endl;
  }

  infile.close();
}

// flush index info into "Index.sst"
void SSTIndex::flushToDisk() {
  // Attempt to open (or create) the file "Index.sst"
  std::ofstream outfile(path / "Index.sst", std::ios::out | std::ios::trunc);

  if (!outfile.is_open()) {
    throw std::runtime_error("Failed to open Index.sst for writing.");
  }
  for (const auto& sst_info : index) {
    outfile << sst_info->filename << " "
            << sst_info->smallest_key << " "
            << sst_info->largest_key << "\n";
  }
  outfile.close();
  if (!outfile) {
    throw std::runtime_error("Failed to write to Index.sst.");
  }

}

// Add a new SST to the index
void SSTIndex::addSST(const string& filename, long long smallest_key, long long largest_key){
  SSTInfo* info = new SSTInfo{filename, smallest_key, largest_key};
  index.push_back(info);

  // Debug Purpose :-D
  // if (!index.empty()) {
  //   SSTInfo* info = index.back();
  //   cout << "\n-->Inside SSTIndex::addSST()" << endl;
  //   std::cout << info->filename << "'s smallest key: "
  //             << info->smallest_key << " and largest key: "
  //             << info->largest_key << std::endl;
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
  path = _path;
}


// SST file binary search
// fixed size line: 8 bytes (key) + 1 byte (comma) + 8 bytes (value) + 1 byte (newline)
long long SSTIndex::SearchInSST(const std::string& filename, long long _key) {
  std::ifstream infile(path / filename, std::ios::binary);
  if (!infile.is_open()) {
    return -1;  // File doesn't exist
  }

  // Determine the size of the file
  infile.seekg(0, std::ios::end);
  std::streampos file_size = infile.tellg();
  if (file_size == 0) {
    infile.close();
    return -1;  // File is empty
  }

  // Calculate the number of records in the file
  const long long record_size = RECORDE_SIZE;  // 8 bytes (key) + 1 byte (comma) + 8 bytes (value) + 1 byte (newline)
  long long num_records = file_size / record_size;

  // Initialize binary search bounds
  long long left = 0;
  long long right = num_records - 1;
  long long key, value;

  while (left <= right) {
    // Calculate the midpoint index
    long long mid = left + (right - left) / 2;

    // Seek directly to the midpoint record
    infile.seekg(mid * record_size, std::ios::beg);

    // Read the key-value pair at the current position
    infile.read(reinterpret_cast<char*>(&key), sizeof(long long int));
    infile.ignore(1);  // Ignore the comma
    infile.read(reinterpret_cast<char*>(&value), sizeof(long long int));
    infile.ignore(1);  // Ignore the newline

    if (key == _key) {
      infile.close();
      return value;  // Key found, return the associated value
    } else if (key < _key) {
      left = mid + 1;  // Move right
    } else {
      right = mid - 1;  // Move left
    }
  }

  infile.close();
  return -1;  // Key not found
}


// search value for key
long long SSTIndex::Search(long long _key) {
  // Traverse the deque from the youngest (back) to the oldest (front)
  for (auto it = index.rbegin(); it != index.rend(); ++it) {
    SSTInfo* sst_info = *it;

    // Check if the key might be in this SST file based on the key range
    if (_key >= sst_info->smallest_key && _key <= sst_info->largest_key) {
      // Search in this SST file
      long long value = SearchInSST(sst_info->filename, _key);
      if (value != -1) {
        return value;  // Key found, return the value
      }
    }
  }
  // Key was not found in any SST file
  return -1;
}

// scan in all SST files [from OLDEST to YOUNGEST]
void SSTIndex::Scan(long long small_key, long long large_key, unordered_map<long long, long long>& res) {
  // Traverse the deque from the oldest (front) to the youngest (back)
  for (auto it = index.begin(); it != index.end(); ++it) {
    SSTInfo* sst_info = *it;

    // Check if the smallest key might be in this SST file based on the key range
    if (sst_info->largest_key >= small_key && sst_info->smallest_key <= large_key) {
      // Scan in this SST file
      ScanInSST(small_key, large_key, sst_info->filename, res);
    }
  }
}

// scan kv-pairs inside sst file
void SSTIndex::ScanInSST(long long small_key, long long large_key, const string& filename, unordered_map<long long, long long>& res) {
  // binary search for finding the smallest key k with the range small_key <= k
  // stop scanning until k > large_key

  std::ifstream infile(path / filename, std::ios::binary);
  if (!infile.is_open()) {
    cerr << "SSTIndex::ScanInSST() : File doesn't exist" << endl; // File doesn't exist
  }
  // Determine the size of the file
  infile.seekg(0, std::ios::end);
  std::streampos file_size = infile.tellg();
  if (file_size == 0) {
    infile.close();
    cerr << "SSTIndex::ScanInSST() : File is empty" << endl;  // File is empty
  }

  // Calculate the number of records in the file
  const long long record_size = RECORDE_SIZE;  // 8 bytes (key) + 1 byte (comma) + 8 bytes (value) + 1 byte (newline)
  long long num_records = file_size / record_size;

  // Initialize binary search bounds
  long long left = 0;
  long long right = num_records - 1;
  long long key = 0, value = 0;
  long long start_pos = -1;

  // Perform binary search to find the smallest key >= small_key
  while (left <= right) {
    long long mid = left + (right - left) / 2;

    // Seek directly to the midpoint record
    infile.seekg(mid * record_size, std::ios::beg);

    // Read the key-value pair at the current position
    infile.read(reinterpret_cast<char*>(&key), sizeof(long long));
    infile.ignore(1);  // Ignore the comma
    infile.read(reinterpret_cast<char*>(&value), sizeof(long long));
    infile.ignore(1);  // Ignore the newline

    if (key >= small_key) {
      start_pos = mid;
      right = mid - 1;  // Move left to find the smallest key >= small_key
    } else {
      left = mid + 1;  // Move right
    }
  }

  if (start_pos == -1) {
    infile.close();
    return;  // No key >= small_key found
  }

  // Start scanning from the found position
  infile.seekg(start_pos * record_size, std::ios::beg);
  while (infile.read(reinterpret_cast<char*>(&key), sizeof(long long))) {
    infile.ignore(1);  // Ignore the comma
    infile.read(reinterpret_cast<char*>(&value), sizeof(long long));
    infile.ignore(1);  // Ignore the newline

    if (key > large_key) {
      break;  // Stop scanning if the key is out of the specified range
    }

    // Add the key-value pair to the result map
    res[key] = value;
  }

  infile.close();
}

