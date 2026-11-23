unsigned __VERIFIER_nondet_uint(void);
/* A return inside the loop leaves through the loop function's `ret`. */
unsigned find(unsigned n, unsigned key) {
  for (unsigned i = 0; i < n; i++)
    if (i == key)
      return i;
  return n;
}
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), key = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 8);
  unsigned r = find(n, key);
  __VERIFIER_assert(r <= n); // result in range
  return 0;
}
// Imported from ESBMC regression/goto2lean/return_in_loop
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^def find_loop1 \(n : BitVec 32\) \(key : BitVec 32\) \(i : BitVec 32\)$
// EXPECT: ^    ret i\)\)$
// EXPECT: ^  find_loop1 n key i \(fun \(n : BitVec 32\) \(key : BitVec 32\) \(i : BitVec 32\) =>$
// Vereda: verdict none
