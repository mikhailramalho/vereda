// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.unwind\.0\]
// The paths that go on past the first loop under --unwind 2 have argc 0 or
// 1, so the second loop's test 1 < argc folds and its body, a call the
// walker refuses (threads are not modelled), is never walked (#120); a walk
// that reached it would end UNKNOWN instead of reporting the first loop's
// unwinding assertion.
// esbmc-unix2/23_picosat-846_01 has this shape, with option parsing in the
// second loop.
extern void __VERIFIER_assume(int);
extern int pthread_create(void *, const void *, void *(*)(void *), void *);
void *run(void *p) { return p; }
int main(int argc, char **argv) {
  __VERIFIER_assume(argc >= 0 && argc < 8);
  for (int c = 0; c < argc; c++)
    __VERIFIER_assume(argv[c] != 0);
  for (int i = 1; i < argc; i++)
    pthread_create(0, 0, run, argv[i]);
  return 0;
}
