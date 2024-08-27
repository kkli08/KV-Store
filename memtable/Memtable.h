//
// Created by Damian Li on 2024-08-26.
//

#ifndef MEMTABLE_H
#define MEMTABLE_H
#include <cstdint>
#include <cstddef>

class Memtable {
    public:
      Memtable(size_t threshold);
      ~Memtable();
      void insert(int64_t key, int64_t value);
      int64_t search(int64_t key);

    private:
      size_t memtable_size; // maximum size of memtable
      size_t current_size = 0;
      void flushToDisk();
//      void destroyMemtable(RedBlackTreeNode* node);

};


#endif //MEMTABLE_H
