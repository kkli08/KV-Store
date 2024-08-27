//
// Created by Damian Li on 2024-08-26.
//

#ifndef API_H
#define API_H
#include <string>
#include "Memtable.h"

using namespace std;
class API {
  private:
    Memtable *memtable;
    int memtable_size = 1e4;
  public:
    API() : memtable(new Memtable(memtable_size)) {};
    ~API();
    void Open(string db_name);
    void Close();
    void Put(long long key, long long value);
    long long Get(long long key);
    // helper function: set memtable_size
    void set_memtable_size(int _size);
};
#endif //API_H
