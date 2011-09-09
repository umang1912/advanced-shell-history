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

#include "unix.hpp"

#include "config.hpp"
#include "database.hpp"
#include "util.hpp"

#include <fstream>      /* for ifstream */
#include <sstream>      /* for stringstream */

#include <arpa/inet.h>  /* for inet_ntop */
#include <ifaddrs.h>    /* for getifaddrs, freeifaddrs */
#include <stdlib.h>     /* for getenv */
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>       /* for time */
#include <unistd.h>     /* for getppid, get_current_dir_name */


using namespace ash;
using namespace std;


/**
 * Returns the i'th row of /proc/$$/stat.
 */
const string proc_stat(int target) {
  static string filename = "/proc/" + unix::pid() + "/stat";
  ifstream fin(filename.c_str());
  string token;
  for (int i = 0; fin.good(); ++i) {
    fin >> token;
    if (i == target) break;
  }
  fin.close();
  return token;
}


/**
 * Returns the current working directory.
 */
const string unix::cwd() {
  char * c = get_current_dir_name();
  if (!c) return DBObject::quote(0);
  string cwd(c);
  free(c);
  return DBObject::quote(cwd);
}


bool exists(const char * dir) {
  struct stat st;
  return stat(dir, &st);
}


/**
 * Returns the parent process ID.
 */
const string unix::ppid() {
  if (exists("/proc"))
    return proc_stat(3);

  stringstream ss;
  ss << "/bin/ps ho ppid " << getppid();

  char pp[100];
  FILE * p = popen(ss.str().c_str(), "r");
  if (p && fgets(pp, 100, p) != 0) {
    ss.str("");
    ss << atoi(pp);
    pclose(p);
    return ss.str();
  }
  pclose(p);
  return "null";
}


/**
 * Returns the name of the running shell.
 */
const string unix::shell() {
  if (exists("/proc")) {
    string sh = proc_stat(1);
    // If the shell name is wrapped in parentheses, strip them.
    if (!sh.empty() && sh[0] == '(' && sh[sh.length() - 1] == ')') {
      sh = sh.substr(1, sh.length() - 2);
    }
    return DBObject::quote(sh);
  } else {
    // This is common on OSX - no procfs.
    stringstream ss;
    ss << "/bin/ps ho cmd " << getppid();

    char sh[100];
    FILE * p = popen(ss.str().c_str(), "r");
    if (p && fgets(sh, 100, p) != 0) {
      pclose(p);
      return DBObject::quote(string(sh));
    }
    pclose(p);
  }
  return "null";
}


/**
 * Returns the effective user ID.
 */
const string unix::euid() {
  return Util::to_string(geteuid());
}


/**
 * Returns the process ID of the shell.
 */
const string unix::pid() {
  return Util::to_string(getppid());
}


/**
 * Returns the current local UNIX epoch timestamp.
 */
const string unix::time() {
  return Util::to_string(::time(0));
}


/**
 * Returns the local time zone code.
 */
const string unix::time_zone() {
  stringstream ss;
  char zone_buffer[5];
  time_t now = ::time(0);
  strftime(zone_buffer, 5, "%Z", localtime(&now));
  ss << zone_buffer;
  return DBObject::quote(ss.str());
}


/**
 * Returns the user ID running the command.
 */
const string unix::uid() {
  return Util::to_string(getuid());
}


/**
 * Returns a list of IP addresses owned on the machine running the commands.
 */
const string unix::host_ip() {
  struct ifaddrs * addrs;
  if (getifaddrs(&addrs)) {
    return "null";
  }

  Config & config = Config::instance();
  bool skip_lo = config.sets("SKIP_LOOPBACK");

  int ips = 0;
  stringstream ss;
  char buffer[256];
  for (struct ifaddrs * i = addrs; i; i = i -> ifa_next) {
    struct sockaddr * address = i -> ifa_addr;
    if (address == NULL) continue;
    if (skip_lo && i -> ifa_name && string("lo") == i -> ifa_name) continue;

    sa_family_t family = address -> sa_family;
    switch (family) {
      case AF_INET: {
        if (config.sets("LOG_IPV4")) {
          struct sockaddr_in * a = (struct sockaddr_in *) address;
          inet_ntop(family, &(a -> sin_addr), buffer, sizeof(buffer));
        }
        break;
      }
      case AF_INET6: {
        if (config.sets("LOG_IPV6")) {
          struct sockaddr_in6 * a = (struct sockaddr_in6 *) address;
          inet_ntop(family, &(a -> sin6_addr), buffer, sizeof(buffer));
        }
        break;
      }
      default:
        continue;
    }
    if (++ips > 1) ss << " ";
    ss << buffer;
  }
  freeifaddrs(addrs);
  if (ips == 0) return "null";
  return DBObject::quote(ss.str());
}


/**
 * Returns the name of the host.
 */
const string unix::host_name() {
  char buffer[1024];
  if (gethostname(buffer, sizeof(buffer))) {
    perror("advanced shell history: Unix: gethostname");
  }
  return DBObject::quote(buffer);
}


/**
 * Return the login name of the user entering commands.
 */
const string unix::login_name() {
  return DBObject::quote(getlogin());
}


/**
 * Returns the abbreviated controlling TTY; the leading /dev/ is stripped.
 */
const string unix::tty() {
  string tty = DBObject::quote(ttyname(0));
  if (tty.find("/dev/") == 1) {
    return "'" + tty.substr(6);
  }
  return tty;
}


/**
 * Returns the shell environment value for the argument variable.
 */
const string unix::env(const char * name) {
  return DBObject::quote(getenv(name));
}


/**
 * Returns an integer-representation of a shell environment value.
 */
const string unix::env_int(const char * name) {
  return Util::to_string(atoi(getenv(name)));
}
