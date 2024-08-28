//
// Created by Damian Li on 2024-08-26.
//

#ifndef API_H
#define API_H
#include <string>
#include "Memtable.h"
#include <filesystem> // C++17 lib

namespace fs = std::filesystem;
using namespace std;
class API {
    public:
        API() : memtable_size(1e4), memtable(new Memtable()) {};
        ~API();
        void Open(string db_name);
        void Close();
        void Put(long long key, long long value);
        long long Get(long long key);
        Memtable* GetMemtable() const {return memtable;};

    private:
        Memtable *memtable;
        int memtable_size;
        fs::path path;
        // helper function: set memtable_size
        void set_memtable_size(int _size);
        void set_path(fs::path);
        void cleanup();

};
#endif //API_H
