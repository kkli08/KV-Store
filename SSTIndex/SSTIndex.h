//
// Created by Damian Li on 2024-08-29.
//
#include <map>
#include <vector>
#include <string>

#ifndef SSTINDEX_H
#define SSTINDEX_H

using namespace std;

struct SSTInfo {
  std::string filename;
  long long smallest_key;
  long long largest_key;
};

struct SSTIndexNode{
  SSTInfo* info;
  SSTIndexNode* next;
  SSTIndexNode* prev;
};

class SSTIndex {
  public:
  // Retrieve all SSTs (e.g., when reopening the database)
  const vector<SSTInfo>& getAllSSTs();

  // flush index info into "Index.sst"
  void flushToDisk();

  // Add a new SST to the index
  void addSST(const string& filename, long long smallest_key, long long largest_key) {
    index[filename] = {filename, smallest_key, largest_key};
  }

private:
  map<std::string, SSTInfo> index;

};



#endif //SSTINDEX_H
