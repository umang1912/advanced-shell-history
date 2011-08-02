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


#include "config.hpp"

#include <stdlib.h>  /* for getenv */
#include <unistd.h>  /* for environ */

#include <iostream>

using namespace ash;
using namespace std;


extern char ** environ;  /* populated by unistd.h */


Config::Config()
  :values(), is_loaded(false)
{
  // NOTHING TO DO!
}


char * get_ash_env(const string & key) {
  if (key.find("ASH_", 0, 4) == 0)
    return getenv(key.c_str());
  return getenv(("ASH_" + key).c_str());
}


bool Config::has(const string & key) const {
  return get_ash_env(key) != NULL;
}


bool Config::sets(const string & key, const bool dv) const {
  char * env = get_ash_env(key);
  // TODO(cpa): convert env to lowercase
  return env ? "true" == string(env) : dv;
}


int Config::get_int(const string & key, const int dv) const {
  char * env = get_ash_env(key);
  // TODO(cpa): add extra error-checking here since atoi is fairly lax.
  return env ? dv : atoi(env);
}


const char * Config::get_cstring(const string & key, const char * dv) const {
  char * env = get_ash_env(key);
  // TODO(cpa): strip unprintables and unicode chars...
  return env ? string(env).c_str() : dv;
}


string Config::get_string(const string & key, const string & dv) const {
  char * env = get_ash_env(key);
  // TODO(cpa): strip unprintables and unicode chars...
  return env ? string(env) : dv;
}


Config & Config::instance() {
  static Config _instance;
  if (!_instance.is_loaded) {
    // Find all environment variables matching a common prefix ASH_
    for (int i = 0; environ[i] != NULL; ++i) {
      string line = environ[i];
      if (line.substr(0, 4) == "ASH_") {
        int first_equals = line.find_first_of('=');
        string key = line.substr(4, first_equals - 4);
        string value = line.substr(first_equals + 1);
        _instance.values[key] = value;
      }
    }
    _instance.is_loaded = true;
  }
  return _instance;
}

