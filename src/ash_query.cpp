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

#include "ash_query.hpp"

#include "command.hpp"
#include "config.hpp"
#include "database.hpp"
#include "flags.hpp"
#include "formatter.hpp"
#include "logger.hpp"
#include "queries.hpp"
#include "session.hpp"

#include <iostream>

using namespace ash;
using namespace std;


// TODO(cpa): add a flag to include the query with the result set
// TODO(cpa): add a flag for suppressing column headings
// TODO(cpa): add some formatting options (csv, tab delim, grouped, etc).

//DEFINE_int(limit, 'l', 0, "Limit the number of rows returned.");
DEFINE_string(database, 'd', 0, "A history database to query.");
DEFINE_string(query, 'q', 0, "The name of the saved query to execute.");

DEFINE_flag(list, 'L', "Display all saved queries.");
DEFINE_flag(version, 0, "Show the version and exit.");


// TODO(cpa): also list the saved queries in the --help output...


int execute(const string & query_name) {
  Config & config = Config::instance();

  // Get the filename backing the database to query.
  string db_file(FLAGS_database);
  if (db_file == "") {
    if (config.get_string("HISTORY_DB") == "") {
      cerr << "Expected either --database or ASH_HISTORY_DB to be defined."
           << endl;
      return 1;
    }
    db_file = config.get_string("HISTORY_DB");
  }

  // Prepare the DB for reading.
  Session::register_table();
  Command::register_table();
  Database db(db_file);

  // Get the query SQL using the name, making sure it is found.
  string sql = Queries::get_sql(query_name);
  if (sql == "") {
    cerr << "Unknown query name: '" << query_name << "'" << endl;
    return 1;
  }

  // TODO(cpa): execute the SQL and retain the ResultSet Object.
  // TODO(cpa): create a Formatter to format the ResultSet.
  // TODO(cpa): Insert the formatted results into cout and errors to (cerr).
  return -1;
}


int main(int argc, char ** argv) {
  // Load the config from the environment.
  Config & config = Config::instance();

  if (argc == 1) {  // No flags.
    if (config.sets("DEFAULT_QUERY")) {
      return execute(config.get_string("DEFAULT_QUERY"));
    }
    if (!config.sets("HIDE_USEAGE_FOR_NO_ARGS")) {
      Flag::show_help(cerr);
    }
    return 1;
  }

  // Parse the flags, removing from argv and argc.
  Flag::parse(&argc, &argv, true);

  // Abort if unrecognized flags were used on the command line.
  if (argc != 0 && !config.sets("IGNORE_UNKNOWN_FLAGS")) {
    cerr << "unrecognized flag: " << argv[0] << endl;
    Flag::show_help(cerr);
    return 1;
  }

  // Display version, if that's all that was wanted.
  if (FLAGS_version) {
    cout << ASH_VERSION << endl;
    return 0;
  }

  // Display available query names, if requested.
  if (FLAGS_list) {
//    Queries::show_help(cout);
cout << "TODO(cpa): display the saved queries HERE..." << endl;
    return 0;
  }

  // Execute the requested command.
  return execute(FLAGS_query);
}
