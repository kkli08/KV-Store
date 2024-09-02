// //
// // Created by Damian Li on 2024-09-01.
// //
// #include <gtest/gtest.h>
// #include "Encryption.h"
//
// using namespace std;
// const std::string TEST_KEY = "thisis16bitkey!!";  // 16-byte key for AES-128
//
// TEST(EncryptionTest, SimpleEncryptionDecryption) {
//     Encryption encryption(TEST_KEY, 32);
//
//     std::string originalText = "Hello, World!";
//     std::string encryptedText = encryption.encrypt(originalText);
//     cout << "Encrypted text length = " << encryptedText.length() << endl;
//     std::string decryptedText = encryption.decrypt(encryptedText);
//
//     EXPECT_EQ(originalText, decryptedText);
// }
//
// TEST(EncryptionTest, EncryptDecryptEmptyString) {
//     Encryption encryption(TEST_KEY);
//
//     std::string originalText = "";
//     std::string encryptedText = encryption.encrypt(originalText);
//     cout << "Encrypted text length = " << encryptedText.length() << endl;
//     std::string decryptedText = encryption.decrypt(encryptedText);
//
//     EXPECT_EQ(originalText, decryptedText);
// }
//
// TEST(EncryptionTest, EncryptDecryptStringWithPadding) {
//     Encryption encryption(TEST_KEY, 32);
//
//     std::string originalText = "123456789";
//     std::string encryptedText = encryption.encrypt(originalText);
//     std::string decryptedText = encryption.decrypt(encryptedText);
//
//     EXPECT_EQ(originalText, decryptedText);
// }
//
//
// // TEST(EncryptionTest, EncryptDecryptLongData) {
// //     Encryption encryption(TEST_KEY);
// //
// //     std::string originalText = "This is a longer string that spans multiple AES blocks!";
// //     std::string encryptedText = encryption.encrypt(originalText);
// //     cout << "Encrypted text length = " << encryptedText.length() << endl;
// //     std::string decryptedText = encryption.decrypt(encryptedText);
// //
// //     EXPECT_EQ(originalText, decryptedText);
// // }
//
//
// std::string longLongToString(long long value) {
//     std::string result(sizeof(long long int), '\0');
//     std::memcpy(&result[0], &value, sizeof(long long int));
//     return result;
// }
//
// long long stringToLongLong(const std::string &str) {
//     if (str.size() != sizeof(long long int)) {
//         throw std::invalid_argument("String size does not match long long int size");
//     }
//     long long value;
//     std::memcpy(&value, str.data(), sizeof(long long int));
//     return value;
// }
//
// TEST(EncryptionTest, EncryptDecryptLongData_II) {
//     Encryption encryption(TEST_KEY);
//
//     // long long int originalValue = 1234567890123456789LL;
//     long long int originalValue = 1LL;
//     std::string binaryString = longLongToString(originalValue);
//
//     std::cout << "Binary string: " << binaryString << std::endl;
//     std::cout << "Binary string length: " << binaryString.size() << std::endl;
//
//     long long int recoveredValue = stringToLongLong(binaryString);
//     std::cout << "Recovered value: " << recoveredValue << std::endl;
//
//     std::string originalText = binaryString;
//     std::string encryptedText = encryption.encrypt(originalText);
//     cout << "Encrypted string: " << encryptedText << endl;
//     cout << "Encrypted text length = " << encryptedText.length() << endl;
//     std::string decryptedText = encryption.decrypt(encryptedText);
//
//     EXPECT_EQ(originalText, decryptedText);
// }
//
// TEST(EncryptionTest, InvalidKeyLength) {
//     EXPECT_THROW(Encryption encryption("shortkey"), std::runtime_error);  // Key less than 16 bytes
//     EXPECT_THROW(Encryption encryption("thiskeyiswaytoolongforaes128bit"), std::runtime_error);  // Key more than 16 bytes
// }
//
// TEST(EncryptionTest, DecryptInvalidEncryptedData) {
//     Encryption encryption(TEST_KEY);
//
//     std::string invalidData = "notanencrypteddata";  // Not valid encrypted data
//     EXPECT_THROW(encryption.decrypt(invalidData), std::runtime_error);
// }
//
// TEST(EncryptionTest, EncryptDecryptSpecialCharacters) {
//     Encryption encryption(TEST_KEY, 64);
//
//     std::string originalText = "This text has special chars! @#$%^&*()_+[]{}|;:',.<>?";
//     std::string encryptedText = encryption.encrypt(originalText);
//     std::string decryptedText = encryption.decrypt(encryptedText);
//
//     EXPECT_EQ(originalText, decryptedText);
// }
