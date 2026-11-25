// Pruning unreachable functions and globals still finds a violation in
// reachable code.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: VERIFICATION SUCCESSFUL
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
extern int pthread_create(void *, const void *, void *(*)(void *), void *);

static void *dead_worker(void *p) { return p; }

static int dead_unsupported(void) {
  void *t = 0;
  return pthread_create(&t, 0, dead_worker, 0);
}

static int (*dead_ptr)(void) = dead_unsupported;

static int live_helper(int x) { return x - 1; }

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 5)
    __VERIFIER_assert(live_helper(x) == 5);
  return 0;
}
