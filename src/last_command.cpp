#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

int main(int argc, char ** argv) {
  // TODO: determine the shell type
  // TODO: based on the shell type, return the last command number, start time, end time and the actual command.
//FILE * out = popen("/bin/bash -c builtin history", "r");
  FILE * out = popen("set", "r");
  if (out == NULL) {
    cout << "ERROR - failed to popen builtin history 1" << endl;
    exit(1);
  }
  char buffer[8000];
  while (fgets(buffer, 8000, out)) {
    cout << buffer << endl;
  }
  pclose(out);
}
