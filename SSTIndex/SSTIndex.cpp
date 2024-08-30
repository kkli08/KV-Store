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