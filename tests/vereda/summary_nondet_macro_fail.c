// Two nondet calls at one location, from a macro, still give two values
// per iteration in a summarised loop.
// EXPECT: ^\[INFO\] Summarised 1 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
#define DIFF() (__VERIFIER_nondet_int() - __VERIFIER_nondet_int())
int a[10];
int main(void) {
  for (int i = 0; i < 10; i++)
    a[i] = DIFF();
  __VERIFIER_assert(a[3] == 0);
  return 0;
}
