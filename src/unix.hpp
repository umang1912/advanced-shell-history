#ifndef __ASH_UNIX__
#define __ASH_UNIX__

#include <string>

using std::string;

namespace ash {


struct Unix {
  static const string euid();
  static const string pid();
  static const string ppid();
  static const string time();
  static const string uid();
  static const string env(const char * name);
  static const string host_ip();
  static const string host_name();
  static const string login_name();
  static const string shell();
  static const string tty();
};


}; // namespace ash

#endif /* __ASH_UNIX__ */
