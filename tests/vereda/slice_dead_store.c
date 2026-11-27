// The slice drops the store into a, which no check reads, the product it
// stores and the nondet z that only feeds the product, so the
// counterexample shows neither. The store into b, which the assertion
// reads, stays.
// EXPECT: ^\[INFO\] Slicing removed [1-9]
// EXPECT: ^  x = 5; //
// EXPECT: ^  b\[1\] = 5; //
// EXPECT-NOT: z =|a\[
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
int a[4], b[2];
int main(void) {
  int x = __VERIFIER_nondet_int();
  int z = __VERIFIER_nondet_int();
  a[1] = x * z;
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 2);
  b[i] = x;
  __VERIFIER_assert(b[1] != 5);
  return 0;
}
