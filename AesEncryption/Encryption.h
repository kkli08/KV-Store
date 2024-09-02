//
// Created by Damian Li on 2024-09-01.
//

#ifndef ENCRYPTION_H
#define ENCRYPTION_H
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string>
#include <vector>
#include <stdexcept>

class Encryption {
public:
    Encryption(const std::string &key, size_t blockSize = 8) : blockSize(blockSize) {
        if (key.size() != 16) {
            throw std::runtime_error("Key must be 16 bytes long");
        }
        // Copy the key to the member variable
        this->key = key;

        // Ensure the block size is a valid value (e.g., 8, 16, 32, 64, etc.)
        if (blockSize % EVP_CIPHER_block_size(EVP_aes_128_ecb()) != 0) {
            throw std::runtime_error("Block size must be a multiple of the AES block size (16 bytes)");
        }
    }

    std::string encrypt(const std::string &data) {
        std::string paddedData = pad(data, blockSize);
        std::vector<unsigned char> encryptedData(paddedData.size());

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("Failed to create encryption context");

        int len;
        if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, (unsigned char*)key.c_str(), nullptr))
            throw std::runtime_error("Failed to initialize encryption");

        if (1 != EVP_EncryptUpdate(ctx, encryptedData.data(), &len, (unsigned char*)paddedData.c_str(), paddedData.size()))
            throw std::runtime_error("Encryption failed");

        int ciphertext_len = len;
        if (1 != EVP_EncryptFinal_ex(ctx, encryptedData.data() + len, &len))
            throw std::runtime_error("Failed to finalize encryption");
        ciphertext_len += len;

        EVP_CIPHER_CTX_free(ctx);
        return std::string(encryptedData.begin(), encryptedData.begin() + ciphertext_len);
    }

    std::string decrypt(const std::string &data) {
        std::vector<unsigned char> decryptedData(data.size());

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("Failed to create decryption context");

        int len;
        if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, (unsigned char*)key.c_str(), nullptr))
            throw std::runtime_error("Failed to initialize decryption");

        if (1 != EVP_DecryptUpdate(ctx, decryptedData.data(), &len, (unsigned char*)data.c_str(), data.size()))
            throw std::runtime_error("Decryption failed");

        int plaintext_len = len;
        if (1 != EVP_DecryptFinal_ex(ctx, decryptedData.data() + len, &len))
            throw std::runtime_error("Failed to finalize decryption");
        plaintext_len += len;

        EVP_CIPHER_CTX_free(ctx);
        return unpad(std::string(decryptedData.begin(), decryptedData.begin() + plaintext_len));
    }

private:
    std::string key;
    size_t blockSize;  // Block size in bytes

    std::string pad(const std::string &data, size_t blockSize) {
        size_t paddingSize = blockSize - (data.size() % blockSize);
        return data + std::string(paddingSize, paddingSize);
    }

    std::string unpad(const std::string &data) {
        size_t paddingSize = static_cast<size_t>(data.back());
        return data.substr(0, data.size() - paddingSize);
    }
};


// class Encryption {
// public:
//     Encryption(const std::string &key) {
//         if (key.size() != 16) {
//             throw std::runtime_error("Key must be 16 bytes long");
//         }
//         // Copy the key to the member variable
//         this->key = key;
//     }
//
//     std::string encrypt(const std::string &data) {
//         // std::string paddedData = pad(data, EVP_CIPHER_block_size(EVP_aes_128_ecb()));
//         std::string paddedData = pad(data, EVP_CIPHER_block_size(EVP_aes_128_ecb()));
//         std::vector<unsigned char> encryptedData(paddedData.size());
//
//         EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
//         if (!ctx) throw std::runtime_error("Failed to create encryption context");
//
//         int len;
//         if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, (unsigned char*)key.c_str(), nullptr))
//             throw std::runtime_error("Failed to initialize encryption");
//
//         if (1 != EVP_EncryptUpdate(ctx, encryptedData.data(), &len, (unsigned char*)paddedData.c_str(), paddedData.size()))
//             throw std::runtime_error("Encryption failed");
//
//         int ciphertext_len = len;
//         if (1 != EVP_EncryptFinal_ex(ctx, encryptedData.data() + len, &len))
//             throw std::runtime_error("Failed to finalize encryption");
//         ciphertext_len += len;
//
//         EVP_CIPHER_CTX_free(ctx);
//         return std::string(encryptedData.begin(), encryptedData.begin() + ciphertext_len);
//     }
//
//     std::string decrypt(const std::string &data) {
//         std::vector<unsigned char> decryptedData(data.size());
//
//         EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
//         if (!ctx) throw std::runtime_error("Failed to create decryption context");
//
//         int len;
//         if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, (unsigned char*)key.c_str(), nullptr))
//             throw std::runtime_error("Failed to initialize decryption");
//
//         if (1 != EVP_DecryptUpdate(ctx, decryptedData.data(), &len, (unsigned char*)data.c_str(), data.size()))
//             throw std::runtime_error("Decryption failed");
//
//         int plaintext_len = len;
//         if (1 != EVP_DecryptFinal_ex(ctx, decryptedData.data() + len, &len))
//             throw std::runtime_error("Failed to finalize decryption");
//         plaintext_len += len;
//
//         EVP_CIPHER_CTX_free(ctx);
//         return unpad(std::string(decryptedData.begin(), decryptedData.begin() + plaintext_len));
//     }
//
// private:
//     std::string key;
//
//     std::string pad(const std::string &data, size_t blockSize) {
//         size_t paddingSize = blockSize - (data.size() % blockSize);
//         return data + std::string(paddingSize, paddingSize);
//     }
//
//     std::string unpad(const std::string &data) {
//         size_t paddingSize = static_cast<size_t>(data.back());
//         return data.substr(0, data.size() - paddingSize);
//     }
// };


#endif //ENCRYPTION_H
