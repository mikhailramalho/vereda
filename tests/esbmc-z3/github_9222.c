// #9222: Z3 checks multiplication overflow with its own predicates. Products
// that reach the type's bounds exactly are not overflows.
int __VERIFIER_nondet_int(void);
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int();
  __VERIFIER_assume(a >= -46340 && a <= 46340 && b >= -46340 && b <= 46340);
  int p = a * b;

  int c = __VERIFIER_nondet_int(), d = __VERIFIER_nondet_int();
  __VERIFIER_assume(
    (c == -2147483647 && d == -1) || (c == -2147483647 - 1 && d == 1) ||
    (c == 65536 && d == -32768));
  int q = c * d;

  unsigned u = __VERIFIER_nondet_uint(), v = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u <= 65535 && v <= 65537);
  unsigned r = u * v;
  return 0;
}
// Imported from ESBMC regression/z3/github_9222
// RUN: --solver z3 --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --unsigned-overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$
