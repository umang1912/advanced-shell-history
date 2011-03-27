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
  if (code < 0 || code > 255)
    usage("exit_code must be between 1 and 255");

  exit(code);
  return code;
}
