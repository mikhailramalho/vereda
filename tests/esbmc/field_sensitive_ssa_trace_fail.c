#include <assert.h>

int __VERIFIER_nondet_int(void);

struct S
{
  int a;
  int b;
};

int main(void)
{
  struct S t;
  t.a = __VERIFIER_nondet_int();
  t.b = 2;
  struct S u;
  u = t;
  assert(u.a != 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_trace_fail
// Needs: --field-sensitive-ssa
// RUN: --field-sensitive-ssa --no-slice
// MODE: FUTURE
// EXPECT: ^  u = \{ \.a = 0 \}\n\n// State 5 file [^\n]*field_sensitive_ssa_trace_fail\.c line 17 [^\n]*\n  u = \{ \.a=1, \.b=2 \}\n\n// State 6 file
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
