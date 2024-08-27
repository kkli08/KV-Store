# KV-Store
Stores key-value pairs and allows retrieval of a value based on its key.
## API
**Open and Run**
```
Open("database name")
```
**Stores a key associated with a value**
```
Put(key, value)
```
**Retrieves a value associated with a given key**
```
Value = Get(key)
```
**Retrieves all KV-pairs in a key range in key order (key1 < key2)**
```
KV-pairs = Scan(Key1, Key2)
```
**Close**
```
Close()
```
**Update**
```
TBA
```
**Delete**
```
TBA
```