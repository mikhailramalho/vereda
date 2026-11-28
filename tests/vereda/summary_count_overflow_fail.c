// RUN: --ub-signed-overflow-check
// EXPECT: ^\[INFO\] Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: arithmetic overflow on add
// The overflow check leaves the loop at a block of its own, so the summary
// still walks an iteration, where x + 1000000 overflows for n > 2147 (#370).
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int n = __VERIFIER_nondet_int(), x = 0;
  for (int i = 0; i < n; i++)
    x += 1000000;
  return x;
}
