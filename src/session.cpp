#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include <stdlib.h> /* for getenv */
#include <time.h>   /* for time */
#include <unistd.h> /* for getpid, getppid, ttyname */

#include "session.hpp"


using namespace std;


string get_pid() {
  stringstream ss;
  ss << getppid();
  return ss.str();
}


string get_ppid() {
  // TODO(cpa): refactor the /proc/$$/stat reading code so it's cached
  string filename = "/proc/" + get_pid() + "/stat";
  ifstream fin(filename.c_str());
  string token;
  for (int i = 0; fin.good(); ++i) {
    fin >> token;
    if (i == 3) break;
  }
  fin.close();
  return token;
}


string get_shell() {
  string filename = "/proc/" + get_pid() + "/stat";
  ifstream fin(filename.c_str());

  string token;
  for (int i = 0; fin.good(); ++i) {
    fin >> token;
    if (i == 1) break;
  }
  fin.close();

  if (!token.empty() && token[0] == '(' && token[token.length() - 1] == ')') {
    token = token.substr(1, token.length() - 2);
  }
  return "'" + token + "'";
}


string get_time() {
  stringstream ss;
  ss << time(0);
  return ss.str();
}


string get_tty() {
  string tty = ttyname(0);
  if (tty.find("/dev/") == 0) {
    tty = tty.substr(5);
  }
  return string("'") + tty + "'";
}


string get_env(const char * name) {
  char * val = getenv(name);
  if (val) {
    return string("'") + val + "'";
  }
  return "null";
}


Session::Session() {
  // TODO(cpa): save the environment variables into strings, etc.
  values["start_time"] = get_time();
  values["ppid"] = get_ppid();
  values["pid"] = get_pid();
  values["tty"] = get_tty();
  values["uid"] = "";
  values["euid"] = "";
  values["logname"] = "";
  values["hostname"] = get_env("HOSTNAME");
  values["host_ip"] = "";
  values["shell"] = get_shell();
  values["sudo_user"] = get_env("SUDO_USER");
  values["sudo_uid"] = get_env("SUDO_UID");
  values["ssh_client"] = get_env("SSH_CLIENT");
  values["ssh_connection"] = get_env("SSH_CONNECTION");
}


// TODO(cpa): extract a base class that defines this method
string Session::get_sql() const {
  typedef map<string, string>::const_iterator c_iter;

  static stringstream ss;
  ss.str("");
  ss << "BEGIN TRANSACTION; ";
  ss << "INSERT INTO sessions (";

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
  ss << "SELECT last_insert_rowid(); ";
  ss << "COMMIT;";

  return ss.str();
}

