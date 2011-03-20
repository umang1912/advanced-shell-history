#include <fstream>      /* for ifstream */
#include <sstream>      /* for stringstream */

#include <arpa/inet.h>  /* for inet_ntop */
#include <ifaddrs.h>    /* for getifaddrs, freeifaddrs */
#include <stdlib.h>     /* for getenv */
#include <time.h>       /* for time */
#include <unistd.h>     /* for getppid */

#include "ash_log.hpp"


using namespace ash;
using namespace std;


const string null = "null";


// TODO(cpa): move this to Util or DBObject
const string quote(const char * value) {
  if (value) {
    string rval(value);
// TODO(cpa): replace all single-quotes with double-single-quotes... O'Brien becomes O''Brien
    return "'" + rval + "'";
  }
  return null;
}


const string proc_stat(int target) {
  static string filename = "/proc/" + Unix::pid() + "/stat";
  ifstream fin(filename.c_str());
  string token;
  for (int i = 0; fin.good(); ++i) {
    fin >> token;
    if (i == target) break;
  }
  fin.close();
  return token;
}


const string Unix::ppid() {
  return "'" + proc_stat(3) + "'";
}


const string Unix::shell() {
  string token = proc_stat(1);
  if (!token.empty() && token[0] == '(' && token[token.length() - 1] == ')') {
    token = token.substr(1, token.length() - 2);
  }
  return "'" + token + "'";
}


const string Unix::euid() {
  return Util::to_string(geteuid());
}


const string Unix::pid() {
  return Util::to_string(getppid());
}


const string Unix::time() {
  return Util::to_string(::time(0));
}


const string Unix::uid() {
  return Util::to_string(getuid());
}


const string Unix::host_ip() {
  struct ifaddrs * addrs;
  if (getifaddrs(&addrs)) {
    return null;  // TODO(cpa): log warning here
  }

  int ips = 0;
  stringstream ss;
  char buffer[256];
  for (struct ifaddrs * i = addrs; i; i = i -> ifa_next) {
    struct sockaddr * address = i -> ifa_addr;
    if (address == NULL) continue;

    sa_family_t family = address -> sa_family;
    switch (family) {
      case AF_INET: {
        struct sockaddr_in * a = (struct sockaddr_in *) address;
        inet_ntop(family, &(a -> sin_addr), buffer, sizeof(buffer));
        break;
      }
      case AF_INET6: {
        struct sockaddr_in6 * a = (struct sockaddr_in6 *) address;
        inet_ntop(family, &(a -> sin6_addr), buffer, sizeof(buffer));
        break;
      }
      default:
        continue;
    }
    if (++ips > 1) ss << " ";
    ss << buffer;
  }
  freeifaddrs(addrs);
  if (ips == 0) return null;
  return "'" + ss.str() + "'";
}


const string Unix::host_name() {
  char buffer[1024];
  if (gethostname(buffer, sizeof(buffer))) {
    // TOOD(cpa): log a warning
  }
  return quote(buffer);
}


const string Unix::login_name() {
  return quote(getlogin());
}


const string Unix::tty() {
  string tty = quote(ttyname(0));
  if (tty.find("/dev/") == 1) {
    return "'" + tty.substr(6);
  }
  return tty;
}


const string Unix::env(const char * name) {
  return quote(getenv(name));
}
