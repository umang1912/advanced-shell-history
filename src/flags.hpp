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

#ifndef __ASH_FLAGS__
#define __ASH_FLAGS__

#include <getopt.h>
#include <list>
#include <map>
#include <string>
using std::list;
using std::map;
using std::string;


#define DEFINE_int(long_name, short_name, default_val, desc) \
static int FLAGS_ ## long_name; \
static IntFlag FLAGS_OPT_ ## long_name(#long_name, short_name, &FLAGS_ ## long_name, default_val, desc)

#define DEFINE_string(long_name, short_name, default_val, desc) \
static string FLAGS_ ## long_name; \
static StringFlag FLAGS_OPT_ ## long_name(#long_name, short_name, &FLAGS_ ## long_name, default_val, desc)

#define DEFINE_bool(long_name, short_name, default_val, desc) \
static bool FLAGS_ ## long_name; \
static BoolFlag FLAGS_OPT_ ## long_name(#long_name, short_name, &FLAGS_ ## long_name, default_val, desc, true)

#define DEFINE_flag(long_name, short_name, desc) \
static bool FLAGS_ ## long_name; \
static BoolFlag FLAGS_OPT_ ## long_name(#long_name, short_name, &FLAGS_ ## long_name, false, desc, false)


class Flag {
  private:
    static string codes;
    static list<struct option> options;
    static map<const char, Flag *> short_names;
    static map<const string, Flag *> long_names;

  public:
    static int parse(int * argc, char *** argv, const bool remove_flags);

  public:
    Flag(const char * long_name, const char short_name, const char * desc, const bool has_arg=false);
    virtual ~Flag();
    virtual void set(const char * optarg) = 0;

  private:
    const char * long_name;
    const char short_name;
    const char * description;
};


class IntFlag : public Flag {
  public:
    IntFlag(const char * ln, const char sn, int * val, const int dv, const char * ds);
    virtual void set(const char * optarg);

  private:
    int * value;
};


class StringFlag : public Flag {
  public:
    StringFlag(const char * ln, const char sn, string * val, const char * dv, const char * ds);
    virtual void set(const char * optarg);

  private:
    string * value;
};


class BoolFlag : public Flag {
  public:
    BoolFlag(const char * ln, const char an, bool * val, const bool dv, const char * ds, const bool has_arg);
    virtual void set(const char * optarg);

  private:
    bool * value;
};


#endif /* __ASH_FLAGS__ */
