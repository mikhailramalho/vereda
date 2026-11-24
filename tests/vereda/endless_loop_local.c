// An endless loop that changes only a local, kept in registers, does
// nothing observable; so does one nested in a loop that ends.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: Unwinding loop 1
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int n = __VERIFIER_nondet_int();
  for (int i = 0; i < 3; i++)
    if (i == n)
      for (int x = n;;)
        if (x > 5)
          x = 0;
        else
          x++;
  __VERIFIER_assert(n < 0 || n >= 3);
  return 0;
}
