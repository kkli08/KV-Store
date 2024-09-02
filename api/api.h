//
// Created by Damian Li on 2024-08-26.
//

#ifndef API_H
#define API_H
#include <string>
#include "Memtable.h"
#include <filesystem> // C++17 lib
#include "SSTIndex.h"

namespace fs = std::filesystem;
using namespace std;
namespace kvdb {
    class API {
    public:
        API() : memtable_size(1e4), memtable(new Memtable()), index(new SSTIndex()) {};
        ~API();
        void Open(string db_name);
        void Close();
        void Put(long long key, long long value);
        long long Get(long long key);
        Memtable* GetMemtable() const {return memtable;};
        void IndexCheck();

    private:
        Memtable *memtable;
        SSTIndex *index;
        int memtable_size;
        fs::path path; // path for store SSTs
        bool is_open;
        // helper function: set memtable_size
        void set_memtable_size(int _size);
        void set_path(fs::path);
        void cleanup();
        void check_if_open() const {
            if (!is_open) {
                throw runtime_error("Database is not open. Please open the database before performing operations.");
            }
        }
    };
}
#endif //API_H
