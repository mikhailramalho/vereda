// llvm.prefetch is a cache hint with no observable effect on memory or values.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

static int g[8] = {1, 2, 3, 4, 5, 6, 7, 8};

int main(void) {
  int a[8];
  for (int i = 0; i < 8; ++i) {
    __builtin_prefetch(&a[i], 1, 3);
    a[i] = i * 2;
  }
  int idx = __VERIFIER_nondet_int();
  if (idx >= 0 && idx < 8) {
    __builtin_prefetch(&a[idx], 0, 0);
    __builtin_prefetch(&g[idx], 0, 2);
    __VERIFIER_assert(a[idx] == idx * 2);
    __VERIFIER_assert(g[idx] == idx + 1);
  }
  return 0;
}
