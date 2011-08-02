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

#include "database.hpp"
#include "config.hpp"
#include "logger.hpp"

#include <sys/stat.h>  /* for stat */
#include <stdio.h>     /* for fopen */
#include <stdlib.h>
#include <unistd.h>    /* for usleep */

#include <iostream>
#include <sstream>

// This hack silences a warning when compiling on a 64 bit platform with
// -ansi and -pedantic flags enabled.
// The original g++ complaint is that 'long long' is deprecated.
#ifdef __LP64__
#define SQLITE_INT64_TYPE long int
#endif
#include "sqlite3.h"

using namespace ash;
using namespace std;


/**
 * 
 */
Database::Database(const string & filename)
  :db_filename(filename), db(0)
{
  struct stat file;
  // TODO(cpa): load the history file name from Config.
  // Test that the history file exists, if not, create it.
  if (stat(db_filename.c_str(), &file)) {
    FILE * created_file = fopen(db_filename.c_str(), "w+e");
    if (!created_file) {
      LOG(FATAL) << "ERROR: failed to create new DB file: " << db_filename << endl;
      exit(1);  // TODO(cpa): implement LOG(FATAL) so this isn't necessary.
    }
    fclose(created_file);
  }

  // Open the DB, if failure, abort.
  if (sqlite3_open(db_filename.c_str(), &db)) {
    cerr << "Failed to open " << db_filename << "\nError: " << sqlite3_errmsg(db) << endl;
    exit(1);
  }

  // Init the DB if it is missing the main tables.
  char query[] =
      "select count(*) "
      "from sqlite_master "
      "where tbl_name in ('sessions', 'commands');";

  int defined_tables = select_int(query);
  if (defined_tables == 2) return;  // Normal case.

  // Initialize the DB if it's not already set up.
  init_db();

  // Log a warning if there was an unexpected number of tables.
  if (defined_tables > 2) {
    LOG(DEBUG) << "Expected only 2 tables to be defined, found "
        << defined_tables << " instead.";
  }
}


/**
 * 
 */
Database::~Database() {
  if (db) {
    sqlite3_close(db);
    db = 0;
  }
}


/**
 * 
 */
int NOOPCallback(void * ignored, int rows, char ** cols, char ** col_names) {
  // Nothing to do in this callback.
  return 0;
}


/**
 * 
 */
void Database::init_db() {
  const string & create_tables = DBObject::get_create_tables();
  sqlite3_exec(db, create_tables.c_str(), NOOPCallback, 0, 0);
}


// Callback used in Database::select_int
int SelectInt(void * result, int rows, char ** cols, char ** column_names) {
  *((int*) result) = atoi(cols[0]);
  return 0;
}


// Convenience type for the callback invoked after query execution.
typedef int (*callback)(void*,int,char**,char**);


/**
 * RECURSIVE: Tries to execute the argument query, retrying a set number of
 * times with a random sleep between each attempt.
 */
bool retry_execute(const string & query, sqlite3 * db, callback c,
                   void * result, int retries, int sleep_ms, int rand_ms)
{
  if (retries == 0)
    return false;  // base case.
  // TODO(cpa): sleep a random amount of ms.
  if (sqlite3_exec(db, query.c_str(), c, result, 0)) {
    // Have we run out of attempts?
    if (retries == 1) {
      LOG(ERROR) << "Failed to execute: " << sqlite3_errmsg(db) << '\n'
          << query << endl;
    }
    return retry_execute(query, db, c, result, retries - 1, sleep_ms, rand_ms);
  }
  return true;
}


/**
 * Retries (if necessary) up to as many times as configured to execute the
 * argument query.
 */
bool executed(const string & query, sqlite3 * db, callback c, void * result) {
  // If the first query fails, retry as many times as configured.
  if (sqlite3_exec(db, query.c_str(), c, result, 0)) {
    Config & config = Config::instance();

    int retries = config.get_int("DB_MAX_RETRIES", -1);
    if (retries < 0) retries = 5;

    int fail_ms = config.get_int("DB_FAIL_TIMEOUT", -1);
    if (fail_ms < 0) fail_ms = 0;

    int random_ms = config.get_int("DB_FAIL_RANDOM_TIMEOUT", -1);
    if (random_ms < 0) random_ms = 0;

    return retry_execute(query, db, c, result, retries, fail_ms, random_ms);
  }
  return true;
}


/**
 * Executes a query expecting a single int return value.
 */
int Database::select_int(const string & query) const {
  int result = -1;
  if (!executed(query, db, SelectInt, &result)) {
    exit(1);
  }
  return result;
}


/**
 * 
 */
void Database::exec(const string & query) const {
  if (sqlite3_exec(db, query.c_str(), NOOPCallback, 0, 0)) {
    cerr << sqlite3_errmsg(db) << endl << query << endl;
    exit(1);
  }
}


/*********
 * DB_OBJECT CODE BELOW:
 ****/


/**
 * 
 */
list<string> DBObject::create_tables;


/**
 * 
 */
const string DBObject::get_create_tables() {
  stringstream ss;
  ss << "PRAGMA foreign_keys=OFF;"
     << "BEGIN TRANSACTION;";
  typedef list<string>::iterator it;
  for (it i = create_tables.begin(), e = create_tables.end(); i != e; ++i) {
    ss << *i << "; ";
  }
  ss << "COMMIT;";
  return ss.str();
}


/**
 * 
 */
void DBObject::register_table(const string & create_statement) {
  create_tables.push_back(create_statement);
}


/**
 * 
 */
const string DBObject::quote(const char * value) {
  return value ? quote(string(value)) : "null";
}


/**
 * 
 */
const string DBObject::quote(const string & in) {
  if (in.empty()) return "null";
  string out = "'";
  char c;
  for (string::const_iterator i = in.begin(), e = in.end(); i != e; ++i) {
    c = *i;
    if (isprint(c)) out.push_back(c);
    if (c == '\'') out.push_back('\'');
  }
  out.push_back('\'');
  return out;
}


/**
 * 
 */
DBObject::DBObject() {
  // Nothing to do here.
}


/**
 * 
 */
DBObject::~DBObject() {
  // Nothing to do here.
}


/**
 * 
 */
const string DBObject::get_sql() const {
  typedef map<string, string>::const_iterator c_iter;

  stringstream ss;
  ss << "INSERT INTO " << get_name() << " (";

  // Insert the column names.
  for (c_iter i = values.begin(), e = values.end(); i != e; ) {
    ss << (i -> first);
    if (++i == e) break;
    ss << ", ";
  }

  ss << ") VALUES (";
  // Insert the values.
  for (c_iter i = values.begin(), e = values.end(); i != e; ) {
    ss << (i -> second);
    if (++i == e) break;
    ss << ", ";
  }

  ss << "); ";

  return ss.str();
}
