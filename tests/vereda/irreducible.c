// A goto into a loop gives a cycle with two ways in, which ir turns into a
// loop with one head (FixIrreducible). Taking the goto skips one s += 2.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int i = 0, s = 0;
  if (__VERIFIER_nondet_int())
    goto inside;
  while (i < 10) {
    s += 2;
  inside:
    i++;
  }
  __VERIFIER_assert(s == 18 || s == 20);
  return 0;
}
