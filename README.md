## KV-Store Database
![Multi-Platform Unit Tests](https://github.com/kkli08/KV-Store/actions/workflows/cmake-multi-platform.yml/badge.svg)

Stores key-value pairs and allows retrieval of a value based on its key.
> 2024-08-28 Support <int_64, int_64> 
### Operations
**kvdb::API::Open(string db_name)**
> Initializes the database system for all database files, including SSTs and other related data.
```c++
auto MyDB = new kvdb::API();
MyDB->Open("database name");
```
**kvdb::API::Put(int_64 key, int_64 value)**
> Put key-value pair into the database.
```c++
auto MyDB = new kvdb::API();
long long key = 1, value = 100;
MyDB->Open("database name");
MyDB->Put(key, value);
```
**kvdb::API::Get(int_64)**
> Return value by the key.
```c++
auto MyDB = new kvdb::API();
long long key = 1, value;
MyDB->Open("database name");
MyDB->Put(key, value);
value = MyDB->Get(key);
```
**kvdb::API::Close()**
> Close the db, move all the data in memory into disk (SSTs).
```c++
auto MyDB = new kvdb::API();
MyDB->Open("database name");
MyDB->Close();
```
**kvdb::API::Scan(int_64, int_64)**
> Retrieves all KV-pairs in a key range in key order (key1 < key2)
```c++
auto MyDB = new kvdb::API();
unordered_map<long long, long long> KvPairs;
MyDB->Open("database name");
KvPairs = MyDB->Scan(Key1, Key2);
```
**kvdb::API::Update()**
> Update the data.
```c++
TBA
```
**kvdb::API::Delete()**
> Delete the data.
```c++
TBA
```

### Benchmark

| Date       | Operation Type | Number of Pairs | Total Time Taken (ms) | Average Time per Operation (ms) |
|------------|----------------|-----------------|-----------------------|---------------------------------|
| 2024-09-02 | Put            | 1M              | 369                   | N/A                             |
| 2024-09-02 | Get            | 1M              | 30529                 | 0.030529                        |
| 2024-09-02 | Scan           | 1M out of 100M  | 207                   | 0.000207                        |

### Supported Platforms and Compilers
| Platform      | Compiler       | Status |
|---------------|----------------|--|
| Ubuntu 20.04  | GCC            | ✅ |
| Ubuntu 20.04  | Clang          | ✅ |
| Windows       | MSVC (cl)      | ✅ |
| macOS         | Clang          | ✅ |
| macOS         | GCC            | ✅ |

