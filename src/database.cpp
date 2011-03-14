#include <sys/stat.h> /* for stat */
#include <stdio.h>    /* for fopen */
#include <stdlib.h>
#include <iostream>

#include "database.hpp"
#include "sqlite3.h"
// TODO(cpa): create an include file that holds all the queries I want to use.


using namespace std;


int MyCallback(void * foo, int argc, char ** argv, char ** column_names) {
  // TODO(cpa): rename this to something better, think about incorporating as a class member or something
  *((int*)foo) = atoi(argv[0]);
  return 0;
}


Database::Database(const char * filename)
  :db_filename(filename), db(0)
{
  struct stat file;
  // Test that the file exists, if not, create it.
  if (stat(db_filename, &file)) {
    FILE * created_file = fopen(db_filename, "w");
    if (!created_file) {
      cout << "ERROR: failed to create new file: " << db_filename << endl;
      exit(1);
    }
    fclose(created_file);
  }

  // Open the DB, if failure, abort.
  if (sqlite3_open(db_filename, &db)) {
    cout << "Failed to open " << db_filename << "\nError: " << sqlite3_errmsg(db) << endl;
    exit(1);
  }

  // Init the DB if it is missing the main tables.
  int result = -1;
  char query[] = "select count(*) from sqlite_master where tbl_name in (\"sessions\", \"commands\");";
  // TODO(cpa): need to save the return value of exec to check for failures?
  sqlite3_exec(db, query, MyCallback, &result, 0);
  if (result != 2) {
    init_db();
  }
}


Database::~Database() {
  if (db) {
    sqlite3_close(db);
    db = 0;
  }
}


void Database::init_db() {
  // TODO(cpa): initialize the database, create the schema, etc.
  cout << "SUCCESS" << endl;
}


int Database::get_session_id() {
  // First check the environment, return that if present.
  char * id = getenv("AH_SESSION_ID");
  if (id) {
    // TODO(cpa): check that the session exists in the DB and is still ongoing - basic sanity checks
    // TODO(cpa): set id to 0 if sanity checks fail, forcing a new session creation.
    return atoi(id);
  }

  // Create a new session for this user.
  // TODO(cpa): if the current session_id exists, emit it
  // TODO(cpa): else create a new session_id and emit it

  return -1;
}

