// pragma_unroll_bound_false's program under --incremental-bmc: there the
// bound grows from walk to walk, and a #pragma unroll bound fixed at 3
// would cut the loop at every one, so the pragma is ignored, as on main,
// and the run ends at the first bound with a verdict (#74).
// RUN: --incremental-bmc --max-k-step 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: too small
extern void __VERIFIER_assert(int);
int a[8];

int main(void) {
  #pragma unroll 3
  for (int i = 0; i < 3; i++)
    a[i] = i + 1;
  __VERIFIER_assert(a[2] == 3);
  return 0;
}
