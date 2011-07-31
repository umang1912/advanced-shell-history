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

#include "ash_log.hpp"

#include "command.hpp"
#include "config.hpp"
#include "flags.hpp"
#include "logger.hpp"
#include "session.hpp"
#include "unix.hpp"

#include <stdlib.h>  /* for getenv, exit */

#include <iostream>  /* for cerr, cout, endl */
#include <sstream>   /* for stringstream */


DEFINE_string(alert, 'a', 0, "A message to display to the user.");
DEFINE_string(command, 'c', 0, "The command to log.");
DEFINE_int(command_exit, 'e', 0, "The exit code of the command to log.");
DEFINE_string(command_pipe_status, 'p', 0, "The pipe states of the command to log.");
DEFINE_int(command_start, 's', 0, "The timestamp when the command started.");
DEFINE_int(command_finish, 'f', 0, "The timestamp when the command stopped.");
DEFINE_int(command_number, 'n', 0, "The command number according to shell history.");
DEFINE_int(exit, 'x', 0, "The exit code to use when exiting this program.");

DEFINE_flag(version, 'V', "Prints the version and exits.");
DEFINE_flag(get_session_id, 'S', "Begins a session and / or emits the session ID.");
DEFINE_flag(end_session, 'E', "Ends the current session.");


using namespace ash;
using namespace std;


int main(int argc, char ** argv) {
  // Load the config from the environment.
  Config & config = Config::instance();

  // Set up the logger.
  if (config.has("LOG_FILE"))
    Logger::set_log_file(config.get_cstring("LOG_FILE"));
  if (config.has("LOG_LEVEL"))
    Logger::set_log_level(Logger::parse(config.get_cstring("LOG_LEVEL")));

  // Log the complete command, if debugging.
  LOG(DEBUG) << "argv = '[0]='" << argv[0] << "'";
  for (int i = 1; i < argc; ++i)
    LOG(DEBUG) << ",[" << i << "]='" << argv[i] << "'";
  LOG(DEBUG) << endl;

  // Show usage if executed with no args.
  if (argc == 1 && !config.sets("HIDE_USAGE_FOR_NO_ARGS")) {
    Flag::parse(&argc, &argv, true);
    Flag::show_help(cerr);
    exit(1);
  }

  // Parse the flags.
  Flag::parse(&argc, &argv, true);

  // Abort if unrecognized flags were used on the command line.
  if (argc != 0 && !config.sets("IGNORE_UNKNOWN_FLAGS")) {
    LOG(ERROR) << "unrecognized flag: " << argv[0] << endl;
    Flag::show_help(cerr);
    exit(1);
  }

  // Initialize the database backend.
  DBObject::register_table(Session::get_create_table());
  DBObject::register_table(Command::get_create_table());

  // TODO(cpa): get this from a config variable?
  string db_file = string(getenv("HOME")) + "/.history.db";

  //
  // Process the command flags.
  //

  // Display the version and stop: -V
  if (FLAGS_version) {
    cout << ASH_VERSION << endl;
    return 0;
  }

  // Display an alert: -a 'My alert'
  if (!FLAGS_alert.empty()) {
    cerr << FLAGS_alert << endl;
  }

  // Emit the current session number, inserting one if none exists: -S
  if (FLAGS_get_session_id) {
    Database db = Database(db_file);
    stringstream ss;
    char * id = getenv(ASH_SESSION_ID);
    if (id) {
      ss << "select count(*) from sessions where id = " << id
         << " and duration is null;";
      if (db.select_int(ss.str()) == 0) {
        cerr << "ERROR: session_id(" << id << ") not found, "
             << "creating new session." << endl << ss.str() << endl;
        id = 0;
      }
      ss.str("");
    }

    if (id) {
      cout << id << endl;
    } else {
      Session session;
      cout << db.select_int(session.get_sql()) << endl;
    }
  }

  // TODO(cpa): determine the command binary / alias / function.
  // TODO(cpa): if none, and the user requested to not store non-commands, skip.

  // Insert a command into the DB if there's a command to insert.
  const bool command_flag_used = !FLAGS_command.empty()
    || FLAGS_command_exit
    || !FLAGS_command_pipe_status.empty()
    || FLAGS_command_start
    || FLAGS_command_finish
    || FLAGS_command_number;

  if (command_flag_used) {
    Database db = Database(db_file);
    Command com(FLAGS_command, FLAGS_command_exit, FLAGS_command_start,
      FLAGS_command_finish, FLAGS_command_number, FLAGS_command_pipe_status);
    db.exec(com.get_sql());
  }

  // End the current session in the DB: -E
  if (FLAGS_end_session) {
    Session session;
    Database db = Database(db_file);
    db.exec(session.get_close_session_sql());
    // TODO(cpa): LOG(ERROR) if there is currently no session ID in the environment or in the DB.
  }

  // Set the exit code to match what the previous command exited: -e 123
  return FLAGS_exit;
}
