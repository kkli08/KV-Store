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
    // retrieve all SST index
    index->getAllSSTs();
  }

  void API::Close(){
    check_if_open();
    std::cout << "Closing database " << std::endl;
    // The close command should transform whatever is in the current Memtable into an SST
    FlushSSTInfo* info = memtable->flushToDisk();
    /*
     *  Insert file into SSTIndex
     *
     */
    if(info->largest_key >= info->smallest_key) {
      // non-empty SST file
      index->addSST(info->fileName, info->smallest_key, info->smallest_key);
    }
    // set flag
    is_open = false;
    // clean up memory
    delete info;
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
    index->set_path(_path);

    // Construct the full path to the "Index.sst" file
    fs::path indexFilePath = path / "Index.sst";

    // Check if the "Index.sst" file exists
    if (!fs::exists(indexFilePath)) {
      // If the file does not exist, create an empty "Index.sst" file
      std::ofstream outfile(indexFilePath);
      if (!outfile.is_open()) {
        throw std::runtime_error("Failed to create Index.sst file at: " + indexFilePath.string());
      }
      outfile.close();  // Close the file after creation
      std::cout << "Created new Index.sst file at: " << indexFilePath << std::endl;
    }
  }

  void API::Put(long long key, long long value) {
    // Put method
    // check if db is open
    check_if_open();

    // The key should not be -1
    if (key == -1) {
      cerr << "Key should not be -1, try use other value as the key." << std::endl;
      return;
    }

    FlushSSTInfo* info = memtable->put(key, value);

    if (info != nullptr) {
      // flush happens and safe access info attribute
      // Debug Purpose :-D
      // cout << "\n>>>>>> Ready for flushing" << endl;
      // cout << info->fileName << "'s smallest key: " << info->smallest_key << " and largest key: " << info->largest_key << endl;
      if(info->largest_key >= info->smallest_key) {
        // non-empty SST file
        index->addSST(info->fileName, info->smallest_key, info->largest_key);
      }
    }

  }

  long long API::Get(long long key) {
    // Get method
    // check if db is open
    check_if_open();

    long long value = memtable->get(key);
    // check memtable
    if (value != -1) {
      return value;
    }

    // check in SSTs.
    value = index->Search(key);
    if ( value != -1) {
      return value;
    }

    return -1;
  }

  // Debug helper function
  void API::IndexCheck() {
    cout << "\n-->Inside API::IndexCheck()" << endl;
    for(auto& info : index->getSSTsIndex()) {
      cout << info->filename << "'s smallest key: " << info->smallest_key << " and largest key: " << info->largest_key << endl;
    }
    cout << "\n-->Inside Index.sst" << endl;
    index->printSSRsInFile();
  }
}