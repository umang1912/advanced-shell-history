#include <getopt.h>  /* for getopt_long */
#include <iostream>  /* for cout, endl */
#include <stdlib.h>  /* for exit */

#include "ash_log.hpp"


using namespace ash;
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
      case '?': {
        cout << "Question Mark???" << endl;
        break;
      }
      default:
        cout << "AWESOME: " << c << endl;
        break;
    }
  }
  return 0;
}
