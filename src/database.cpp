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

#include <errno.h>     /* for errno */
#include <sys/stat.h>  /* for stat */
#include <sys/time.h>  /* for timeval */
#include <stdio.h>     /* for fopen */
#include <stdlib.h>    /* for rand, srand */
#include <string.h>    /* for strerror */
#include <time.h>      /* for time */

#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

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
  ss << "select count(*) as table_count "
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
  char * error = 0;
  if (sqlite3_exec(db, create_tables.c_str(), NOOPCallback, 0, &error)) {
    cerr << "Failed to create tables:\n"
         << create_tables
         << "Error:\n"
         << error << endl;
    sqlite3_free(error);
    error = 0;
  }
}


/**
 * Callback used in Database::select_int.
 */
int SelectInt(void * result, int rows, char ** cols, char ** column_names) {
  *((int*) result) = atoi(cols[0]);
  return 0;
}


// Convenience type for the callback invoked after query execution.
typedef int (*callback)(void*,int,char**,char**);


/**
 * This method does a lot to make sure a good sleep is had.  It checks the
 * configured sleep settings: ASH_CFG_DB_FAIL_RANDOM_TIMEOUT and
 * ASH_CFG_DB_FAIL_TIMEOUT.  It also tries to make sure that the specified sleep
 * amount is honored.
 */
void ash_sleep() {
  Config & config = Config::instance();

  int retries = config.get_int("DB_MAX_RETRIES", -1);
  if (retries < 0) retries = 5;

  int fail_ms = config.get_int("DB_FAIL_TIMEOUT", -1);
  if (fail_ms < 0) fail_ms = 0;

  int random_ms = config.get_int("DB_FAIL_RANDOM_TIMEOUT", -1);
  if (random_ms < 0) random_ms = 0;

  // Sleep a number amount of ms within the parameters.
  long int ms = fail_ms;
  if (random_ms > 0) {
    // Randomize with both the time and the PID.  Since the database being
    // locked is likely a collission with another _ash_log - time along will
    // randomize both processes to generate the same random numbers.
    srand(time(0) ^ getpid());
    ms += rand() % random_ms;
  }
  LOG(INFO) << "Sleeping " << ms << " milliseconds.";
  if (ms == 0) return;

  // Measure a timestamp to count how long we actually slept.
  struct timespec before_ts, after_ts;
  if (clock_gettime(CLOCK_MONOTONIC, &before_ts)) {
    perror("clock_gettime failed");
  }

  unsigned int sleep_attempts = ms;
  struct timespec to_sleep, remaining;
  to_sleep.tv_sec = ms / 1000;
  to_sleep.tv_nsec = (ms % 1000) * 1000000L;

  while (to_sleep.tv_sec || to_sleep.tv_nsec) {
    // Sanity check to make sure we aren't looping infinitely.
    if (sleep_attempts == 0) {
      LOG(WARNING) << "Sleep break triggered.  "
                   << "Maybe CLOCK_MONOTONIC doesn't work on your system.";
      break;  // safety
    }
    --sleep_attempts;

    // Sleep and verify the return code.
    int rval = clock_nanosleep(CLOCK_MONOTONIC, 0, &to_sleep, &remaining);
    switch (rval) {
      case -1: 
        LOG(ERROR) << "Failed to clock_nanosleep: " << strerror(errno);
        continue;
      case 0: break;
      default:
        LOG(ERROR) << "Unexpected rval from clock_nanosleep: " << rval;
    }

    // If there is any time remaining, prepare to sleep again.
    to_sleep.tv_sec = remaining.tv_sec;
    to_sleep.tv_nsec = remaining.tv_nsec;
  }

  // Check to see how long has passed since ash_sleep began.
  if (clock_gettime(CLOCK_MONOTONIC, &after_ts)) {
    perror("clock_gettime failed");
  }
  
  long int slept =
    (after_ts.tv_sec - before_ts.tv_sec) * 1000 +
    (after_ts.tv_nsec - before_ts.tv_nsec) / 1000000L;

  LOG(INFO) << "Slept " << slept << " milliseconds.";

  if (ms > slept) {
    // This should never happen because of the while loop above.
    LOG(ERROR) << "Failed to sleep " << ms << " ms between failures.";
  }
  
  if (slept > (ms + 1) * 100) {
    LOG(ERROR) << "Major clock problem detected.  Requested sleep of: " << ms
               << " ms took " << slept << " ms to complete.";
  }
}


/**
 * RECURSIVE: Tries to execute the argument query, retrying a set number of
 * times with a random sleep between each attempt.
 */
bool retry_execute(const string & query, sqlite3 * db, callback c,
                   void * result, int retries)
{
  // TODO(cpa): deprecate this method in favor of exec
  if (retries == 0)
    return false;  // base case.

  // sleep a random amount of ms.
  ash_sleep();

  if (sqlite3_exec(db, query.c_str(), c, result, 0)) {
    // Have we run out of attempts?
    if (retries == 1) {
      LOG(ERROR) << "Failed to execute: " << sqlite3_errmsg(db) << '\n'
          << query << endl;
    }
    return retry_execute(query, db, c, result, retries - 1);
  }
  return true;
}


