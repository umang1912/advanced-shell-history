/*
   Copyright 2011 Carl Anderson

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <sys/stat.h>  /* for stat */
#include <stdio.h>     /* for fopen */
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "ash_log.hpp"
#include "sqlite3.h"

// TODO(cpa): instead of exiting, throw an exception where i'm currently exiting.
// TODO(cpa): create an include file that holds all the queries I want to use???


using namespace ash;
using namespace std;


Database::Database(const char * filename)
  :db_filename(filename), db(0)
{
  struct stat file;
  // Test that the file exists, if not, create it.
  if (stat(db_filename, &file)) {
    FILE * created_file = fopen(db_filename, "w+e");
    if (!created_file) {
      cerr << "ERROR: failed to create new file: " << db_filename << endl;
      exit(1);
    }
    fclose(created_file);
  }

  // Open the DB, if failure, abort.
  if (sqlite3_open(db_filename, &db)) {
    cerr << "Failed to open " << db_filename << "\nError: " << sqlite3_errmsg(db) << endl;
    exit(1);
  }

  // Init the DB if it is missing the main tables.
// TODO(cpa): this query can be built from the names of classes extending DBObject...
  char query[] = "select count(*) from sqlite_master where tbl_name in (\"sessions\", \"commands\");";
  if (select_int(query) != 2) {
    init_db();
  }
  // TODO(cpa): maybe emit a warning if there are more tables than just the two...
}


Database::~Database() {
  if (db) {
    sqlite3_close(db);
    db = 0;
  }
}


int CreateTables(void * ignored, int rows, char ** cols, char ** col_names) {
  return 0;
}


void Database::init_db() {
  sqlite3_exec(db, DBObject::get_create_tables().c_str(), CreateTables, 0, 0);
}


// Callback used in Database::select_int
int SelectInt(void * result, int rows, char ** cols, char ** column_names) {
  *((int*) result) = atoi(cols[0]);
  return 0;
}


int Database::select_int(const char * query) const {
  int result = -1;
  if (sqlite3_exec(db, query, SelectInt, &result, 0)) {
    cerr << sqlite3_errmsg(db) << endl << query << endl;
    exit(1);
  }
  return result;
}


void Database::exec(const char * query) const {
  if (sqlite3_exec(db, query, CreateTables, 0, 0)) {
    cerr << sqlite3_errmsg(db) << endl << query << endl;
    exit(1);
  }
}
