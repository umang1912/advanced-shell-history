#include <sstream>

#include "ash_log.hpp"


using namespace ash;
using std::stringstream;


Command::Command(const string command, int rval, int start_ts, int end_ts, int number) {
  values["session_id"] = Unix::env_int("AH_SESSION_ID");
  values["shell_level"] = Unix::env_int("SHLVL");
  values["command_no"] = Util::to_string(number);
  values["tty"] = Unix::tty();
  values["euid"] = Unix::euid();
  values["cwd"] = Unix::cwd();
  values["rval"] = Util::to_string(rval);
  values["start_time"] = Util::to_string(start_ts);
  values["end_time"] = Util::to_string(end_ts);
  values["duration"] = Util::to_string(end_ts - start_ts);
  values["pipe_cnt"] = "null";
  values["pipe_vals"] = "null";
  values["command"] = quote(command);
}


Command::~Command() {
  // Nothing to do.
}


// TODO(cpa): rename this table_name or something
const string Command::get_name() const {
  return "commands";
}


const string Command::get_create_table() const {
  stringstream ss;
  ss << "CREATE TABLE IF NOT EXISTS commands("
     << "  id integer primary key autoincrement,"
     << "  session_id integer not null,"
     << "  shell_level integer not null,"
     << "  command_no integer not null,"
     << "  tty varchar(20) not null,"
     << "  euid int(16) not null,"
     << "  cwd varchar(256) not null,"
     << "  rval int(5) not null,"
     << "  start_time integer not null,"
     << "  end_time integer not null,"
     << "  duration integer not null,"
     << "  pipe_cnt int(3) not null,"
     << "  pipe_vals varchar(80) not null,"
     << "  command varchar(1000) not null"
     << ");";
  return ss.str();
}


const string Command::get_sql() const {
  stringstream ss;
  ss << DBObject::get_sql();
  ss << "UPDATE sessions ";
  ss << "SET end_time = null, duration = null ";
  ss << "WHERE id = " << Unix::env_int("AH_SESSION_ID") << ";";
  return ss.str();
}
