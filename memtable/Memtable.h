//
// Created by Damian Li on 2024-08-26.
//

#ifndef MEMTABLE_H
#define MEMTABLE_H
#include <cstdint>
#include <cstddef>
#include "RedBlackTree.h"
#include <filesystem> // C++17 lib

namespace fs = std::filesystem;
using namespace std;

struct FlushSSTInfo {
    string fileName;
    long long smallest_key;
    long long largest_key;
};

/*
 * Handle in-memory operations.
 */
class Memtable {
    public:
      Memtable();
      Memtable(int threshold);
      ~Memtable();
      void put(long long key, long long value);
      long long get(long long key);
      void set_path(fs::path);
      fs::path get_path();
      FlushSSTInfo* flushToDisk();

      // helper function
      FlushSSTInfo* int_flush();
      string generateSstFilename();
      string longLongToString(long long value);
      long long stringToLongLong(const std::string &str);

    private:
      RedBlackTree* tree;
      int memtable_size; // maximum size of memtable
      int current_size = 0;
      fs::path path;
      int SST_file_size = 0;


};


#endif //MEMTABLE_H
