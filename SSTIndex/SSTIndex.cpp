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
  const long long record_size = 18;  // 8 bytes (key) + 1 byte (comma) + 8 bytes (value) + 1 byte (newline)
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


// BINARY SEARCH
// This function is not work since we store data in binary format
// long long SSTIndex::SearchInSST(const std::string& filename, long long _key) {
//   std::ifstream infile(path / filename);
//   if (!infile.is_open()) {
//     return -1;  // File doesn't exist
//   }
//
//   infile.seekg(0, std::ios::end);
//   std::streampos file_size = infile.tellg();
//   if (file_size == 0) {
//     infile.close();
//     return -1;  // File is empty
//   }
//
//   long long left = 0;
//   long long right = file_size;
//   long long key, value;
//   std::string line;
//
//   while (left <= right) {
//     // Calculate the midpoint
//     long long mid = left + (right - left) / 2;
//
//     // Seek to the approximate midpoint in the file
//     infile.seekg(mid, std::ios::beg);
//
//     // Move to the start of the next line if we are not at the beginning
//     if (mid != 0) {
//       infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//     }
//
//     // Read the key-value pair at the current position
//     if (!std::getline(infile, line)) {
//       break;  // Reached the end of the file or error
//     }
//
//     std::istringstream iss(line);
//     std::string key_str, value_str;
//     if (std::getline(iss, key_str, ',') && std::getline(iss, value_str)) {
//       key = std::stoll(key_str);
//       value = std::stoll(value_str);
//
//       if (key == _key) {
//         infile.close();
//         return value;  // Key found, return the associated value
//       } else if (key < _key) {
//         left = infile.tellg();  // Move right
//       } else {
//         right = mid - 1;  // Move left
//       }
//     } else {
//       // Handle malformed lines (if any)
//       infile.close();
//       return -1;  // Invalid line format
//     }
//   }
//
//   infile.close();
//   return -1;  // Key not found
// }


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

