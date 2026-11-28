// A bare #pragma unroll lifts the bound for its loop, as in ESBMC: the loop
// takes all three iterations although --unwind 2 allows one (#74).
// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
int a[4], b[4];

int main(void) {
  #pragma unroll
  for (int i = 0; i < 3; i++) {
    a[i] = i;
    b[i] = i + 1;
  }
  __VERIFIER_assert(a[2] + b[2] == 5);
  return 0;
}
