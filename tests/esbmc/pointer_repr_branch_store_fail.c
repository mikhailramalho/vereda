// A pointer stored on either arm of a branch reads back equal on both arms,
// so asserting it differs must fail.
#include <assert.h>
#include <stdlib.h>

void *__VERIFIER_nondet_ptr(void);
int __VERIFIER_nondet_int(void);

struct entry
{
  const void *key;
  unsigned long h;
};

int main(void)
{
  struct entry *s = malloc(4 * sizeof(struct entry));
  if (!s)
    return 0;
  const void *key = __VERIFIER_nondet_ptr();
  int i = __VERIFIER_nondet_int() ? 1 : 2;
  if (i == 1)
    s[1].key = key;
  else
    s[2].key = key;
  assert(s[i].key != key);
  return 0;
}
// Imported from ESBMC regression/esbmc/pointer_repr_branch_store_fail
// RUN: -m64 --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.assertion\.\d+\] +line +\d+ +assertion
// Vereda: unmet EXPECT: ^  FAILED +\[main\.assertion\.\d+\] +line +\d+ +assertion
// MODE: FUTURE
// Vereda: verdict FAILED
