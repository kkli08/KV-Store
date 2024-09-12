cmake_minimum_required(VERSION 3.10)  # Specify the minimum version of CMake

# Project name
project(KVStore)

# Set the required C++ standard and features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the required protobuf package
find_package(Protobuf REQUIRED)

# Specify the location of the generated .pb.cc and .pb.h files
set(PROTO_SRCS ${PROJECT_SOURCE_DIR}/kv/KeyValue.pb.cc)
set(PROTO_HDRS ${PROJECT_SOURCE_DIR}/kv/KeyValue.pb.h)

# Include GoogleTest via FetchContent
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find OpenSSL library
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Enable testing
enable_testing()

# ---- TEST TARGET ----
add_executable(runTests
        # Test files
        tests/binarytree_tests.cpp
        tests/redblacktree_unittest.cpp
        tests/api_unittest.cpp
        tests/memtable_unittest.cpp
        tests/sstIndex_unittest.cpp
        tests/encryption_unittest.cpp
        tests/kvpair_unittest.cpp
        tests/file_manager_unittest.cpp

        # Source files required by tests
        tree/BinaryTree.cpp
        tree/TreeNode.cpp
        tree/RedBlackTree.cpp
        api/api.cpp
        memtable/Memtable.cpp
        SSTIndex/SSTIndex.cpp
        AesEncryption/Encryption.h
        kv/KeyValue.cpp
        FileManager/FileManager.cpp
        ${PROTO_SRCS}
)

# Link Google Test and OpenSSL to the test executable
target_link_libraries(runTests gtest gtest_main OpenSSL::Crypto ${Protobuf_LIBRARIES})

# Register tests with CTest
add_test(NAME runTests COMMAND runTests)


# ---- MAIN TARGET ----
# List of source files
set(SOURCE_FILES
        main.cpp
        api/api.cpp
        memtable/Memtable.cpp
        tree/BinaryTree.cpp
        tree/TreeNode.cpp
        tree/RedBlackTree.cpp
        SSTIndex/SSTIndex.cpp
        kv/KeyValue.cpp
        FileManager/FileManager.cpp
)

# Add the executable
add_executable(main ${SOURCE_FILES} ${PROTO_SRCS})
target_link_libraries(main ${Protobuf_LIBRARIES})

# Include directories (header files)
include_directories(
        ${PROJECT_SOURCE_DIR}/api
        ${PROJECT_SOURCE_DIR}/memtable
        ${PROJECT_SOURCE_DIR}/tree
        ${PROJECT_SOURCE_DIR}/SSTIndex
        ${PROJECT_SOURCE_DIR}/AesEncryption
        ${PROJECT_SOURCE_DIR}/kv
        ${PROJECT_SOURCE_DIR}/FileManager
)

