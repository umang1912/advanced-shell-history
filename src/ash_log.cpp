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

#include <stdlib.h>  /* for getenv, exit */

#include <iostream>  /* for cerr, cout, endl */
#include <sstream>   /* for stringstream */

#include "ash_log.hpp"
#include "command.hpp"
#include "config.hpp"
#include "flags.hpp"
#include "session.hpp"
#include "unix.hpp"


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
  // Show usage if executed with no args.
  if (argc == 1) {
    Flag::parse(&argc, &argv, true);
    Flag::show_help(cerr);
    exit(1);
  }

cerr << "DEBUG: argv = '[0]='" << argv[0] << "'";
for (int i = 1; i < argc; ++i)
  cerr << ",[" << i << "]='" << argv[i] << "'";
cerr << endl;

  Flag::parse(&argc, &argv, true);

  // Abort if unrecognized flags were used on the command line.
  if (argc != 0) {
    cerr << "unrecognized option: " << argv[0] << endl;
    Flag::show_help(cerr);
    exit(1);
  }

  // Load the config file for this executable.
  Config & config = Config::instance();
  if (config.has("DEBUG")) {
    cout << "THIS IS A TEST DEBUG MESSAGE" << endl;
  }

  DBObject::register_table(Session::get_create_table());
  DBObject::register_table(Command::get_create_table());

  string db_file = string(getenv("HOME")) + "/.history.db";

  if (!FLAGS_alert.empty()) {
    cerr << FLAGS_alert << endl;
  }

  if (FLAGS_version) {
    cout << ASH_VERSION << endl;
    return 0;
  }

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

  if (FLAGS_end_session) {
    Session session;
    Database db = Database(db_file);
    db.exec(session.get_close_session_sql());
    // TODO(cpa): alert if there is currently no session ID in the environment or in the DB
  }

  return FLAGS_exit;
}
