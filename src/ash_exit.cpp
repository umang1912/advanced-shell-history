#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;


void usage(const char * message = 0) {
  cout << "\nUsage: ash_exit exit_code\n"
       << "  Sets the exit code to the value of the first argument (quits).";
  if (message) {
    cout << "\n  Note: " << message;
  }
  cout << "\n" << endl;
  exit(1);
}


int main(int argc, char ** argv) {
  if (argc < 1) usage();
  int code = atoi(argv[1]);
  if (code == 0) usage("exit_code of zero is not allowed.");
  if (code < 0 || code > 255) usage("exit_code must be between 1 and 255");
  return code;
}
