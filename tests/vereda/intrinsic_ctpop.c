// llvm.ctpop counts set bits across widths for both constant and symbolic values.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern unsigned long __VERIFIER_nondet_ulong(void);
extern unsigned long long __VERIFIER_nondet_ulonglong(void);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  unsigned y = __VERIFIER_nondet_uint();

  // Path-known constants exercise Encoder's constant folding.
  if (x == 0u)
    __VERIFIER_assert(__builtin_popcount(x) == 0);
  if (x == ~0u)
    __VERIFIER_assert(__builtin_popcount(x) == 32);
  if (x == 0x55555555u)
    __VERIFIER_assert(__builtin_popcount(x) == 16);

  // Symbolic identities.
  __VERIFIER_assert(__builtin_popcount(x) >= 0 && __builtin_popcount(x) <= 32);
  __VERIFIER_assert(__builtin_popcount(x) + __builtin_popcount(~x) == 32);
  if (x != 0u)
    __VERIFIER_assert(__builtin_popcount(x) >= 1);
  if ((x & y) == 0u)
    __VERIFIER_assert(__builtin_popcount(x | y) ==
                      __builtin_popcount(x) + __builtin_popcount(y));

  unsigned long l = __VERIFIER_nondet_ulong();
  if (l == 0UL)
    __VERIFIER_assert(__builtin_popcountl(l) == 0);
  if (l == ~0UL)
    __VERIFIER_assert(__builtin_popcountl(l) == (int)(sizeof(unsigned long) * 8));
  __VERIFIER_assert(__builtin_popcountl(l) + __builtin_popcountl(~l) ==
                    (int)(sizeof(unsigned long) * 8));

  unsigned long long ll = __VERIFIER_nondet_ulonglong();
  if (ll == 0ULL)
    __VERIFIER_assert(__builtin_popcountll(ll) == 0);
  if (ll == ~0ULL)
    __VERIFIER_assert(__builtin_popcountll(ll) == 64);
  if (ll == 0x123456789abcdef0ULL)
    __VERIFIER_assert(__builtin_popcountll(ll) == 32);
  __VERIFIER_assert(__builtin_popcountll(ll) + __builtin_popcountll(~ll) == 64);
  return 0;
}
