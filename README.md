## KV-Store Database
Stores key-value pairs and allows retrieval of a value based on its key.
> 2024-08-28 Support <int, int> 
### API
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
KV-pairs = Scan(Key1, Key2)
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