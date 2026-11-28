// EXPECT: ^\[INFO\] Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
// The loop leaves j at 7 + 3 * n, which is 913 for n = 302 only (#370).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), i, j = 7;
  for (i = 0; i < n; i++)
    j += 3;
  __VERIFIER_assert(j != 913);
  return 0;
}
