//
// Created by Damian Li on 2024-09-07.
//
#include "KeyValue.pb.h"
#include <iostream>
#include <stdexcept>
#include <string>

#ifndef KEYVALUEWRAPPER_H
#define KEYVALUEWRAPPER_H

using namespace std;

class KeyValueWrapper {
public:
    // Default constructor
    KeyValueWrapper() = default;

    // Templated constructor that automatically deduces the type
    template<typename K, typename V>
    KeyValueWrapper(K key, V value);

    // Accessor methods
    KeyValue::KeyValueType getKeyType() const { return kv.key_type(); }
    KeyValue::KeyValueType getValueType() const { return kv.value_type(); }

    // Print key-value pair
    void printKeyValue() const;
    string keyValueTypeToString(KeyValue::KeyValueType type) const;

    // Comparison operator for keys
    bool operator<(const KeyValueWrapper& other) const;
    bool operator>(const KeyValueWrapper& other) const;
    bool operator<=(const KeyValueWrapper& other) const;
    bool operator>=(const KeyValueWrapper& other) const;
    bool operator==(const KeyValueWrapper& other) const;

    // Serialize and Deserialize methods
    void serialize(std::ostream& os) const;
    static KeyValueWrapper deserialize(std::istream& is);

    bool isEmpty() const;
    KeyValue kv;  // Protobuf-generated KeyValue object


private:

    // Helper to deduce and set the key type in Protobuf
    template<typename T>
    void setKey(T key);

    // Helper to deduce and set the value type in Protobuf
    template<typename T>
    void setValue(T value);
};

#include "KeyValue.tpp"

#endif