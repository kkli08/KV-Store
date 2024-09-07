//
// Created by Damian Li on 2024-09-07.
//
#include <variant> // c++17 new features
#include <string>
#include <iostream>


#ifndef KEYVALUE_H
#define KEYVALUE_H


using namespace std;

class KeyValue {
public:
    // Enum to record the type of key and value
    enum class KeyValueType { INT, LONG, DOUBLE, CHAR, STRING };

    using KeyType = std::variant<int, long long, double, char, std::string>;
    using ValueType = std::variant<int, long long, double, char, std::string>;

private:
    KeyType key;
    ValueType value;
    KeyValueType keyType;
    KeyValueType valueType;

public:
    // Templated constructor that automatically deduces the type
    template<typename K, typename V>
    KeyValue(K k, V v);

    // Accessor methods
    KeyType getKey() const;
    ValueType getValue() const;
    KeyValueType getKeyType() const;
    KeyValueType getValueType() const;

    // Comparison operator for keys
    bool operator<(const KeyValue& other) const;

    // Print key-value
    void printKeyValue() const;

    // Convert enum to string for printing purposes
    static std::string keyValueTypeToString(KeyValueType type);

private:
    // Function to deduce the type of the key and value and return the corresponding enum
    template<typename T>
    KeyValueType deduceType(const T& value) const;

    // Function to convert const char* to std::string
    template<typename T>
    KeyType convertToVariant(const T& value) const;  // Declared here
};

#include "KeyValue.tpp"  // Include the implementation for templated functions




#endif //KEYVALUE_H
