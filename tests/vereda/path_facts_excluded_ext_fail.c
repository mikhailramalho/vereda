// EXPECT: ^VERIFICATION FAILED$
// y is x widened; the branches exclude different values from it, 5 and 6,
// so after the join x may be either: x = 5 when c is 0 reaches the
// assertion (#468).
extern unsigned char __VERIFIER_nondet_uchar(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned char x = __VERIFIER_nondet_uchar();
  int k = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  if (k != 5)
    return 0;
  int y = x;
  if (c) {
    if (y == k)
      return 1;
  } else {
    if (y == k + 1)
      return 2;
  }
  __VERIFIER_assert(x != 5);
  return 3;
}
