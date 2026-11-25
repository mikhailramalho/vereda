// An unreachable function with unsupported constructs and a dead global table
// are pruned before IR preparation and walking, so verification succeeds.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: VERIFICATION FAILED
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
extern int pthread_create(void *, const void *, void *(*)(void *), void *);

static void *dead_worker(void *p) { return p; }

static int dead_unsupported(int x) {
  static void *targets[] = {&&a, &&b};
  void *t = 0;
  pthread_create(&t, 0, dead_worker, 0);
  goto *targets[x & 1];
a:
  return 1;
b:
  return 2;
}

static int (*dead_table[2])(int) = {dead_unsupported, dead_unsupported};

static int live_helper(int x) { return x + 1; }

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x >= 0 && x < 100)
    __VERIFIER_assert(live_helper(x) > x);
  return 0;
}
