//
// Created by Damian Li on 2024-08-26.
//
#include "api.h"
#include <iostream>
#include <string>
#include <filesystem> // C++17 lib

namespace fs = std::filesystem;
namespace kvdb {
  void API::Open(string db_name){
    std::cout << "Opening database " << db_name << std::endl;

    // c++17 new feature
    // Define the path to the database directory
    fs::path db_path = db_name;
    // Check if the directory exists
    if (!fs::exists(db_path)) {
      // Directory does not exist, so create it
      if (fs::create_directory(db_path)) {
        std::cout << "Created database directory: " << db_name << std::endl;
      } else {
        std::cerr << "Failed to create directory: " << db_name << std::endl;
      }
    } else {
      std::cout << "Existed database directory: " << db_name << std::endl;
    }
    // set api attribute fs::path
    set_path(db_path);

    // set flag
    is_open = true;
  }

  void API::Close(){
    check_if_open();
    std::cout << "Closing database " << std::endl;
    // The close command should transform whatever is in the current Memtable into an SST
    memtable->flushToDisk();
    // set flag
    is_open = false;
    // clean up memory
    cleanup();
  }

  void API::cleanup() {
    // Check if memtable is not null
    if (memtable) {
      delete memtable;  // Delete the dynamically allocated memtable
      memtable = nullptr;  // Set pointer to nullptr to avoid dangling pointer
    }
    std::cout << "Cleanup completed." << std::endl;
  }

  API::~API() {
    // Check if memtable is not null
    if (memtable) {
      delete memtable;  // Delete the dynamically allocated memtable
      memtable = nullptr;  // Set pointer to nullptr to avoid dangling pointer
    }
  }
  void API::set_path(fs::path _path) {
    path = _path;
    memtable->set_path(_path);
  }

  void API::Put(long long key, long long value) {
    // Put method
    // check if db is open
    check_if_open();

    memtable->put(key, value);
  }

  long long API::Get(long long key) {
    // Get method
    // check if db is open
    check_if_open();

    // check memtable
    if (memtable->get(key) != -1) {
      return memtable->get(key);
    }

    // check in SSTs.


    return -1;
  }
}