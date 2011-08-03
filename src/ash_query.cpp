
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
DEFINE_int(exit, 'e', 0, "Show commands with this exit code.");
DEFINE_string(query, 'q', 0, "A custom query to execute against the history database.");

DEFINE_flag(cwd, 'C', "View history that occurred in the current dir.");
DEFINE_flag(duration, 'D', "Show the duration of the command in the output.");
DEFINE_flag(recursive, 'R', "View history in the current dir or subdirs.");


int main(int argc, char ** argv) {
  Flag::parse(&argc, &argv, true);
  // TODO(cpa): should no args mimic normal history output?
  return 0;
}
