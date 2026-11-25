// --incremental-bmc raises the bound past failed recursion unwinding
// assertions too: f(3) recurses four deep.
// RUN: --incremental-bmc
// EXPECT: ^\[INFO\] Bound 3 too small: recursion unwinding assertion$
// EXPECT-NOT: ^\[INFO\] Bound 4 too small
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);

int f(int n) {
  if (n <= 0)
    return 0;
  return 1 + f(n - 1);
}

int main(void) {
  __VERIFIER_assert(f(3) != 3);
  return 0;
}
