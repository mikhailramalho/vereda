#include <assert.h>
#include <stddef.h>
struct S
{
  int a;
  int arr[4];
};
int __VERIFIER_nondet_int(void);
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 4);
  size_t o = offsetof(struct S, arr[i]);
  assert(o != 12);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7466_dynamic_index_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 14 +assertion
// Vereda: unmet EXPECT: line 14 +assertion
// MODE: FUTURE
// Vereda: verdict FAILED
