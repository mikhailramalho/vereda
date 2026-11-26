/* github #9226: the byte offset of a member of an array element, with a
 * stride one int too long. */
#include <assert.h>

int __VERIFIER_nondet_int(void);
struct s
{
  char c;
  int x[3];
} st[4];

int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 3);
  assert((char *)&st[i].x[i] - (char *)st == 20 * i + 4 + 4 * i);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9226_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
