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

#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
//#include "ash_log.hpp"
#include "flags.hpp"

using namespace std;


// Static members.
string Flag::codes;
list<struct option> Flag::options;
map<const char, Flag *> Flag::short_names;
map<const string, Flag *> Flag::long_names;


// Special Flags.
DEFINE_flag(help, 0, "Display flags for this command.");


/**
 * 
 */
int Flag::parse(int * p_argc, char *** p_argv, const bool remove_flags) {
//cout << "Flag::codes = " << Flag::codes << endl;
//cout << "Program name is: " << argv[0] << endl;

  // TODO(cpa): grab the program name from argv[0], set it into a static variable for --help output later
  int argc = *p_argc;
  char ** argv = *p_argv;

  // Put the options into an array, as getopt expects them.
  struct option * options = new struct option[Flag::options.size()];
  int x = 0;
  for (list<struct option>::iterator i = Flag::options.begin(), e = Flag::options.end(); i != e; ++i) {
    options[x++] = *i;
  }

  // Parse the arguments.
  while (true) {
    int index = 0, c = getopt_long(argc, argv, Flag::codes.c_str(), options, &index);
    switch (c) {
      case -1: goto done;

      case 0: {  // longopt with no short name.
        Flag::long_names[options[index].name] -> set(optarg);
        break;
      }

      case '?':
        cout << "QUESTION MARK" << endl; break;

      default:
        // TODO(cpa): make sure c is in short_names...
        Flag::short_names[c] -> set(optarg);
        break;
    }
  }
done:

  // TODO(cpa): check optind < argc - see manpage...
  cout << "optind = " << optind << ", argc = " << argc << endl;

  delete [] options;
  return 0;
}


/**
 * 
 */
Flag::Flag(const char * ln, const char sn, const char * ds, const bool has_arg)
  : long_name(ln), short_name(sn), description(ds)
{
  Flag::long_names[long_name] = this;

  // Create an option struct and add it to the list.
  Flag::options.push_back((struct option) {ln, has_arg ? 1 : 0, 0, sn});

  // Add the short_name to a flag_code string.
  if (isgraph(short_name)) {
    Flag::short_names[short_name] = this;
    Flag::codes.push_back(short_name);
    if (has_arg) {
      Flag::codes.push_back(':');
    }
  }
}


Flag::~Flag() {
  // TODO(cpa): delete the option structs, if this is the --help option ???
}


IntFlag::IntFlag(const char * ln, const char sn, int * val, const int dv, const char * ds)
  : Flag(ln, sn, ds, true), value(val)
{
  *value = dv;
}


void IntFlag::set(const char * optarg) {
  if (optarg) {
    *value = atoi(optarg);
  }
}


StringFlag::StringFlag(const char * ln, const char sn, string * val, const char * dv, const char * ds)
  : Flag(ln, sn, ds, true), value(val)
{
  set(dv);
}


void StringFlag::set(const char * optarg) {
  *value = optarg;
}


BoolFlag::BoolFlag(const char * ln, const char sn, bool * val, const bool dv, const char * ds, const bool has_arg)
  : Flag(ln, sn, ds, has_arg), value(val)
{
  *val = dv;
}


void BoolFlag::set(const char * optarg) {
  if (optarg) {
    // TODO(cpa): try to parse the option passed - 0 / 1, true / false, yes / no, Y / N or T / F
  } else {
    *value = true;
  }
}
