#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  if (argc<2) {
    fprintf(stderr, "Program name is: %s\n", argv[0]);
  }
}
// Imported from ESBMC regression/esbmc/argc_argv
// EXPECT: ^VERIFICATION SUCCESSFUL$
