//
// Created by Damian Li on 2024-08-26.
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "api.h"
#include "Memtable.h"


using namespace std;
int main(){
  auto MyDB = new kvdb::API();
  MyDB->Open("my_database");
  MyDB->Put(1, 100);
  MyDB->Close();

  // const long long num_pairs = 1e4 + 10;  // 1 million key-value pairs
  // // API* api = new API();
  // std::string db_name = "test_db_performance_1M";

  // // Ensure the directory exists before the test
  // if (!fs::exists(db_name)) {
  //   fs::create_directory(db_name);
  // }

  // // Open the database
  // MyDB->Open(db_name);
  //
  // // Measure the time taken to put 1 million key-value pairs
  // auto start_put = std::chrono::high_resolution_clock::now();
  // for (long long i = 1; i <= num_pairs; ++i) {
  //   MyDB->Put(i, i * 10);  // Inserting key i with value i*10
  // }

  return 0;

}