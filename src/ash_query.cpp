#include "ash_query.hpp"

using namespace std;


DEFINE_string(dir, 'd', 0, "A directory where a command was invoked.");
DEFINE_int(exit, 'e', 0, "Show commands with this exit code.");
DEFINE_string(query, 'q', 0, "A custom query to execute against the history database.");

DEFINE_flag(cwd, 'C', "View history that occurred in the current dir.");
DEFINE_flag(duration, 'D', "Show the duration of the command in the output.");
DEFINE_flag(recursive, 'R', "View history in the current dir or subdirs.");


int main(int argc, char ** argv) {
  Flag::parse(&argc, &argv, true);
  // TODO(cpa): no args should mimic normal history output.
  return 0;
}
