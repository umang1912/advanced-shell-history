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

  string db_file = string(getenv("HOME")) + "/.history.db";
  string command, exit_code, start, finish, number, pipes;
  bool end_session = false, start_session = false;

  int c = 0;
  while (c != -1) {
    int index = 0;
    c = getopt_long(argc, argv, "c:e:f:n:p:s:ES", options, &index);
    switch (c) {
      case -1: break;

      case 'c': command = optarg; break;
      case 'e': exit_code = optarg; break;
      case 'f': finish = optarg; break;
      case 'n': number = optarg; break;
      case 'p': pipes = optarg; break;
      case 's': start = optarg; break;

      case 'E': end_session = true; break;
      case 'S': start_session = true; break;

      case '?': {
        cout << "Question Mark???" << endl;
        break;
      }
      default:
        cout << "AWESOME: " << c << endl;
        break;
    }
  }
  if (start_session) {
    Session session;
    Database db = Database(db_file.c_str());
    cout << db.get_session_id() << endl;
    exit(0);
  }
  if (end_session) {
    Session session;
    Database db = Database(db_file.c_str());
    db.exec(session.get_close_session_sql().c_str());
    exit(0);
  }
  // TODO(cpa): parse the received arguments for sanity and log the command
  Database db = Database(db_file.c_str());
  int ec = atoi(exit_code.c_str());
  int st = atoi(start.c_str());
  int fi = atoi(finish.c_str());
  int nu = atoi(number.c_str());
  Command com(command, ec, st, fi, nu, pipes);
  db.exec(com.get_sql().c_str());
//cout << com.get_sql() << endl;
  exit(ec);
  return ec;
}
