#include <getopt.h>
#include <iostream>  /* for cout, endl */
#include <stdlib.h>  /* for exit */

#include "database.hpp"


using namespace std;


static struct option options[] = {
  {"get-session-id", 0, 0, 's'}
};


int main(int argc, char ** argv) {
  int c = 0;
  while (c != -1) {
    int index = 0;
    c = getopt_long(argc, argv, "s", options, &index);
    switch (c) {
      case 's': {
        Database db = Database("/home/riddle/.history.db");
        cout << db.get_session_id() << endl;
        exit(0);
      }
      default:
        cout << "AWESOME: " << c << endl;
    }
  }
  return 0;
}
