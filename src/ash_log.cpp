#include <getopt.h>  /* for getopt_long */
#include <stdlib.h>  /* for exit */

#include <iostream>  /* for cout, endl */
#include <sstream>

#include "ash_log.hpp"


using namespace ash;
using namespace std;


static struct option options[] = {
  {"alert", 1, 0, 'a'},
  {"command", 1, 0, 'c'},
  {"command-exit", 1, 0, 'e'},
  {"finish", 1, 0, 'f'},
  {"number", 1, 0, 'n'},
  {"pipe-status", 1, 0, 'p'},
  {"start", 1, 0, 's'},
  {"exit", 1, 0, 'x'},
  {"end-session", 0, 0, 'E'},
  {"get-session-id", 0, 0, 'S'}
};


int main(int argc, char ** argv) {
  DBObject::register_table(Session::get_create_table());
  DBObject::register_table(Command::get_create_table());

  string db_file = string(getenv("HOME")) + "/.history.db";
  string alert, command, exit_code, start, finish, number, pipes, to_exit;
  bool end_session = false, start_session = false;

  int c = 0;
  while (c != -1) {
    int index = 0;
    c = getopt_long(argc, argv, "a:c:e:f:n:p:s:x:ES", options, &index);
    switch (c) {
      case -1: break;

      case 'a': alert = optarg; break;
      case 'c': command = optarg; break;
      case 'e': exit_code = optarg; break;
      case 'f': finish = optarg; break;
      case 'n': number = optarg; break;
      case 'p': pipes = optarg; break;
      case 's': start = optarg; break;
      case 'x': to_exit = optarg; break;

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

  if (!alert.empty()) {
    cout << alert << endl;
  }

  if (start_session) {
    Database db = Database(db_file.c_str());
    stringstream ss;
    char * id = getenv(ASH_SESSION_ID);
    if (id) {
      ss << "select count(*) from sessions where id = " << id << ";";
      if (db.select_int(ss.str().c_str()) == 0) {
        cout << "ERROR: session_id(" << id << ") not found, creating new session." << endl << ss.str() << endl;
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

  if (!command.empty()) {
    // TODO(cpa): parse the received arguments for sanity
    Database db = Database(db_file.c_str());
    int ec = atoi(exit_code.c_str());
    int st = atoi(start.c_str());
    int fi = atoi(finish.c_str());
    int nu = atoi(number.c_str());
    Command com(command, ec, st, fi, nu, pipes);
    db.exec(com.get_sql().c_str());
  }

  if (end_session) {
cout << "Ending session" << endl;
    Session session;
    Database db = Database(db_file.c_str());
    db.exec(session.get_close_session_sql().c_str());
  }

  if (!to_exit.empty()) {
    exit(atoi(to_exit.c_str()));
  }

  return 0;
}
