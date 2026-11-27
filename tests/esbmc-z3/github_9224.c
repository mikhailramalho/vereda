// #9224: signed / and % by 2^k truncate toward zero (C11 6.5.5p6). The
// expected values are built from shifts and masks of the magnitude.
int __VERIFIER_nondet_int(void);
long long __VERIFIER_nondet_ll(void);
int main(void)
{
  int x = __VERIFIER_nondet_int();
  long long mx = x < 0 ? -(long long)x : x;
  int q = (int)(mx >> 2), r = (int)(mx & 7);
  __VERIFIER_assert(x / 4 == (x < 0 ? -q : q)); // x / 4
  __VERIFIER_assert(x % 8 == (x < 0 ? -r : r)); // x % 8
  __VERIFIER_assert(x / 1073741824 * 1073741824 + x % 1073741824 == x); // 2^30

  long long y = __VERIFIER_nondet_ll();
  unsigned long long my = y < 0 ? -(unsigned long long)y : y;
  long long qy = (long long)(my >> 1), ry = (long long)(my & 15);
  __VERIFIER_assert(y / 2 == (y < 0 ? -qy : qy)); // y / 2
  __VERIFIER_assert(y % 16 == (y < 0 ? -ry : ry)); // y % 16
  return 0;
}
// Imported from ESBMC regression/z3/github_9224
// RUN: --solver z3
// EXPECT: ^VERIFICATION SUCCESSFUL$
