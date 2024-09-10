template<typename K, typename V>
void kvdb::API::Put(K key, V value) {
    check_if_open();


    KeyValue kv(key, value);
    FlushSSTInfo info = memtable->put(kv);

    if (!info.largest_key.isEmpty()) {
        // Handle flush info...

        // Debug Purpose :-D
        // cout << "\n>>>>>> Ready for flushing" << endl;
        // cout << info->fileName << "'s smallest key: " << info->smallest_key << " and largest key: " << info->largest_key << endl;

        // flush happens and safe access info attribute
          index->addSST(info.fileName, info.smallest_key, info.largest_key);
    }
}
