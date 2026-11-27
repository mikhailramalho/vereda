#include <assert.h>
#include <stdio.h>

struct s
{
  char c;
  char d;
  short h;
} g = {1, 2, 3};

int main()
{
  sscanf("x", "%c", &g.d);
  assert(g.c == 1);
  assert(g.h == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_char_member
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
