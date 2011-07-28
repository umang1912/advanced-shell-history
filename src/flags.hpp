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

/*
   This class provides flags for programs, much in the same way that Google
   gflags does, although with fewer bells and whistles and no major
   dependencies.
*/

#ifndef __ASH_FLAGS__
#define __ASH_FLAGS__

#include <getopt.h>  /* for struct option */

#include <iostream>
#include <list>
#include <map>
#include <string>

using std::ostream;
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


/**
 * 
 */
class Flag {
  // STATIC
  public:
    static int parse(int * argc, char *** argv, const bool remove_flags);
    static void show_help(ostream & out);

  private:
    static string program_name;
    static string codes;
    static list<struct option> options;
    static list<Flag *> instances;
    static map<const char, Flag *> short_names;
    static map<const string, Flag *> long_names;

  // NON-STATIC
  public:
    Flag(const char * long_name, const char short_name, const char * desc, const bool has_arg=false);
    virtual ~Flag();
    virtual void set(const char * optarg) = 0;
    virtual ostream & insert(ostream & out) const;

  private:
    const char * long_name;
    const char short_name;
    const char * description;
    const bool has_arg;

  // DISABLED
  private:
    Flag(const Flag & other);
    Flag & operator = (const Flag & other);
};


/**
 * Inserts a Flag into an ostream.
 */
ostream & operator << (ostream & out, const Flag & flag);


/**
 * 
 */
class IntFlag : public Flag {
  public:
    IntFlag(const char * ln, const char sn, int * val, const int dv, const char * ds);
    virtual ~IntFlag() {}
    virtual void set(const char * optarg);
    virtual ostream & insert(ostream & out) const;

  private:
    int * value;
};


/**
 * 
 */
class StringFlag : public Flag {
  public:
    StringFlag(const char * ln, const char sn, string * val, const char * dv, const char * ds);
    virtual ~StringFlag() {}
    virtual void set(const char * optarg);
    virtual ostream & insert(ostream & out) const;

  private:
    string * value;
};


/**
 * 
 */
class BoolFlag : public Flag {
  public:
    BoolFlag(const char * ln, const char an, bool * val, const bool dv, const char * ds, const bool has_arg);
    virtual ~BoolFlag() {}
    virtual void set(const char * optarg);
    virtual ostream & insert(ostream & out) const;

  private:
    bool * value;
};


#endif  /* __ASH_FLAGS__ */
