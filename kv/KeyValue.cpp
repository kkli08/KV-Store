//
// Created by Damian Li on 2024-09-07.
//
#include "KeyValue.h"
#include "KeyValue.pb.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
using namespace std;

void KeyValueWrapper::serialize(std::ostream& os) const {
    kv.SerializeToOstream(&os);  // Use Protobuf serialization
}

KeyValueWrapper KeyValueWrapper::deserialize(std::istream& is) {
    KeyValue kv;
    kv.ParseFromIstream(&is);  // Use Protobuf deserialization
    KeyValueWrapper wrapper;
    wrapper.kv = kv;
    return wrapper;
}

bool KeyValueWrapper::isEmpty() const {
    return kv.key_case() == KeyValue::KEY_NOT_SET || kv.value_case() == KeyValue::VALUE_NOT_SET;
}



// Comparison operator
bool KeyValueWrapper::operator<(const KeyValueWrapper& other) const {
    // Compare based on which key type is set in the Protobuf message
    if (kv.has_int_key() && other.kv.has_int_key()) {
        return kv.int_key() < other.kv.int_key();
    } else if (kv.has_long_key() && other.kv.has_long_key()) {
        return kv.long_key() < other.kv.long_key();
    } else if (kv.has_double_key() && other.kv.has_double_key()) {
        return kv.double_key() < other.kv.double_key();
    } else if (kv.has_string_key() && other.kv.has_string_key()) {
        return kv.string_key() < other.kv.string_key();
    } else if (kv.has_char_key() && other.kv.has_char_key()) {
        return kv.char_key() < other.kv.char_key();
    } else if (kv.has_int_key() && other.kv.has_string_key()) {
        return true;  // Numeric keys are smaller than strings
    } else if (kv.has_string_key() && other.kv.has_int_key()) {
        return false;  // String keys are larger than numeric keys
    }
    throw std::invalid_argument("Unsupported or mismatched key types for comparison.");
}




// Define operator> in terms of operator<
bool KeyValueWrapper::operator>(const KeyValueWrapper& other) const {
    return other < *this;
}


// Define operator<= in terms of operator<
bool KeyValueWrapper::operator<=(const KeyValueWrapper& other) const {
    return !(other < *this);
}


// Define operator>= in terms of operator<
bool KeyValueWrapper::operator>=(const KeyValueWrapper& other) const {
    return !(*this < other);
}


// Comparison operator for key equality
// Only for key
bool KeyValueWrapper::operator==(const KeyValueWrapper& other) const {
    if (kv.has_int_key() && other.kv.has_int_key()) {
        return kv.int_key() == other.kv.int_key();
    } else if (kv.has_long_key() && other.kv.has_long_key()) {
        return kv.long_key() == other.kv.long_key();
    } else if (kv.has_double_key() && other.kv.has_double_key()) {
        return kv.double_key() == other.kv.double_key();
    } else if (kv.has_string_key() && other.kv.has_string_key()) {
        return kv.string_key() == other.kv.string_key();
    } else if (kv.has_char_key() && other.kv.has_char_key()) {
        return kv.char_key() == other.kv.char_key();
    }
    // Different types can't be equal
    return false;
}



// Print key-value
void KeyValueWrapper::printKeyValue() const {
    std::cout << "Key Type: " << KeyValueWrapper::keyValueTypeToString(kv.key_type()) << std::endl;

    if (kv.has_int_key()) {
        std::cout << "Key: " << kv.int_key() << std::endl;
    } else if (kv.has_long_key()) {
        std::cout << "Key: " << kv.long_key() << std::endl;
    } else if (kv.has_double_key()) {
        std::cout << "Key: " << kv.double_key() << std::endl;
    } else if (kv.has_string_key()) {
        std::cout << "Key: " << kv.string_key() << std::endl;
    } else if (kv.has_char_key()) {
        std::cout << "Key: " << kv.char_key() << std::endl;
    }

    std::cout << "Value Type: " << KeyValueWrapper::keyValueTypeToString(kv.value_type()) << std::endl;

    if (kv.has_int_value()) {
        std::cout << "Value: " << kv.int_value() << std::endl;
    } else if (kv.has_long_value()) {
        std::cout << "Value: " << kv.long_value() << std::endl;
    } else if (kv.has_double_value()) {
        std::cout << "Value: " << kv.double_value() << std::endl;
    } else if (kv.has_string_value()) {
        std::cout << "Value: " << kv.string_value() << std::endl;
    } else if (kv.has_char_value()) {
        std::cout << "Value: " << kv.char_value() << std::endl;
    }
}


// Enum to string conversion
std::string KeyValueWrapper::keyValueTypeToString(KeyValue::KeyValueType type) const {
    switch (type) {
        case KeyValue::INT: return "INT";
        case KeyValue::LONG: return "LONG";
        case KeyValue::DOUBLE: return "DOUBLE";
        case KeyValue::CHAR: return "CHAR";
        case KeyValue::STRING: return "STRING";
        default: return "UNKNOWN";
    }
}





