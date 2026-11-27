int __VERIFIER_nondet_int(void);
_Bool __VERIFIER_nondet_bool(void);
struct S { long long v; };
struct W { struct S e[4096]; };
int main(void)
{
  struct W w1, w2;
  int j = __VERIFIER_nondet_int();
  __VERIFIER_assume(j >= 0 && j < 4096);
  w2.e[j].v = 1;
  _Bool b = __VERIFIER_nondet_bool();
  struct W w = b ? w1 : w2;
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 4096);
  __VERIFIER_assert(i != j || w.e[i].v == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_array_ite_new_index_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
