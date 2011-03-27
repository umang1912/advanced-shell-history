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

//#include "ash_log.hpp"
#include "flags.hpp"


int Flag::parse(int * argc, char *** argv, const bool remove_flags) {
  return 0;
}


Flag::Flag(const char * ln, const char sn, const char * ds)
  : long_name(ln), short_name(sn), description(ds)
{
}


IntFlag::IntFlag(const char * ln, const char sn, const int dv, const char * ds)
  : Flag(ln, sn, ds), default_value(dv)
{
}


StringFlag::StringFlag(const char * ln, const char sn, const char * dv, const char * ds)
  : Flag(ln, sn, ds), default_value(dv)
{
}


BoolFlag::BoolFlag(const char * ln, const char sn, const bool dv, const char * ds)
  : Flag(ln, sn, ds), default_value(dv)
{
}


