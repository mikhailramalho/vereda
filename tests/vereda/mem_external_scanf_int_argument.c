// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Variadic arguments that are not pointers, an error in the program, are
// not written, with a constant format or not.
#include <stdio.h>
int main(int argc, char **argv) {
  char s[8];
  const char *f = argc > 1 ? "%d" : "%u";
  sscanf(s, f, 5, 6.0);
  sscanf(s, "%d", argc);
  return 0;
}
