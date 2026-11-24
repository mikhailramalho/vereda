// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// Ranges at the limits of their types fold as well (#120): the greatest
// unsigned values of 8, 32 and 64 bits, signed values near the least and
// greatest, a negative int read as unsigned (all of it at or above
// 0x80000000), an unsigned one below 0x80000000 read as signed, and a
// negative range carried through a sign extension.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern long long __VERIFIER_nondet_longlong(void);
extern unsigned long long __VERIFIER_nondet_ulonglong(void);
extern unsigned char __VERIFIER_nondet_uchar(void);
extern void __VERIFIER_assume(int);
extern void reach_error(void);
int main(void) {
  unsigned u = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u >= 0xFFFFFFF0u);
  if (u < 0xFFFFFFF0u)
    reach_error();
  unsigned char c = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(c >= 250);
  if (c < 250)
    reach_error();
  unsigned long long x = __VERIFIER_nondet_ulonglong();
  __VERIFIER_assume(x > 0xFFFFFFFFFFFFFFF0ull);
  if (x <= 0xFFFFFFFFFFFFFFF0ull)
    reach_error();

  int s = __VERIFIER_nondet_int();
  __VERIFIER_assume(s > 2147483644);
  if (s <= 2147483644)
    reach_error();
  int t = __VERIFIER_nondet_int();
  __VERIFIER_assume(t < -2147483645);
  if (t >= -2147483645)
    reach_error();
  long long w = __VERIFIER_nondet_longlong();
  __VERIFIER_assume(w <= -9223372036854775807LL);
  if (w > -9223372036854775807LL)
    reach_error();

  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n < -5);
  if ((unsigned)n < 0x80000000u)
    reach_error();
  unsigned p = __VERIFIER_nondet_uint();
  __VERIFIER_assume(p < 0x80000000u);
  if ((int)p < 0)
    reach_error();
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assume(y < -5 && y > -100);
  long long z = y;
  if (z >= -5)
    reach_error();
  return 0;
}
