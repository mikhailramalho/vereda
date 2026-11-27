#include <assert.h>
int __VERIFIER_nondet_int(void);
struct T
{
  int *p;
  int k;
};
struct T mk(int c, int *a, int *b)
{
  struct T t;
  t.k = 0;
  if (c)
    t.p = a;
  else
    t.p = b;
  return t;
}
int main(void)
{
  int a, b;
  struct T g;
  g = mk(__VERIFIER_nondet_int(), &a, &b);
  assert(g.p == &a);
  return g.k;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_carried_copy_local_fail
// EXPECT: ^State [0-9]+ file .*field_sensitive_ssa_carried_copy_local_fail\.c line +22 column 7 function main thread 0$
// EXPECT: ^  g = \{ \.p=&b, \.k=0 \}$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^State [0-9]+ file .*field_sensitive_ssa_carried_copy_local_fail\.c line +22 column 7 function main thread 0$
// MODE: FUTURE
// Vereda: verdict FAILED
