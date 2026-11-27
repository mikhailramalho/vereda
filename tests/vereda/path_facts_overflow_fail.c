// RUN: --overflow-check --unsigned-overflow-check --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: SUCCESS
// EXPECT: ^  FAILED +\[main\.arithmetic-overflow\.5\]
// path_facts_overflow with each operation one past what its operands'
// ranges keep in range (#296), so a range one off would lose a failure.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
volatile int sink;
int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > -1000 && x <= 1000);
  sink = x * 2147484;
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assume(y > -1000 && y <= 1000);
  sink = y + 2147482648;
  int z = __VERIFIER_nondet_int();
  __VERIFIER_assume(z >= -1000 && z < 1000);
  sink = z - 2147482649;
  unsigned u = __VERIFIER_nondet_uint(), v = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u >= 998 && u < 2000 && v < 1000);
  sink = u - v;
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint();
  __VERIFIER_assume(a <= 65536 && b <= 65536);
  sink = a * b;
  return 0;
}
