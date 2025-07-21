#include <stdio.h>
#include <stdlib.h>

#ifndef COMMIT_HASH
#define COMMIT_HASH "0"
#endif

int main() {
  fprintf(stderr, "Lily version " COMMIT_HASH " by CeriseSky\n");

  return EXIT_SUCCESS;
}

