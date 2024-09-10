//
// Created by Damian Li on 2024-09-08.
//

#include "FileManager.h"
#include <stdexcept>
#include <functional>
#include <cstdint>

// Constructor
FileManager::FileManager() : directory("defaultDB") {}

// Constructor with custom directory
FileManager::FileManager(fs::path dir) : directory(std::move(dir)) {
    if (!fs::exists(directory)) {
        fs::create_directories(directory);  // Ensure the directory exists
    }
}


// Set directory for SST files
void FileManager::setDirectory(const fs::path& path) {
    directory = path;
}

// Get directory for SST files
fs::path FileManager::getDirectory() const {
    return directory;
}

// Generate a unique filename for an SST file
std::string FileManager::generateSstFilename() {
    return "sst_" + std::to_string(increaseFileCounter()) + ".sst";
}

/*
 * SST Header
 *
 */
uint32_t SSTHeader::calculateChecksum() const {
    // The header contains two fields, each 4 bytes (uint32_t size)
    uint32_t checksum = sizeof(num_key_values) + sizeof(header_checksum);
    return checksum;
}

/*
 * .sst File SSTHeader Structure
 * void SSTHeader::serialize(ofstream&)
 * ==============================================================================
 * num_key_values | header_checksum |
 * ==============================================================================
 */
void SSTHeader::serialize(std::ofstream& file) const {
    file.write(reinterpret_cast<const char*>(&num_key_values), sizeof(num_key_values));
    file.write(reinterpret_cast<const char*>(&header_checksum), sizeof(header_checksum));
}

SSTHeader SSTHeader::deserialize(std::ifstream& file) {
    SSTHeader header;
    file.read(reinterpret_cast<char*>(&header.num_key_values), sizeof(header.num_key_values));
    file.read(reinterpret_cast<char*>(&header.header_checksum), sizeof(header.header_checksum));
    return header;
}


/*
 * Serialized Key Value
 *
 */
uint32_t SerializedKeyValue::calculateChecksum() const {
    uint32_t checksum = 0;

    // Start with the size of the checksum field itself (4 bytes)
    checksum += sizeof(kv_checksum);

    // Add the size of the key type (4 bytes, since it's an enum)
    checksum += sizeof(KeyValue::KeyValueType);

    // Add the size of the key itself
    std::visit([&](auto&& arg) {
        checksum += sizeof(arg);  // Add the size of the key
    }, kv.getKey());

    // Add the size of the value type (4 bytes, since it's an enum)
    checksum += sizeof(KeyValue::KeyValueType);

    // Add the size of the value itself
    std::visit([&](auto&& arg) {
        checksum += sizeof(arg);  // Add the size of the value
    }, kv.getValue());

    return checksum;
}

/*
 * .sst File SerializedKeyValue Structure
 * void SerializedKeyValue::serialize(ofstream&)
 * ==============================================================================
 * String Type:
 * kv_checksum | keyType | str_len | (string) keyValue | valueType | valueValue |
 * ==============================================================================
 * Other Type:
 * kv_checksum | keyType | (T) keyValue | valueType | valueValue |
 * ==============================================================================
 */
void SerializedKeyValue::serialize(std::ofstream& file) const {
    // Write the checksum (total length of the key-value block)
    file.write(reinterpret_cast<const char*>(&kv_checksum), sizeof(kv_checksum));

    // Write the key type
    KeyValue::KeyValueType keyType = kv.getKeyType();
    file.write(reinterpret_cast<const char*>(&keyType), sizeof(KeyValue::KeyValueType));

    // Write the key based on its type
    std::visit([&file](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            uint32_t str_len = arg.size();
            file.write(reinterpret_cast<const char*>(&str_len), sizeof(str_len));  // Write string length
            file.write(arg.data(), str_len);  // Write string data
        } else {
            file.write(reinterpret_cast<const char*>(&arg), sizeof(arg));  // Write other types (int, double, etc.)
        }
    }, kv.getKey());

    // Write the value type
    KeyValue::KeyValueType valueType = kv.getValueType();
    file.write(reinterpret_cast<const char*>(&valueType), sizeof(KeyValue::KeyValueType));

    // Write the value based on its type
    std::visit([&file](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            uint32_t str_len = arg.size();
            file.write(reinterpret_cast<const char*>(&str_len), sizeof(str_len));  // Write string length
            file.write(arg.data(), str_len);  // Write string data
        } else {
            file.write(reinterpret_cast<const char*>(&arg), sizeof(arg));  // Write other types (int, double, etc.)
        }
    }, kv.getValue());
}


