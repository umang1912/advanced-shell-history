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

#include <sys/stat.h>  /* for stat */
#include <stdio.h>     /* for fopen */
#include <stdlib.h>    /* for rand, srand */
#include <time.h>      /* for time */

#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include "config.hpp"
#include "logger.hpp"

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
 * A list of the registered tables names for the DB.
 */
vector<string> DBObject::table_names;


/**
 * A list of the queries that create registered tables for the DB.
 */
list<string> DBObject::create_tables;


/**
 * Initialize a ResultSet.
 */
ResultSet::ResultSet(const HeadersType & h, const DataType & d)
  : headers(h),
    data(d),
    rows(d.size()),
    columns(h.size())
{
  // Nothing to do!
}


/**
 * Create a new Database, creating a new backing file if necessary.
 */
Database::Database(const string & filename)
  : db_filename(filename), db(0)
{
  struct stat file;
  // Test that the history file exists, if not, create it.
  if (stat(db_filename.c_str(), &file)) {
    FILE * created_file = fopen(db_filename.c_str(), "w+e");
    if (!created_file) {
      LOG(FATAL) << "failed to create new DB file: " << db_filename << endl;
    }
    fclose(created_file);
  }

  // Open the DB, if failure, abort.
  if (sqlite3_open(db_filename.c_str(), &db)) {
    LOG(FATAL) << "Failed to open " << db_filename << "\nError: "
        << sqlite3_errmsg(db) << endl;
  }

  // Init the DB if it is missing the main tables.
  size_t registered = DBObject::table_names.size();

  stringstream ss;
  ss << "select count(*) "
     << "from sqlite_master "
     << "where type = 'table' and tbl_name in (";

  // List the table names registered by the code.
  if (registered > 0) ss << DBObject::quote(DBObject::table_names[0]);
  for (size_t i = 1; i < registered; ++i)
    ss << ", " << DBObject::quote(DBObject::table_names[i]);

  ss << ");";
  string query = ss.str();

  size_t defined_tables = select_int(query.c_str());
  if (defined_tables == registered) return;  // Normal case.

  // Initialize the DB if it's not already set up.
  init_db();

  // Log a warning if there was an unexpected number of tables.
  if (defined_tables > registered) {
    LOG(WARNING) << "Expected " << registered
        << " tables to be defined, found " << defined_tables << " instead.";
  }
}


/**
 * Close the Database and free internal resources.
 */
Database::~Database() {
  if (db) {
    sqlite3_close(db);
    db = 0;
  }
}


/**
 * A No-Op callback that returns 0.
 */
int NOOPCallback(void * ignored, int rows, char ** cols, char ** col_names) {
  // Nothing to do in this callback.
  return 0;
}


/**
 * Executes the create-tables query to initialize this database.
 */
void Database::init_db() {
  const string & create_tables = DBObject::get_create_tables();
  // TODO(cpa): add actual database error message.
  if (sqlite3_exec(db, create_tables.c_str(), NOOPCallback, 0, 0)) {
    cerr << "Failed to create tables for ash database." << endl;
  }
}


/**
 * Callback used in Database::select_int.
 */
int SelectInt(void * result, int rows, char ** cols, char ** column_names) {
// TODO(cpa): templatize this method somehow?
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

  // sleep a random amount of ms.
  long int ms = sleep_ms;
  if (rand_ms) {
    srand(time(0));
    ms += rand() % rand_ms;
  }
  if (ms > sqlite3_sleep(ms)) {
    LOG(WARNING) << "Failed to sleep " << ms << " ms between failed queries.";
  }

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
 * TODO(cpa): refactor code using this to use exec below.
 */
int Database::select_int(const string & query) const {
  int result = -1;
  if (!executed(query, db, SelectInt, &result)) {
    LOG(FATAL) << "Failed to select an int from: " << query;
  }
  return result;
}


/**
 * Execute a query or abort the program with the DB error message.
 */
ResultSet * Database::exec(const string & query) const {
  sqlite3_stmt * ps;
  sqlite3_prepare_v2(db, query.c_str(), query.length(), &ps, 0);
// TODO(cpa): can i detect an error in the query here?
  ResultSet::HeadersType headers;
  ResultSet::DataType results;
  stringstream ss;

  unsigned int rows = 0, columns = sqlite3_column_count(ps);
  for (rows = 0; true; ++rows) {
    int result = sqlite3_step(ps);
    switch (result) {
      // TODO(cpa): add more cases to handle errors.
      case SQLITE_ROW:
        // build the list of header names, if this is the first row fetched.
        if (headers.empty()) {
          for (size_t c = 0; c < columns; ++c) {
            ss.str("");
            ss << sqlite3_column_name(ps, c);
            headers.push_back(ss.str());
          }
        }
        // Add the row data.
        results.push_back(ResultSet::RowType());
        for (size_t c = 0; c < columns; ++c) {
          ss.str("");
          ss << sqlite3_column_text(ps, c);
          results.back().push_back(ss.str());
        }
      	continue;  // for loop
      case SQLITE_CONSTRAINT:
        LOG(DEBUG) << "constraint violation executing: " << query;
        goto finalize;
      case SQLITE_DONE:
        goto finalize;
      default:
        sqlite3_finalize(ps);
        cerr << "unknown sqlite3_step code: " << result << " executing "
             << query << endl;
        LOG(FATAL) << "unknown sqlite3_step code: " << result << " executing "
                   << query << endl;
    }
  }

finalize:
  sqlite3_finalize(ps);

  return rows ? new ResultSet(headers, results) : 0;
}



/*********
 * DB_OBJECT CODE BELOW:
 ****/


/**
 * Returns a query that creates the table schema for the DB.
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
 * Adds a create-table query to the list of create-table queries.
 */
void DBObject::register_table(const string & name, const string & sql) {
  table_names.push_back(name);
  create_tables.push_back(sql);
}


/**
 * Returns a quoted string value using a char * input.
 */
const string DBObject::quote(const char * value) {
  return value ? quote(string(value)) : "null";
}


/**
 * Returns a quoted string suitable for insertion into the DB.
 * Converts an empty string to null.  Removes unprintable characters.
 */
const string DBObject::quote(const string & in) {
  if (in.empty()) return "null";
  string out = "'";
  char c;
  for (string::const_iterator i = in.begin(), e = in.end(); i != e; ++i) {
    c = *i;
    switch (c) {
      case '\n':  // fallthrough
      case '\t':
        out.push_back(c);
        break;
      case '\'':
        out.push_back(c); // fallthrough
      default:
        if (isprint(c)) out.push_back(c);
    }
  }
  out.push_back('\'');
  return out;
}


/**
 * Construct an empty DB Object.
 */
DBObject::DBObject() {
  // Nothing to do here.
}


/**
 * REQUIRED since it was declared virtual.
 */
DBObject::~DBObject() {
  // Nothing to do here.
}


/**
 * Returns the SQL statement needed to insert a concrete instance of this
 * class.
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

