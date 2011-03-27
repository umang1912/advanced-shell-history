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

#ifndef __ASH_UNIX__
#define __ASH_UNIX__

#include <string>

using std::string;

namespace ash {


struct Unix {
  static const string cwd();
  static const string env(const char * name);
  static const string env_int(const char * name);
  static const string euid();
  static const string host_ip();
  static const string host_name();
  static const string login_name();
  static const string pid();
  static const string ppid();
  static const string shell();
  static const string time();
  static const string tty();
  static const string uid();
};


}; // namespace ash

#endif /* __ASH_UNIX__ */