SerializedKeyValue SerializedKeyValue::deserialize(std::ifstream& file) {
    SerializedKeyValue skv;

    // Read the checksum
    file.read(reinterpret_cast<char*>(&skv.kv_checksum), sizeof(skv.kv_checksum));

    // Read the key type
    KeyValue::KeyValueType keyType;
    file.read(reinterpret_cast<char*>(&keyType), sizeof(KeyValue::KeyValueType));

    // Deserialize the key based on its type
    KeyValue::KeyType key;
    switch (keyType) {
        case KeyValue::KeyValueType::INT: {
            int k;
            file.read(reinterpret_cast<char*>(&k), sizeof(k));
            key = k;
            break;
        }
        case KeyValue::KeyValueType::LONG: {
            long long k;
            file.read(reinterpret_cast<char*>(&k), sizeof(k));
            key = k;
            break;
        }
        case KeyValue::KeyValueType::DOUBLE: {
            double k;
            file.read(reinterpret_cast<char*>(&k), sizeof(k));
            key = k;
            break;
        }
        case KeyValue::KeyValueType::CHAR: {
            char k;
            file.read(reinterpret_cast<char*>(&k), sizeof(k));
            key = k;
            break;
        }
        case KeyValue::KeyValueType::STRING: {
            std::string k;
            uint32_t str_len;
            file.read(reinterpret_cast<char*>(&str_len), sizeof(str_len));  // Read string length
            k.resize(str_len);
            file.read(&k[0], str_len);  // Read string content
            key = k;
            break;
        }
        default:
            throw std::runtime_error("FileManager::SerializedKeyValue::deserialize()::Key >>>> Unsupported key type");
    }

    // Read the value type
    KeyValue::KeyValueType valueType;
    file.read(reinterpret_cast<char*>(&valueType), sizeof(KeyValue::KeyValueType));

    // Deserialize the value based on its type
    KeyValue::ValueType value;
    switch (valueType) {
        case KeyValue::KeyValueType::INT: {
            int v;
            file.read(reinterpret_cast<char*>(&v), sizeof(v));
            value = v;
            break;
        }
        case KeyValue::KeyValueType::LONG: {
            long long v;
            file.read(reinterpret_cast<char*>(&v), sizeof(v));
            value = v;
            break;
        }
        case KeyValue::KeyValueType::DOUBLE: {
            double v;
            file.read(reinterpret_cast<char*>(&v), sizeof(v));
            value = v;
            break;
        }
        case KeyValue::KeyValueType::CHAR: {
            char v;
            file.read(reinterpret_cast<char*>(&v), sizeof(v));
            value = v;
            break;
        }
        case KeyValue::KeyValueType::STRING: {
            std::string v;
            uint32_t str_len;
            file.read(reinterpret_cast<char*>(&str_len), sizeof(str_len));  // Read string length
            v.resize(str_len);
            file.read(&v[0], str_len);  // Read string content
            value = v;
            break;
        }
        default:
            throw std::runtime_error("FileManager::SerializedKeyValue::deserialize()::Value >>>> Unsupported value type");
    }

    // Now use the KeyValue constructor to set the key and value
    skv.kv = KeyValue(key, value);

    return skv;
}





/*
 * File Manager Public Methods
 */
FlushSSTInfo FileManager::flushToDisk(const std::vector<KeyValue>& kv_pairs) {
    FlushSSTInfo flushInfo;
    flushInfo.fileName = generateSstFilename();

    // Make sure directory exists
    if (!fs::exists(directory)) {
        throw std::runtime_error("FileManager::flushToDisk() >>>> Directory does not exist: " + directory.string());
    }

    std::ofstream file(directory / flushInfo.fileName, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("FileManager::flushToDisk() >>>> Could not open SST file for writing.");
    }

    if (kv_pairs.size() < 1) return flushInfo;
    // Set the smallest and largest keys
    flushInfo.smallest_key = kv_pairs.front();
    flushInfo.largest_key = kv_pairs.back();

    // Create the header
    sstHeader.num_key_values = kv_pairs.size();
    sstHeader.header_checksum = sstHeader.calculateChecksum();

    // Write header to disk
    sstHeader.serialize(file);

    // Write each serialized key-value pair
    for (const auto& kv : kv_pairs) {
        SerializedKeyValue skv;
        skv.kv = kv;
        skv.kv_checksum = skv.calculateChecksum();
        skv.serialize(file);
    }

    file.close();
    return flushInfo;
}

RedBlackTree* FileManager::loadFromDisk(const std::string& sst_filename) {
    // Open the SST file
    std::ifstream file(directory / sst_filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("FileManager::loadFromDisk() >>>> Could not open SST file for reading.");
    }

    // Check if the file is empty
    file.seekg(0, std::ios::end);  // Move to the end of the file
    std::streampos file_size = file.tellg();  // Get the size of the file
    file.seekg(0, std::ios::beg);  // Move back to the beginning of the file

    if (file_size == 0) {
        // If the file is empty, return an empty RedBlackTree
        file.close();
        return new RedBlackTree();  // Returning an empty tree
    }

    // Deserialize the SST header
    SSTHeader sstHeader = SSTHeader::deserialize(file);

    // Create a new RedBlackTree to hold the KeyValue pairs
    auto* tree = new RedBlackTree();

    // Loop through the number of KeyValue pairs
    for (uint32_t i = 0; i < sstHeader.num_key_values; ++i) {
        // Deserialize each SerializedKeyValue from the file
        SerializedKeyValue skv = SerializedKeyValue::deserialize(file);

        // Insert the deserialized KeyValue into the RedBlackTree
        tree->insert(skv.kv);
    }

    // Close the file
    file.close();

    // Return the populated RedBlackTree
    return tree;
}

