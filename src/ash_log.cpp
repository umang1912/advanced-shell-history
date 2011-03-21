#include <getopt.h>  /* for getopt_long */
#include <iostream>  /* for cout, endl */
#include <stdlib.h>  /* for exit */

#include "ash_log.hpp"


using namespace ash;
using namespace std;


static struct option options[] = {
  {"command", 1, 0, 'c'},
  {"exit-code", 1, 0, 'e'},
  {"finish", 1, 0, 'f'},
  {"number", 1, 0, 'n'},
  {"pipe-status", 1, 0, 'p'},
  {"start", 1, 0, 's'},
  {"end-session", 0, 0, 'E'},
  {"get-session-id", 0, 0, 'S'}
};


int main(int argc, char ** argv) {
  DBObject::register_table(Session::get_create_table());
  DBObject::register_table(Command::get_create_table());

  string command, exit_code, start, finish, number, pipes;
  int c = 0;
  while (c != -1) {
    int index = 0;
    c = getopt_long(argc, argv, "c:e:f:n:p:s:S", options, &index);
    switch (c) {
      case 'c': command = optarg; break;
      case 'e': exit_code = optarg; break;
      case 'f': finish = optarg; break;
      case 'n': number = optarg; break;
      case 'p': pipes = optarg; break;
      case 's': start = optarg; break;
      case 'E': {
        Session session;
        Database db = Database("/home/riddle/.history.db");
        // TODO(cpa): end the session
/*
UPDATE
  sessions
SET
  end_time = ${end_ts},
  duration = ${end_ts} - start_time
WHERE
  id = ${AH_SESSION_ID}
*/
        exit(0);
      }
      case 'S': {
        Session session;
        Database db = Database("/home/riddle/.history.db");
        cout << db.get_session_id() << endl;
        exit(0);
      }
      case '?': {
        cout << "Question Mark???" << endl;
        break;
      }
      default:
        cout << "AWESOME: " << c << endl;
        break;
    }
  }
  // TODO(cpa): parse the received arguments for sanity and log the command
  Database db = Database("/home/riddle/.history.db");
  int ec = atoi(exit_code.c_str());
  int st = atoi(start.c_str());
  int fi = atoi(finish.c_str());
  int nu = atoi(number.c_str());
  Command com(command, ec, st, fi, nu);
  cout << com.get_sql() << endl;
  return 0;
}
