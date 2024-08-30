//
// Created by Damian Li on 2024-08-29.
//

#include "SSTIndex.h"
#include <fstream>
#include <sstream>
using namespace std;

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
long long SSTIndex::SearchInSST(const string& filename, long long _key) {
  std::ifstream infile(path / filename);
  if (!infile.is_open()) {
    // File doesn't exist
    return -1;
  }

  // Reset the file pointer to the beginning
  infile.seekg(0, std::ios::end);
  std::streampos file_size = infile.tellg();

  if (file_size == 0) {
    // File is empty
    infile.close();
    return -1;
  }

  /*
   * Implement binary search in the SST file
   * This is how the kv data stored in sst:
   * ofstream sst_file(filepath);
   * for (const auto& pair : kv_pairs) {
   *  sst_file << pair.first << ", " << pair.second << "\n";
   * }
   * sst_file.close();
   */
  long long left = 0;
  long long right = file_size;
  long long key;
  long long value;

  while (left <= right) {
    // Find the middle position
    long long mid = left + (right - left) / 2;
    infile.seekg(mid);

    // Move to the start of the line to read the full key-value pair
    // This is necessary because seekg may land in the middle of a line
    std::string line;
    std::getline(infile, line);

    if (infile.eof()) {
      infile.clear();  // Clear the eof flag to continue reading
    }

    // Skip to the next line if not at the beginning of a new line
    std::getline(infile, line);

    // Parse the key-value pair
    std::istringstream iss(line);
    std::string key_str, value_str;
    if (std::getline(iss, key_str, ',') && std::getline(iss, value_str)) {
      key = std::stoll(key_str);
      value = std::stoll(value_str);

      if (key == _key) {
        infile.close();
        return value;  // Key found, return the associated value
      } else if (key < _key) {
        left = infile.tellg();  // Move right
      } else {
        right = mid - 1;  // Move left
      }
    } else {
      // If the line is not formatted correctly, continue searching
      // Issue link : https://github.com/kkli08/KV-Store/issues/43
      right = mid - 1;
    }
  }

  infile.close();
  return -1;  // Key not found
}

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

