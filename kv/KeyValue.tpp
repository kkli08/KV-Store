template<typename K, typename V>
KeyValue::KeyValue(K k, V v)
    : key(convertToVariant(k)), value(convertToVariant(v)) {
    keyType = std::visit([&](auto&& arg) {
        return deduceType(arg);  // Deduce the actual type of the key
    }, key);
    valueType = std::visit([&](auto&& arg) {
        return deduceType(arg);  // Deduce the actual type of the value
    }, value);
}


template<typename T>
KeyValue::KeyValueType KeyValue::deduceType(const T& value) const {
    if constexpr (std::is_same_v<T, int>) {
        return KeyValueType::INT;
    } else if constexpr (std::is_same_v<T, long long>) {
        return KeyValueType::LONG;
    } else if constexpr (std::is_same_v<T, double>) {
        return KeyValueType::DOUBLE;
    } else if constexpr (std::is_same_v<T, char>) {
        return KeyValueType::CHAR;
    } else if constexpr (std::is_same_v<T, std::string>) {
        return KeyValueType::STRING;
    } else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
        return KeyValueType::STRING;  // Treat C-style strings as std::string
    } else {
        std::cerr << "Unsupported type: " << typeid(value).name() << std::endl;
        throw std::invalid_argument("Unsupported type");
    }
}


// Helper function to convert types like const char* to std::string
template<typename T>
KeyValue::KeyType KeyValue::convertToVariant(const T& value) const {
    if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
        return std::string(value);  // Convert C-string to std::string
    } else {
        return value;  // Return as-is for other types
    }
}
