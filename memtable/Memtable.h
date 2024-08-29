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

class Memtable {
    public:
      Memtable();
      Memtable(int threshold);
      ~Memtable();
      void put(long long key, long long value);
      long long get(long long key);
      void set_path(fs::path);
      fs::path get_path();
      void flushToDisk();

      // helper function
      string int_flush();
      string generateSstFilename();

    private:
      RedBlackTree* tree;
      int memtable_size; // maximum size of memtable
      int current_size = 0;
      fs::path path;

};


#endif //MEMTABLE_H
