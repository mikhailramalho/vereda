// #10186: type 1 or 3 gives the size of s.a, so this can fail.
#include <assert.h>
struct S
{
  int a, b;
};
extern int __VERIFIER_nondet_int(void);
int main(void)
{
  struct S s;
  assert(__builtin_object_size(&s.a, __VERIFIER_nondet_int()) == 8);
}
// Imported from ESBMC regression/esbmc/github_10186_object_size_nondet_type_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
