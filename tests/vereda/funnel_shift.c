// InstCombine turns rotates, and shifts that join two words, into
// llvm.fshl and llvm.fshr. Each is checked against a shift of the two words
// joined in one wider word; the volatile copy hides the join from
// InstCombine, which would otherwise fold the reference into a funnel shift
// as well. The amount is nondet (any value, so also 0, N and above N), pinned
// to 0 or N, or constant.
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
typedef unsigned short u16;
typedef unsigned long long u64;
typedef unsigned __int128 u128;
extern unsigned char __VERIFIER_nondet_uchar(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern unsigned long __VERIFIER_nondet_ulong(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

static unsigned fshl32(unsigned a, unsigned b, unsigned s) {
  unsigned k = s & 31;
  return k == 0 ? a : (a << k) | (b >> (32 - k));
}
static unsigned fshr32(unsigned a, unsigned b, unsigned s) {
  unsigned k = s & 31;
  return k == 0 ? b : (b >> k) | (a << (32 - k));
}

static void check32(unsigned a, unsigned b, unsigned s) {
  volatile u64 join = (u64)a << 32 | b;
  u64 ab = join;
  __VERIFIER_assert(fshl32(a, b, s) == (unsigned)(ab << s % 32 >> 32));
  __VERIFIER_assert(fshr32(a, b, s) == (unsigned)(ab >> s % 32));
}

int main(void) {
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint();
  unsigned s = __VERIFIER_nondet_uint(), z = __VERIFIER_nondet_uint();
  check32(a, b, s);
  __VERIFIER_assume(z == 0 || z == 32);
  check32(a, b, z);

  volatile u64 join = (u64)a << 32 | b;
  u64 ab = join;
  __VERIFIER_assert(((a << 3) | (b >> 29)) == (unsigned)(ab << 3 >> 32));
  __VERIFIER_assert(((b >> 7) | (a << 25)) == (unsigned)(ab >> 7));

  unsigned char c = __VERIFIER_nondet_uchar();
  volatile u16 cc = (u16)(c << 8 | c);
  __VERIFIER_assert(__builtin_rotateleft8(c, s) == (unsigned char)(cc << s % 8 >> 8));
  __VERIFIER_assert(__builtin_rotateright8(c, z) == c);

  volatile u64 aa = (u64)a << 32 | a;
  __VERIFIER_assert(__builtin_rotateleft32(a, s) == (unsigned)(aa << s % 32 >> 32));
  __VERIFIER_assert(__builtin_rotateright32(a, s) == (unsigned)(aa >> s % 32));

  u64 w = __VERIFIER_nondet_ulong();
  volatile u128 ww = (u128)w << 64 | w;
  __VERIFIER_assert(__builtin_rotateleft64(w, s) == (u64)(ww << s % 64 >> 64));
  __VERIFIER_assert(__builtin_rotateright64(w, 2 * z) == w);
  return 0;
}
