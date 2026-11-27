#include <assert.h>
struct S
{
  unsigned short a, b, c;
};
struct S s[4];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 0 || i == 2);
  unsigned char *p = (unsigned char *)&s[i] + 4;
  *(unsigned int *)p = 0x00020001u;
  assert(s[i].c == 1);
  assert(s[i + 1].a == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10088_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
