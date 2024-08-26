cmake_minimum_required(VERSION 3.10)  # Specify the minimum version of CMake

project(KVStore)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add the test executable
add_executable(runTests
        tests/binarytree_tests.cpp
        tree/BinaryTree.cpp  # Include the source file you're testing
)

# Link Google Test to the test executable
target_link_libraries(runTests gtest gtest_main)

# Enable the tests to be run with `ctest`
add_test(NAME runTests COMMAND runTests)


# Add source file
set(SOURCE
    main.cpp
        api/api.cpp
        memtable/memtable.cpp
        tree/BinaryTree.cpp
)

# Include directories (header files)
include_directories(${PROJECT_SOURCE_DIR}/api)
include_directories(${PROJECT_SOURCE_DIR}/memtable)
include_directories(${PROJECT_SOURCE_DIR}/tree)

# Add the executable by specifying the source file(s)
add_executable(main ${SOURCE})
