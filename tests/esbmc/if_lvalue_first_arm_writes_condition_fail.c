struct S { int a, b, c; };
struct S s;
int c;
int __VERIFIER_nondet_int(void);
int main(void)
{
  c = __VERIFIER_nondet_int();
  if (c == 1 || c == 2 || c == 3) return 0;
  *(c != 1 && c != 2 && c != 3 ? &c : (c == 1 ? &s.a : (c == 2 ? &s.b : &s.c))) = 1;
  __VERIFIER_assert(s.a == 1); // the store went to c
  return 0;
}
// Imported from ESBMC regression/esbmc/if_lvalue_first_arm_writes_condition_fail
// EXPECT: ^VERIFICATION FAILED$
