template<typename K, typename V>
KeyValueWrapper::KeyValueWrapper(K key, V value) {
    setKey(key);   // Use Protobuf to set the key
    setValue(value); // Use Protobuf to set the value
}

// Helper functions to set the key and value in the Protobuf object
template<typename T>
void KeyValueWrapper::setKey(T key) {
    if constexpr (std::is_same_v<T, int>) {
        kv.set_int_key(key);
        kv.set_key_type(KeyValue::INT);
    } else if constexpr (std::is_same_v<T, long long>) {
        kv.set_long_key(key);
        kv.set_key_type(KeyValue::LONG);
    } else if constexpr (std::is_same_v<T, double>) {
        kv.set_double_key(key);
        kv.set_key_type(KeyValue::DOUBLE);
    } else if constexpr (std::is_same_v<T, std::string>) {
        kv.set_string_key(key);
        kv.set_key_type(KeyValue::STRING);
    } else if constexpr (std::is_same_v<T, char>) {
        kv.set_char_key(std::string(1, key));  // Store char as string
        kv.set_key_type(KeyValue::CHAR);
    } else {
        throw std::invalid_argument("Unsupported key type");
    }
}

template<typename T>
void KeyValueWrapper::setValue(T value) {
    if constexpr (std::is_same_v<T, int>) {
        kv.set_int_value(value);
        kv.set_value_type(KeyValue::INT);
    } else if constexpr (std::is_same_v<T, long long>) {
        kv.set_long_value(value);
        kv.set_value_type(KeyValue::LONG);
    } else if constexpr (std::is_same_v<T, double>) {
        kv.set_double_value(value);
        kv.set_value_type(KeyValue::DOUBLE);
    } else if constexpr (std::is_same_v<T, std::string>) {
        kv.set_string_value(value);
        kv.set_value_type(KeyValue::STRING);
    } else if constexpr (std::is_same_v<T, char>) {
        kv.set_char_value(std::string(1, value));  // Store char as string
        kv.set_value_type(KeyValue::CHAR);
    } else {
        throw std::invalid_argument("Unsupported value type");
    }
}
