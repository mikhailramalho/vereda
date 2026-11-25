// A fully-known object holding pointers keeps each pointer's provenance across
// a compressed flush.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int x = 10, y = 20;
  int *p[8];
  for (int k = 0; k < 6; ++k)
    p[k] = &x;
  p[6] = &y;
  p[7] = &y;

  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 8);
  __VERIFIER_assert(*p[i] == (i < 6 ? 10 : 20));
  return 0;
}
