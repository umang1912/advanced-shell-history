#include <sstream>

#include "ash_log.hpp"


using namespace ash;
using std::stringstream;


const string Session::get_create_table() {
  stringstream ss;
  ss << "CREATE TABLE IF NOT EXISTS sessions ("
     << "  id integer primary key autoincrement,"
     << "  hostname varchar(128) not null,"
     << "  host_ip varchar(40) not null,"
     << "  ppid int(5) not null,"
     << "  pid int(5) not null,"
     << "  start_time integer not null,"
     << "  end_time integer,"
     << "  duration integer,"
     << "  tty varchar(20) not null,"
     << "  uid int(16) not null,"
     << "  euid int(16) not null,"
     << "  logname varchar(48) not null,"
     << "  shell varchar(50) not null,"
     << "  sudo_user varchar(48),"
     << "  sudo_uid int(16),"
     << "  ssh_client varchar(60),"
     << "  ssh_connection varchar(100)"
     << ");";
  return ss.str();
}


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


const string Session::get_name() const {
  return "sessions";
}


const string Session::get_sql() const {
  stringstream ss;
  ss << "BEGIN TRANSACTION; ";
  ss << DBObject::get_sql();
  ss << "SELECT last_insert_rowid(); ";
  ss << "COMMIT;";
  return ss.str();
}

