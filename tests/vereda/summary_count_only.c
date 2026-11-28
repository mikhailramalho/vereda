// EXPECT: ^\[INFO\] Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// A loop with no store, no call and no exit but its header's is summarised
// without walking an iteration: it leaves with its header's values after
// the last one (#370).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), i, j = 7;
  for (i = 0; i < n; i++)
    j += 3;
  __VERIFIER_assert(i == n && j == 7 + 3 * n);
  return 0;
}
