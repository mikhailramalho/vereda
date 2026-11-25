// Nested loops inside a function with many sibling blocks exercise
// precomputed per-region block lists.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: VERIFICATION FAILED
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int c = __VERIFIER_nondet_int();
  int base = 0;
  if (c == 0) base = 1;
  else if (c == 1) base = 2;
  else if (c == 2) base = 3;
  else if (c == 3) base = 4;
  else if (c == 4) base = 5;
  else if (c == 5) base = 6;
  else if (c == 6) base = 7;
  else if (c == 7) base = 8;
  else base = 9;

  int sum = 0;
  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j < 5; ++j)
      sum += i * j;
  }
  // sum = (0+1+2+3+4+5) * (0+1+2+3+4) = 15 * 10 = 150
  __VERIFIER_assert(sum == 150 && base >= 1 && base <= 9);
  return 0;
}
