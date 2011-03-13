#include <getopt.h>
#include <iostream>  /* for cout, endl */
#include <stdlib.h>  /* for exit */

#include "sqlite3.h"


using namespace std;


void get_session_id() {
  // TODO(cpa): open the database
  // TODO(cpa): if the current session_id exists, emit it
  // TODO(cpa): else create a new session_id and emit it
  // TODO(cpa): close the database
  exit(0);
}


static struct option options[] = {
  {"get-session-id", 0, 0, 's'}
};


int main(int argc, char ** argv) {
  int c = 0;
  while (c != -1) {
    int index = 0;
    c = getopt_long(argc, argv, "s", options, &index);
    switch (c) {
      case 's':
        get_session_id();
      default:
        cout << "AWESOME: " << c << endl;
    }
  }
  return 0;
}
