#include <assert.h>

/* #10008: designated initialiser of a member of an anonymous struct. */
int main(void)
{
  union { struct { int s; int c; }; unsigned i; } v = { .s = 17 };
  assert(v.i == 18);
  assert(v.s == 17 && v.c == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10008_fail
// EXPECT: ^VERIFICATION FAILED$
