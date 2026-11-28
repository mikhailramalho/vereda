// As pragma_unroll_bound, with #pragma unroll 3: the body runs twice, one
// time fewer than the loop needs, so the unwinding assertion fails (#74).
// EXPECT: ^  FAILED +\[main\.\S+\]  line 10  unwinding assertion loop 0$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
int a[8];

int main(void) {
  #pragma unroll 3
  for (int i = 0; i < 3; i++)
    a[i] = i + 1;
  __VERIFIER_assert(a[2] == 3);
  return 0;
}
// RUN: --no-incremental-bmc
