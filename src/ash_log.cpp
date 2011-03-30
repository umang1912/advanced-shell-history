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


DEFINE_string(alert, 'a', 0, "A message to display to the user.");
DEFINE_string(command, 'c', 0, "The command to log.");
DEFINE_int(command_exit, 'e', 0, "The exit code of the command to log.");
DEFINE_string(command_pipe_status, 'p', 0, "The pipe states of the command to log.");
DEFINE_int(command_start, 's', 0, "The timestamp when the command started.");
DEFINE_int(command_finish, 'f', 0, "The timestamp when the command stopped.");
DEFINE_int(command_number, 'n', 0, "The command number according to shell history.");
DEFINE_int(exit, 'x', 0, "The exit code to use when exiting this program.");
DEFINE_flag(get_session_id, 'S', "Begins a session and / or emits the session ID.");
DEFINE_flag(end_session, 'E', "Ends the current session.");


using namespace ash;
using namespace std;


int main(int argc, char ** argv) {
  Flag::parse(&argc, &argv, true);
  // TODO(cpa): check the remaining args to make sure something else wasn't there
  // TODO(cpa): make sure at least one arg was given - show usage if no args
  DBObject::register_table(Session::get_create_table());
  DBObject::register_table(Command::get_create_table());

  string db_file = string(getenv("HOME")) + "/.history.db";

  if (!FLAGS_alert.empty()) {
    cerr << FLAGS_alert << endl;
  }

  if (FLAGS_get_session_id) {
    Database db = Database(db_file.c_str());
    stringstream ss;
    char * id = getenv(ASH_SESSION_ID);
    if (id) {
      ss << "select count(*) from sessions where id = " << id << ";";
      if (db.select_int(ss.str().c_str()) == 0) {
        cerr << "ERROR: session_id(" << id << ") not found, creating new session." << endl << ss.str() << endl;
        id = 0;
      }
      ss.str("");

      // TODO(cpa): check that the session is still ongoing - basic sanity check

    }

    if (id) {
      cout << id << endl;
    } else {
      Session session;
      cout << db.select_int(session.get_sql().c_str()) << endl;
    }
  }

  if (!FLAGS_command.empty()) {
    // TODO(cpa): make sure all the required flags are present.
    Database db = Database(db_file.c_str());
    Command com(FLAGS_command, FLAGS_command_exit, FLAGS_command_start,
      FLAGS_command_finish, FLAGS_command_number, FLAGS_command_pipe_status);
    db.exec(com.get_sql().c_str());
  }

  if (FLAGS_end_session) {
    Session session;
    Database db = Database(db_file.c_str());
    db.exec(session.get_close_session_sql().c_str());
    // TODO(cpa): alert if there is currently no session ID in the environment or in the DB
  }

  // TODO(cpa): warn if FLAGS_exit is > 255
  return FLAGS_exit;
}
