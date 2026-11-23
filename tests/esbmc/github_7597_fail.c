/* Negative counterpart of github_7597: the counter folds to 6, so the claim
 * below is refuted. It is reached only once the loop terminates, which is
 * what whole-object propagation prevented (#7597). */
#include <assert.h>

struct plc
{
  int step;
  float out;
};

struct io
{
  float in;
};

struct plc VAR;
struct io IO;

float __VERIFIER_nondet_float(void);

int main(void)
{
  IO.in = __VERIFIER_nondet_float();
  if (IO.in < -1.0e38f)
    IO.in = -1.0e38f;
  else if (IO.in > 1.0e38f)
    IO.in = 1.0e38f;

  for (VAR.step = 1; VAR.step <= 5; VAR.step = VAR.step + 1)
    VAR.out = IO.in;

  assert(VAR.step == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7597_fail
// Needs: --no-standard-checks
// RUN: --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
