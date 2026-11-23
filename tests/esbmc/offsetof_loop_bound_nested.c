#include <stddef.h>

struct P
{
  char c;
  int v[3];
};

int main()
{
  unsigned n = (unsigned)offsetof(struct P, v[2]);
  int s = 0;
  for (unsigned i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(s == 12); // padding then two ints
  return 0;
}
// Imported from ESBMC regression/esbmc/offsetof_loop_bound_nested
// RUN: --unwind 16
// EXPECT: ^VERIFICATION SUCCESSFUL$
