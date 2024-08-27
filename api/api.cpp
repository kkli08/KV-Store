//
// Created by Damian Li on 2024-08-26.
//
#include "api.h"
#include <iostream>

using namespace std;

void API::Open(string db_name){
  cout << "Opening database " << db_name << endl;
  return;
}

void API::Close(){
  cout << "Closing database " << endl;
  return;
}