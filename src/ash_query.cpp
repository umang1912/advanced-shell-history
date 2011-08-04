
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

#include "ash_query.hpp"

#include "flags.hpp"

#include <iostream>

using namespace std;


DEFINE_string(dir, 'd', 0, "A directory where a command was invoked.");
DEFINE_int(exit, 'e', 0, "Show commands matching this exit code.");
DEFINE_int(exit_not, 0, 0, "Show commands not matching this exit code.");
DEFINE_string(query, 'q', 0, "A custom query (or comma-separated list of queries) to execute.");
DEFINE_int(rowcount, 'r', 0, "Limit the number of rows returned to this value (defaults to all).");
DEFINE_int(session, 's', 0, "Limit history to a given session ID (defaults to current).");

DEFINE_flag(cwd, 'C', "View history that occurred in the current dir.");
DEFINE_flag(duration, 'D', "Show the duration of the command in the output.");
DEFINE_flag(list, 'L', "Display all saved queries.");
DEFINE_flag(recursive, 'R', "View history in the current dir or subdirs.");
DEFINE_flag(version, 0, "Show the version and exit.");

// TODO(cpa): also list the saved queries in the --help output...


int main(int argc, char ** argv) {
  Flag::parse(&argc, &argv, true);
  // TODO(cpa): should no args mimic normal history output?
  return 0;
}
