//
// Created by Damian Li on 2024-08-26.
//

#ifndef MEMTABLE_H
#define MEMTABLE_H
#include <cstdint>
#include <cstddef>
#include "RedBlackTree.h"

class Memtable {
    public:
      Memtable(int threshold);
      ~Memtable();
      void put(long long key, long long value);
      long long get(long long key);

    private:
      RedBlackTree* tree;
      int memtable_size; // maximum size of memtable
      int current_size = 0;
      void flushToDisk();
};


#endif //MEMTABLE_H
