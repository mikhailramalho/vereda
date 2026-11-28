// With --fresh-solver, a resumed bound solves on a fresh solver instance
// (array-crafted/mapsum3 reduced). The bounds below 10001 each asked a trivial
// query; when one instance answered all of them, Bitwuzla skipped its full
// preprocessing on the last, real one and ran out of memory past 4 GB. Since
// #392 skips those constant queries the kept instance needs 130 MB too.
// RUN: --incremental-bmc --fresh-solver --unlimited-k-steps
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
#define N 10000
int x[N];
long long sum(void) {
  long long r = 0;
  for (int i = 0; i < N; i++)
    r = r + i + x[i];
  return r;
}
int main(void) {
  for (int i = 0; i < N; i++)
    x[i] = __VERIFIER_nondet_int();
  long long a = sum();
  int t = x[0];
  x[0] = x[1];
  x[1] = t;
  __VERIFIER_assert(a == sum());
}
