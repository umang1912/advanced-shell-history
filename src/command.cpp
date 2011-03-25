#include <sstream>

#include "ash_log.hpp"


using namespace ash;
using std::stringstream;


const string Command::get_create_table() {
  stringstream ss;
  ss << "CREATE TABLE IF NOT EXISTS commands(\n"
     << "  id integer primary key autoincrement,\n"
     << "  session_id integer not null,\n"
     << "  shell_level integer not null,\n"
     << "  command_no integer not null,\n"
     << "  tty varchar(20) not null,\n"
     << "  shlvl integer not null,\n"
     << "  euid int(16) not null,\n"
     << "  cwd varchar(256) not null,\n"
     << "  rval int(5) not null,\n"
     << "  start_time integer not null,\n"
     << "  end_time integer not null,\n"
     << "  duration integer not null,\n"
     << "  pipe_cnt int(3) not null,\n"
     << "  pipe_vals varchar(80) not null,\n"
     << "  command varchar(1000) not null\n"
     << ");";
  return ss.str();
}


Command::Command(const string command, const int rval, const int start_ts, const int end_ts, const int number, const string pipes) {
  values["session_id"] = Unix::env_int(ASH_SESSION_ID);
  values["shell_level"] = Unix::env_int("SHLVL");
  values["command_no"] = Util::to_string(number);
  values["tty"] = Unix::tty();
  values["shlvl"] = Unix::env_int("SHLVL");
  values["euid"] = Unix::euid();
  if (rval == 0 && command.find("cd") == 0) {
    values["cwd"] = Unix::env("OLDPWD");
  } else {
    values["cwd"] = Unix::cwd();
  }
  values["rval"] = Util::to_string(rval);
  values["start_time"] = Util::to_string(start_ts);
  values["end_time"] = Util::to_string(end_ts);
  values["duration"] = Util::to_string(end_ts - start_ts);
  int pipe_cnt = 1;
  for (string::const_iterator i = pipes.begin(), e = pipes.end(); i != e; ++i)
    if ((*i) == ' ') ++pipe_cnt;
  values["pipe_cnt"] = Util::to_string(pipe_cnt);
  values["pipe_vals"] = pipes;
  values["command"] = quote(command);
}


Command::~Command() {
  // Nothing to do.
}


// TODO(cpa): rename this table_name or something
const string Command::get_name() const {
  return "commands";
}


const string Command::get_sql() const {
  stringstream ss;
  // TODO(cpa): wrap this in a transaction... or nullify the end_time / duration before inserting command...
  ss << DBObject::get_sql();
  ss << "UPDATE sessions ";
  ss << "SET end_time = null, duration = null ";
  ss << "WHERE id = " << Unix::env_int(ASH_SESSION_ID) << ";";
  return ss.str();
}
