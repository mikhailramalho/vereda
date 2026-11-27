// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: SUCCESS
// EXPECT: ^  FAILED +\[notOne\.assertion\.1\]
// EXPECT: ^  FAILED +\[main\.assertion\.7\]
// path_facts with each check reachable at the edge of what the path knows,
// or on a path that does not know it (#296): x = y learnt on one side of a
// join, the greatest value x = y and y < 10 leave, an extension's operand at
// its bound, x != c where c is not an end of x's range, a switch case and
// the default at values in range, a load whose index may meet the store
// above it, and an equality through an extension on the path it holds.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern unsigned char __VERIFIER_nondet_uchar(void);
extern short __VERIFIER_nondet_short(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
static int widen(unsigned char c) { return c; }
static long long wide(short s) { return s; }
static void notOne(unsigned v) { __VERIFIER_assert(v != 1); }
int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  if (__VERIFIER_nondet_int())
    __VERIFIER_assume(x == y);
  __VERIFIER_assume(y < 10);
  __VERIFIER_assert(x < 10);
  int p = __VERIFIER_nondet_int(), q = __VERIFIER_nondet_int();
  __VERIFIER_assume(p == q);
  __VERIFIER_assume(q < 10);
  __VERIFIER_assert(p != 9);

  unsigned char c = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(widen(c) < 7);
  __VERIFIER_assert(c != 6);

  unsigned u = __VERIFIER_nondet_uint();
  if (u != 0)
    notOne(u);

  unsigned char k = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(k < 4);
  switch (k) {
  case 0:
  case 1:
    break;
  case 3:
    __VERIFIER_assert(k != 3);
    break;
  default:
    __VERIFIER_assert(k != 2);
  }

  int a[10];
  int i = __VERIFIER_nondet_int(), j = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 6 && j >= 5 && j < 10);
  a[i] = 1;
  a[j] = 2;
  __VERIFIER_assert(a[i] != 2);

  short s = __VERIFIER_nondet_short();
  if (wide(s) == -300)
    __VERIFIER_assert(s != -300);
  return 0;
}
