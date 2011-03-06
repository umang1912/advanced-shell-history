#include <stdio.h>
#include <stdlib.h>


int main(int argc, char ** argv) {
  if (argc == 1) {
    printf("Usage: %s exit_code [extra args]\n", argv[0]);
    return 1;
  }
  return atoi(argv[1]);
}
