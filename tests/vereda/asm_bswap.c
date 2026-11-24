// bswap, as the kernel's __arch_swab32 and __arch_swab64 run it, reverses
// the bytes. The volatile copies keep InstCombine from folding the bswap
// into the comparisons, so the walker encodes it.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern unsigned long __VERIFIER_nondet_ulong(void);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned v = __VERIFIER_nondet_uint(), r;
  __asm__("bswapl %0" : "=r"(r) : "0"(v));
  volatile unsigned rv = r;
  __VERIFIER_assert(rv == (v >> 24 | (v >> 8 & 0xff00) | (v << 8 & 0xff0000) | v << 24));
  unsigned long w = __VERIFIER_nondet_ulong(), s;
  __asm__("bswapq %0" : "=r"(s) : "0"(w));
  volatile unsigned long sv = s;
  __VERIFIER_assert((sv & 0xff) == w >> 56 && (sv >> 48 & 0xff) == (w >> 8 & 0xff));
  return 0;
}
