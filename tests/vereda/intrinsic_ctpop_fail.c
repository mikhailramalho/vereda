// A symbolic popcount assertion that fails when 17 bits are set.
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assert(__builtin_popcount(x) != 17);
  return 0;
}
