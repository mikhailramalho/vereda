/* Negative counterpart of github_7597-merged-base: the symbolic sibling write
 * inside the loop must survive on the merged base, so VAR.out holds `in` and
 * not the literal either branch left there. */
#include <assert.h>

struct plc
{
  int step;
  int out;
};

struct plc VAR;

int __VERIFIER_nondet_int(void);

int main(void)
{
  int in = __VERIFIER_nondet_int();

  if (__VERIFIER_nondet_int())
    VAR.out = 1;
  else
    VAR.out = 2;

  for (VAR.step = 1; VAR.step <= 5; VAR.step = VAR.step + 1)
    VAR.out = in;

  assert(VAR.out == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7597-merged-base_fail
// RUN: --ub-pointer-check=false --ub-div-by-zero-check=false --vla-size-check=false
// EXPECT: ^VERIFICATION FAILED$
