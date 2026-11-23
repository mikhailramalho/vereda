#include <stddef.h>

struct P
{
  int a;
  int b;
};

int main()
{
  unsigned n = (unsigned)offsetof(struct P, b);
  int s = 0;
  for (unsigned i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(s == 4); // b sits four bytes into the struct
  return 0;
}
// Imported from ESBMC regression/esbmc/offsetof_loop_bound
// RUN: --unwind 6
// EXPECT: ^VERIFICATION SUCCESSFUL$
