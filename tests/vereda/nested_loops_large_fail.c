// Nested loops inside a function with many sibling blocks still detect a bug
// in the inner loop.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: VERIFICATION SUCCESSFUL
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
    for (int j = 0; j < 5; ++j) {
      sum += i * j;
      __VERIFIER_assert(!(i == 4 && j == 3 && base == 5));
    }
  }
  return sum;
}
