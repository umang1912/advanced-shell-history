#include "ash_log.hpp"


using namespace ash;


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


const string Command::get_sql() const {
  // TODO(cpa): create a transactional insert, as in Session??? NO - don't want to lock the DB
  return DBObject::get_sql();
}
