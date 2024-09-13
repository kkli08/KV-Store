## KV-Store Database
![Multi-Platform Unit Tests](https://github.com/kkli08/KV-Store/actions/workflows/cmake-multi-platform.yml/badge.svg)

Stores key-value pairs and allows retrieval of a value based on its key.
> 2024-09-12 Restructure data storage with Protobuf. 
> 
> Latest KvDB Repo --> [here](https://github.com/kkli08/KvDB)

> 2024-09-09 Support Template<typename K, typename V> 
```c++
enum class KeyValueType { INT, LONG, DOUBLE, CHAR, STRING };
```

> 2024-08-28 Support <int_64, int_64> 
### Operations
**kvdb::API::Open(string db_name)**
> Initializes the database system for all database files, including SSTs and other related data.
```c++
// default memtable size = 1e4
auto MyDBDefault = new kvdb::API();
MyDB_default->Open("database name");

// set memtable size
int memtableSize = 1e3;
auto MyDBSetMemtableSize = new kvdb::API(memtableSize);
MyDBSetMemtableSize->Open("database name");

// smart pointer
int memtableSize = 1e3;
auto MyDBSetMemtableSize = std::make_unique<kvdb::API>(memtableSize);
MyDBSetMemtableSize->Open("database name");
```
**template<typename K, typename V>**

**kvdb::API::Put(K key, V value)**
> Put key-value pair into the database.
```c++
auto MyDB = new kvdb::API();
MyDB->Open("database name");
MyDB->Put(1, 100);
MyDB->Put(1.5, 'A');
MyDB->Put("Hello, World", 1e8LL);
```
**kvdb::API::Get(int_64)**
> Return value by the key.
```c++
auto MyDB = new kvdb::API();
KeyValue kv;
MyDB->Open("database name");
MyDB->Put(1, 100);
MyDB->Put(1.5, 'A');
MyDB->Put("Hello, World", 1e8LL);
kv = MyDB->Get("Hello, World"); // return kv -> {key: "Hello, World", value: 1e8LL}
```
**kvdb::API::Close()**
> Close the db, move all the data in memory into disk (SSTs).
```c++
auto MyDB = new kvdb::API();
MyDB->Open("database name");
MyDB->Close();
```
**kvdb::API::Scan(KeyValue smallestKey, KeyValue largestKey)**
> Retrieves all KV-pairs in a key range in key order (key1 < key2)
```c++
auto MyDB = new kvdb::API();
set<KeyValue> KvPairs;
MyDB->Open("database name");
KvPairs = MyDB->Scan(smallestKey, largestKey2);
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


### SST File Layout
> 2024-09-09
>
![SSTLayout](/img/SSTFileLayout_v1.1.jpg)

### UML
> 2024-09-02 
> 
![UML](/img/kvdb_s1_uml.jpg)

### Dataflow Diagram
![DFD](/img/kvdb_lv0.jpg)

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
