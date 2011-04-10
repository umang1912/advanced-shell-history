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

#include <sstream>

#include "ash_log.hpp"


using namespace ash;
using std::stringstream;


/**
 * 
 */
const string Session::get_create_table() {
  stringstream ss;
  ss << "CREATE TABLE IF NOT EXISTS sessions ( \n"
     << "  id integer primary key autoincrement, \n"
     << "  hostname varchar(128), \n"
     << "  host_ip varchar(40), \n"
     << "  ppid int(5) not null, \n"
     << "  pid int(5) not null, \n"
     << "  start_time integer not null, \n"
     << "  end_time integer, \n"
     << "  duration integer, \n"
     << "  tty varchar(20) not null, \n"
     << "  uid int(16) not null, \n"
     << "  euid int(16) not null, \n"
     << "  logname varchar(48), \n"
     << "  shell varchar(50) not null, \n"
     << "  sudo_user varchar(48), \n"
     << "  sudo_uid int(16), \n"
     << "  ssh_client varchar(60), \n"
     << "  ssh_connection varchar(100) \n"
     << ");";
  return ss.str();
}


/**
 * 
 */
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


/**
 * 
 */
Session::~Session() {
  // Nothing to do here.
}


/**
 * 
 */
const string Session::get_name() const {
  return "sessions";
}


/**
 * 
 */
const string Session::get_sql() const {
  stringstream ss;
  ss << "BEGIN TRANSACTION; ";
  ss << DBObject::get_sql();
  ss << "SELECT last_insert_rowid(); ";
  ss << "COMMIT;";
  return ss.str();
}


/**
 * 
 */
const string Session::get_close_session_sql() const {
  stringstream ss;
  ss << "UPDATE sessions \n"
     << "SET \n"
     << "  end_time = " << Unix::time() << ", \n"
     << "  duration = " << Unix::time() << " - start_time \n"
     << "WHERE id == " << Unix::env(ASH_SESSION_ID) << "; ";
  return ss.str();
}
