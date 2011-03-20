#include <sstream>

#include "ash_log.hpp"


using namespace ash;
using std::stringstream;


Session::Session() {
  values["start_time"] = Unix::time();
  values["ppid"] = Unix::ppid();
  values["pid"] = Unix::pid();
  values["tty"] = Unix::tty();
  values["uid"] = Unix::uid();
  values["euid"] = Unix::euid();
  values["logname"] = Unix::login_name();
  values["hostname"] = Unix::host_name();
  values["host_ip"] = Unix::host_ip();
  values["shell"] = Unix::shell();
  values["sudo_user"] = Unix::env("SUDO_USER");
  values["sudo_uid"] = Unix::env("SUDO_UID");
  values["ssh_client"] = Unix::env("SSH_CLIENT");
  values["ssh_connection"] = Unix::env("SSH_CONNECTION");
}


Session::~Session() {
  // Nothing to do here.
}


string Session::get_name() const {
  return "session";
}


string Session::get_sql() const {
  stringstream ss;
  ss << "BEGIN TRANSACTION; ";
  ss << DBObject::get_sql();
  ss << "SELECT last_insert_rowid(); ";
  ss << "COMMIT;";
  return ss.str();
}

