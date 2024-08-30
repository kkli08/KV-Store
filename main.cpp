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

  // Memtable* memtable = new Memtable();
  // fs::path path = fs::path("test_db");
  // memtable->set_path(path);
  // memtable->put(10, 100);
  //
  // // Perform flush and get the filename used
  // std::string filename = memtable->int_flush();

  return 0;

}