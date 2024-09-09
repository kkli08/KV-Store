//
// Created by Damian Li on 2024-09-07.
//
#include "KeyValue.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
using namespace std;
// Accessor methods
KeyValue::KeyType KeyValue::getKey() const {
    return key;
}

KeyValue::ValueType KeyValue::getValue() const {
    return value;
}

KeyValue::KeyValueType KeyValue::getKeyType() const {
    return keyType;
}

KeyValue::KeyValueType KeyValue::getValueType() const {
    return valueType;
}

// Comparison operator
bool KeyValue::operator<(const KeyValue& other) const {
    return std::visit([](auto&& arg1, auto&& arg2) -> bool {
        using T1 = std::decay_t<decltype(arg1)>;
        using T2 = std::decay_t<decltype(arg2)>;

        if constexpr (std::is_same_v<T1, T2>) {
            // If both types are the same, compare them normally
            if constexpr (std::is_arithmetic_v<T1>) {
                return arg1 < arg2;  // Numeric comparison
            } else {
                return arg1 < arg2;  // Lexicographical comparison for strings and chars
            }
        } else if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>) {
            // Handle mixed numeric type comparisons (e.g., int vs double)
            return static_cast<double>(arg1) < static_cast<double>(arg2);
        } else {
            // If types differ, prioritize numeric vs. string/char comparison
            if constexpr (std::is_arithmetic_v<T1> && (std::is_same_v<T2, std::string> || std::is_same_v<T2, char>)) {
                return true;  // Numeric is always smaller than string/char
            } else if constexpr ((std::is_same_v<T1, std::string> || std::is_same_v<T1, char>) && std::is_arithmetic_v<T2>) {
                return false;  // String/char is always larger than numeric
            } else {
                throw std::invalid_argument("Unsupported type comparison");
            }
        }
    }, this->key, other.getKey());
}



// Define operator> in terms of operator<
bool KeyValue::operator>(const KeyValue& other) const {
    return other < *this;
}

// Define operator<= in terms of operator<
bool KeyValue::operator<=(const KeyValue& other) const {
    return !(other < *this);
}

// Define operator>= in terms of operator<
bool KeyValue::operator>=(const KeyValue& other) const {
    return !(*this < other);
}

// Comparison operator for key equality
// Only for key
bool KeyValue::operator==(const KeyValue& other) const {
    // Compare the keys using std::visit
    bool keysEqual = std::visit([](auto&& arg1, auto&& arg2) -> bool {
        using T1 = std::decay_t<decltype(arg1)>;
        using T2 = std::decay_t<decltype(arg2)>;
        if constexpr (std::is_same_v<T1, T2>) {
            return arg1 == arg2;  // Compare if both types are the same
        } else if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>) {
            // Compare different numeric types by casting them to double
            /*
             * if type matters, comment else if statement
             */
            return static_cast<double>(arg1) == static_cast<double>(arg2);
        } else {
            return false;  // Different types are not equal
        }
    }, this->key, other.getKey());

    // keys must be equal
    return keysEqual;
}


// Print key-value
void KeyValue::printKeyValue() const {
    std::cout << "Key Type: " << keyValueTypeToString(keyType) << std::endl;
    std::visit([](auto&& arg) { std::cout << "Key: " << arg << std::endl; }, key);

    std::cout << "Value Type: " << keyValueTypeToString(valueType) << std::endl;
    std::visit([](auto&& arg) { std::cout << "Value: " << arg << std::endl; }, value);
}

// Enum to string conversion
std::string KeyValue::keyValueTypeToString(KeyValueType type) {
    switch (type) {
        case KeyValueType::INT: return "INT";
        case KeyValueType::LONG: return "LONG";
        case KeyValueType::DOUBLE: return "DOUBLE";
        case KeyValueType::CHAR: return "CHAR";
        case KeyValueType::STRING: return "STRING";
        default: return "UNKNOWN";
    }
}

// Method to check if the KeyValue is empty (no valid key or value)
bool KeyValue::isEmpty() const {
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        // Check if the value is the default for its type (e.g., 0 for int, empty string, etc.)
        if constexpr (std::is_arithmetic_v<T>) {
            return arg == 0;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg.empty();
        } else if constexpr (std::is_same_v<T, char>) {
            return arg == '\0';
        }
        return true;  // Handle other cases as empty
    }, this->key);
}


