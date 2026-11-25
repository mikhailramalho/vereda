// Failing sibling of mem_flush_partial: an unwritten element of a partially
// initialised array must remain nondet after flush, not take the dominant value
// of the written elements.
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int a[16];
  for (int k = 0; k < 15; ++k)
    a[k] = 5;
  // a[15] is uninitialised and must remain nondet.
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 16);
  __VERIFIER_assert(a[i] == 5);
  return 0;
}
