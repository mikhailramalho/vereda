// Failing sibling of mem_flush_pointer: a pointer recovered after a compressed
// flush still aliases its target, so writing through it modifies y.
// EXPECT: ^VERIFICATION FAILED$
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
  *p[i] = 30;
  __VERIFIER_assert(y == 20);
  return 0;
}
