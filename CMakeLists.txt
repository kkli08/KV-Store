cmake_minimum_required(VERSION 3.10)  # Specify the minimum version of CMake

project(KVStore)

set(CMAKE_CXX_STANDARD 11)

# Add source file
set(SOURCE
    main.cpp
        api/api.cpp
        memtable/memtable.cpp
)

# Include directories (header files)
include_directories(${PROJECT_SOURCE_DIR}/api)
include_directories(${PROJECT_SOURCE_DIR}/memtable)

# Add the executable by specifying the source file(s)
add_executable(main ${SOURCE})
