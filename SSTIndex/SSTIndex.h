//
// Created by Damian Li on 2024-08-29.
//
#include <vector>
#include <string>
#include <deque>

#ifndef SSTINDEX_H
#define SSTINDEX_H
#include <filesystem> // C++17 lib

namespace fs = std::filesystem;
using namespace std;

struct SSTInfo {
  std::string filename;
  long long smallest_key;
  long long largest_key;
};

class SSTIndex {
  public:
  SSTIndex();
  ~SSTIndex(){};
  // Retrieve all SSTs into index (e.g., when reopening the database)
  void getAllSSTs();
  // flush index info into "Index.sst"
  void flushToDisk();
  // Add a new SST to the index
  void addSST(const string& filename, long long smallest_key, long long largest_key);
  // get index
  deque<SSTInfo*> getSSTsIndex() {return index;};
  // SST file binary search
  long long SearchInSST(const string& filename, long long _key);
  // Search in all SST files
  long long Search(long long);
  // helper function
  void set_path(fs::path);
  void printSSRsInFile();

private:
  deque<SSTInfo*> index;
  fs::path path;

};

#endif //SSTINDEX_H