/**
 * Retries (if necessary) up to as many times as configured to execute the
 * argument query.
 */
bool executed(const string & query, sqlite3 * db, callback c, void * result) {
  // TODO(cpa): deprecate this method in favor of exec
  // If the first query fails, retry as many times as configured.
  if (sqlite3_exec(db, query.c_str(), c, result, 0)) {
    Config & config = Config::instance();

    int retries = config.get_int("DB_MAX_RETRIES", -1);
    if (retries < 0) retries = 5;

    return retry_execute(query, db, c, result, retries);
  }
  return true;
}


/**
 * Executes a query expecting a single int return value.
 */
int Database::select_int(const string & query) const {
/*
WHY U NO WORK?!?!?!
  ResultSet * rs = exec(query);
  if (!rs) LOG(FATAL) << "Expected a resulting int from query: " << query;
  const char * rval = rs -> data[0][0].c_str();
  if (!rval) LOG(FATAL) << "Expected a non-null value from query: " << query;
  return atoi(rval);
//*/
  // TODO(cpa): deprecate this method in favor of exec
  int result = -1;
  if (!executed(query, db, SelectInt, &result)) {
    LOG(FATAL) << "Failed to select an int from: " << query;
  }
  return result;
// */
}


/**
 * Inserts the DBObject, returning the new ROWID.
 */
long int Database::insert(DBObject * object) const {
  if (!object) return 0;
  exec(object -> get_sql());
  return sqlite3_last_insert_rowid(db);
}


/**
 * Execute a query or abort the program with the DB error message.
 */
ResultSet * Database::exec(const string & query) const {
  // Halt if the query doesn't even parse.
  sqlite3_stmt * ps = 0;
  sqlite3_prepare_v2(db, query.c_str(), query.length(), &ps, 0);
  if (!ps) {
    LOG(FATAL) << "Failed to create a prepared statement for query: '" << query
               << "'\nError:\n" << sqlite3_errmsg(db);
  }

  // Load the relevant configured values.
  Config & config = Config::instance();

  int max_retries = config.get_int("DB_MAX_RETRIES", -1);
  if (max_retries <= 0) max_retries = 5;
  int tries = max_retries + 1;

  ResultSet::HeadersType headers;
  ResultSet::DataType results;
  stringstream ss;
  unsigned int rows = 0, columns = sqlite3_column_count(ps);

  // YES, this is a GOTO target.  This is used to implement the retry logic.
  // If a query fails because of a lock, it may goto this block to retry.
try_sql:
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
          if (sqlite3_column_text(ps, c)) {
            ss << sqlite3_column_text(ps, c);
          }
          results.back().push_back(ss.str());
        }
        continue;  // for loop
      case SQLITE_CONSTRAINT:
        // Note: there is no point retrying this type of error.
        LOG(DEBUG) << "constraint violation executing: '" << query << "'";
        goto finalize;
      case SQLITE_DONE:
        goto finalize;
      case SQLITE_BUSY: {
        // Abort if we are out of attempts.
        if (tries <= 0) {
          LOG(FATAL) << "Failed to unlock db: " << sqlite3_errmsg(db)
                     << "\nGave up after " << max_retries << " failures."
                     << "\nExecuting: '" << query << "'\n";
        }

        // Sleep some random number of milliseconds.
        ash_sleep();

        // Reset the prepared statement.
        sqlite3_finalize(ps);
        ps = 0;
        if (sqlite3_prepare_v2(db, query.c_str(), query.length(), &ps, 0)) {
          LOG(FATAL) << "Prepared statement failed unexpectedly.";
        }

        // Decrement the try count and jump.
        --tries;
        LOG(WARNING) << "Database was locked, tries remaining: " << tries;
        goto try_sql;
      }
      default:
        sqlite3_finalize(ps);
        // TODO(cpa): remove this cerr line once the FATAL errors are redirected to stderr by default.
        cerr << "unknown sqlite3_step code: " << result << " executing '"
             << query << "'\nError:\n" << sqlite3_errmsg(db) << endl;
        LOG(FATAL) << "unknown sqlite3_step code: " << result
                   << " executing '" << query << "'\nError:\n"
                   << sqlite3_errmsg(db);
    }  // switch
  }  // for loop

  // Yes, another GOTO target.  This is used because there is a switch statement
  // within a for loop and it's convenient to break the loop within the switch.
finalize:
  sqlite3_finalize(ps);

  return rows ? new ResultSet(headers, results) : 0;
}


/**
 * DB_OBJECT CODE BELOW:
 */


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
 * Replaces all single-quotes with double single quotes in the output string.
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

