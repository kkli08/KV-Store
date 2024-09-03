//
// Created by Damian Li on 2024-08-26.
//

#ifndef API_H
#define API_H
#include <string>
#include "Memtable.h"
#include <filesystem> // C++17 lib
#include <unordered_map>
#include "SSTIndex.h"
#include <memory>

namespace fs = std::filesystem;
using namespace std;
namespace kvdb {
    class API {
    public:
        // constructor
        API()
                    : memtable_size(1e4),
                      memtable(make_unique<Memtable>(1e4)),
                      index(make_unique<SSTIndex>())
        {};

        API(int memtable_size)
                    : memtable_size(memtable_size),
                      memtable(make_unique<Memtable>(memtable_size)),
                      index(make_unique<SSTIndex>())
        {};
        // destructor
        ~API() = default;
        void Open(string db_name);
        void Close();

        void Put(long long key, long long value);
        long long Get(long long key);
        unordered_map<long long, long long> Scan(long long small_key, long long large_key);
        Memtable* GetMemtable() const {return memtable.get();};
        int SetMemtableSize(int memtable_size);
        void IndexCheck();

    private:
        unique_ptr<Memtable> memtable;
        unique_ptr<SSTIndex> index;

        int memtable_size;
        fs::path path; // path for store SSTs
        bool is_open = false;
        // helper function: set memtable_size
        void set_memtable_size(int memtable_size);
        void set_path(fs::path);
        void check_if_open() const {
            if (!is_open) {
                throw runtime_error("Database is not open. Please open the database before performing operations.");
            }
        }
    };
}
#endif //API_H
